/*
 * SNU CSE System programming 2020 Spring
 * malloclab
 * 2013-10958 & user8
 * Customized memory allocator with high performance
 * In this memory allocator, we use explicit sixteen segregated free lists for high performance (If needed, we can use more segregated list)
 * Therefore, we implement each block with header and footer
 * Header includes size, allocation bit as in the text book
 * Footer includes size, allocation bit as in the text book
 * We also need to include pred(predecessor) and succ(successor) pointer to manipulate segregated free list
 * Size of segregated list headers is power of two: 1, 2, 4, 8 ... (That is, we will use the buddy system)
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
/*********************************************************
 * NOTE TO STUDENTS : Before you do anything else, please
 * provide your team information in the following struct.
 * Student ID : 2013-10958
 * User ID : user8
 ********************************************************/

/* Single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* Rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)   

/* Additional macros from textbook 
 * Basic constants and macros 
 */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* One page size */
#define MAX_LIST_NUMBER 16 /* Maximum segregated free list number (start from 0!) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = val)

/* Read the size and allocated(reallocated) fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
                                      
/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)                                       

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/* Given block ptr bp, get pointer of predecessor and successor blocks in segregated free list */
#define PRED_PTR(bp) ((char *)(bp))
#define SUCC_PTR(bp) ((char *)(bp) + WSIZE)

/* Get real address value of predecessor and successor blocks in segregated free list */
#define PRED(bp) (*(char **)(PRED_PTR(bp)))
#define SUCC(bp) (*(char **)(SUCC_PTR(bp)))

/* Make given bp points ptr: used to make new links in segregated free list */
#define POINT(bp, ptr) (*(unsigned int *)(bp) = (unsigned int)(ptr))

/* Global variable: segreaged free list 
 * We will use seven segregated free list, each free list's size is 1, 2, 4, 8, 16, ...  (Buddy system)
 */
#define SIZE0 1
#define SIZE1 2
#define SIZE2 4
#define SIZE3 8
#define SIZE4 16
#define SIZE5 32
#define SIZE6 64
#define SIZE7 128
#define SIZE8 256
#define SIZE9 512
#define SIZE10 1024
#define SIZE11 2048
#define SIZE12 4096
#define SIZE13 8192
#define SIZE14 16384
#define SIZE15 32768
#define SIZE16 65536 /* Maximum 65535 bytes can be allocated (Can be made larger if needed) */

///////////////////////////////////////////////////////////////////////////////////////////////
/* Structure of allocated and free blocks */
/*
 a: allocation bit (1: true, 0: false)

 < Allocated block >
 
             31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    Header: |                               Size of block (8 byte aligned)                         |  |  | a|
      bp -> +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
            |                                                                                               |
            |                               Payload (possibly with padding)                                 |   
            .                                                                                               .
            .                                                                                               .
            .                                                                                               .
            |                                                                                               |
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    Footer: |                               Size of block (8 byte aligned)                         |  |  | a|
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


 < Freed block >

             31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    Header: |                               Size of block (8 byte aligned)                         |  |  | a|
      bp -> +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
            |                       Address of predecessor in segregated free list                          |
bp+WSIZE -> +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
            |                        Address of successor in segregated free list                           |
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+                                                                           |
            |                               Payload (possibly with padding)                                 |   
            .                                                                                               .
            .                                                                                               .
            .                                                                                               .
            |                                                                                               |
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    Footer: |                               Size of block (8 byte aligned)                         |  |  | a|
            +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+



*/
///////////////////////////////////////////////////////////////////////////////////////////////

static void* segregated_free_list0;
static void* segregated_free_list1;
static void* segregated_free_list2;
static void* segregated_free_list3;
static void* segregated_free_list4;
static void* segregated_free_list5;
static void* segregated_free_list6;
static void* segregated_free_list7;
static void* segregated_free_list8;
static void* segregated_free_list9;
static void* segregated_free_list10;
static void* segregated_free_list11;
static void* segregated_free_list12;
static void* segregated_free_list13;
static void* segregated_free_list14;
static void* segregated_free_list15;
static void* segregated_free_list16;

/* Used for efficient place and split
 * If size is larger than threshold, split and use the right block (vice versa)
 * Threshold value is calculated for every malloc 
 */
static size_t threshold; 
static size_t op_num;

/* Partially referenced from the text book 
 * Utility functions for memory allocator 
 * add_block & remove_block functions are implemented to use segregated free list 
 */
static void add_block(void *bp, size_t size);
static void remove_block(void *bp);
static void *extend_heap(size_t size);
static void *coalesce(void *bp);
static void *place(void *bp, size_t asize);

/* Implemented to manipulate segeregated free list */
static void update_list(int list_number, void *bp);
static void *get_list(int list_number);

/* Implemented for heap consistency check */
// static int mm_check(char check_mode, void *bp);
// static int address_valid_check(void *bp);
// static int overlap_check(void *bp);
// static int header_footer_check(void *bp);
// static int free_list_check(void *bp);
// static int coalesce_check(void *bp);

/* Update selected segregated free list to bp*/
static void update_list(int list_number, void *bp){
    switch (list_number)
    {
    case 0:
        segregated_free_list0 = bp;
        break;

    case 1:
        segregated_free_list1 = bp;
        break;

    case 2:
        segregated_free_list2 = bp;
        break;

    case 3:
        segregated_free_list3 = bp;
    break;

    case 4:
        segregated_free_list4 = bp;
    break;

    case 5:
        segregated_free_list5 = bp;
    break;

    case 6:
        segregated_free_list6 = bp;
    break;

    case 7:
        segregated_free_list7 = bp;
    break;

    case 8:
        segregated_free_list8 = bp;
    break;    
    
    case 9:
        segregated_free_list9 = bp;
        break;

    case 10:
        segregated_free_list10 = bp;
    break;

    case 11:
        segregated_free_list11 = bp;
    break;

    case 12:
        segregated_free_list12 = bp;
    break;

    case 13:
        segregated_free_list13 = bp;
    break;

    case 14:
        segregated_free_list14 = bp;
    break;

    case 15:
        segregated_free_list15 = bp;
    break;

    case 16:
        segregated_free_list16 = bp;
    break;

    default:
        break;
    }
    return;
}

/* Return segregated free list with list_number */
static void *get_list(int list_number){
    switch (list_number)
    {
    case 0:
        return segregated_free_list0;
        break;
    case 1:
        return segregated_free_list1;
        break;
    case 2:
        return segregated_free_list2;
        break;
    case 3:
        return segregated_free_list3;
        break;
    case 4:
        return segregated_free_list4;
        break;
    case 5:
        return segregated_free_list5;
        break;
    case 6:
        return segregated_free_list6;
        break;
    case 7:
        return segregated_free_list7;
        break;
    case 8:
        return segregated_free_list8;
        break;
    case 9:
        return segregated_free_list9;
        break;
    case 10:
        return segregated_free_list10;
        break;
    case 11:
        return segregated_free_list11;
        break;
    case 12:
        return segregated_free_list12;
        break;
    case 13:
        return segregated_free_list13;
        break;
    case 14:
        return segregated_free_list14;
        break;
    case 15:
        return segregated_free_list15;
        break;
    case 16:
        return segregated_free_list16;
        break;
    default:
        return NULL;
        break;
    }
}


/* Add new block to appropriate segregated free list */
static void add_block(void *bp, size_t size){
    void* target_list = NULL;
    void* found_location = NULL; /* Location to insert new blocks */
    int list_number;

    /* Find target segregated free list */
    if(size >= SIZE0 && size < SIZE1) target_list = segregated_free_list0, list_number = 0;
    else if(size >= SIZE1 && size < SIZE2) target_list = segregated_free_list1, list_number = 1;
    else if(size >= SIZE2 && size < SIZE3) target_list = segregated_free_list2, list_number = 2;
    else if(size >= SIZE3 && size < SIZE4) target_list = segregated_free_list3, list_number = 3;
    else if(size >= SIZE4 && size < SIZE5) target_list = segregated_free_list4, list_number = 4;
    else if(size >= SIZE5 && size < SIZE6) target_list = segregated_free_list5, list_number = 5;
    else if(size >= SIZE6 && size < SIZE7) target_list = segregated_free_list6, list_number = 6;
    else if(size >= SIZE7 && size < SIZE8) target_list = segregated_free_list7, list_number = 7;
    else if(size >= SIZE8 && size < SIZE9) target_list = segregated_free_list8, list_number = 8;
    else if(size >= SIZE9 && size < SIZE10) target_list = segregated_free_list9, list_number = 9;
    else if(size >= SIZE10 && size < SIZE11) target_list = segregated_free_list10, list_number = 10;
    else if(size >= SIZE11 && size < SIZE12) target_list = segregated_free_list11, list_number = 11;
    else if(size >= SIZE12 && size < SIZE13) target_list = segregated_free_list12, list_number = 12;
    else if(size >= SIZE13 && size < SIZE14) target_list = segregated_free_list13, list_number = 13;
    else if(size >= SIZE14 && size < SIZE15) target_list = segregated_free_list14, list_number = 14;
    else if(size >= SIZE15 && size < SIZE16) target_list = segregated_free_list15, list_number = 15;
    else target_list = segregated_free_list16, list_number = 16;

    /* We use target_list for search pointer (start from the end of the list!) 
     * Each segregated free list maintains size increasing order from the end 
     */
    while((target_list != NULL) && (size > GET_SIZE(HDRP(target_list)))){
        found_location = target_list;
        target_list = PRED(target_list);
    }


    /* Set predecessor and successor pointer 
     * Block order should be target_list -> bp -> found_location 
     */
    if(target_list == NULL){ 
        if(found_location == NULL){ /* Add first block to the target segregated free list */
            POINT(PRED_PTR(bp), NULL);
            POINT(SUCC_PTR(bp), NULL);
            
            /* Update end address of segregated free list */
            update_list(list_number, bp);
        }

        else{ /* Largest block: add at the root of the list */
            POINT(PRED_PTR(bp), NULL);
            POINT(SUCC_PTR(bp), found_location);
            POINT(PRED_PTR(found_location), bp);
        }        
    }

    else{ 
        if(found_location == NULL){ /* Smallest block: add at the end of the list */
            POINT(PRED_PTR(bp), target_list);
            POINT(SUCC_PTR(bp), NULL);
            POINT(SUCC_PTR(target_list), bp);

            /* Update end address of segregated free list */
            update_list(list_number, bp);
        }

        else{ /* Normal case */
            POINT(PRED_PTR(bp), target_list);
            POINT(SUCC_PTR(bp), found_location);
            POINT(SUCC_PTR(target_list), bp);
            POINT(PRED_PTR(found_location), bp);
        }
    }

    return;
}


static void remove_block(void *bp){
    int list_number;
    size_t size = GET_SIZE(HDRP(bp));

    /* Find target segregated free list */
    if(size >= SIZE0 && size < SIZE1) list_number = 0;
    else if(size >= SIZE1 && size < SIZE2) list_number = 1;
    else if(size >= SIZE2 && size < SIZE3) list_number = 2;
    else if(size >= SIZE3 && size < SIZE4) list_number = 3;
    else if(size >= SIZE4 && size < SIZE5) list_number = 4;
    else if(size >= SIZE5 && size < SIZE6) list_number = 5;
    else if(size >= SIZE6 && size < SIZE7) list_number = 6;
    else if(size >= SIZE7 && size < SIZE8) list_number = 7;
    else if(size >= SIZE8 && size < SIZE9) list_number = 8;
    else if(size >= SIZE9 && size < SIZE10) list_number = 9;
    else if(size >= SIZE10 && size < SIZE11) list_number = 10;
    else if(size >= SIZE11 && size < SIZE12) list_number = 11;
    else if(size >= SIZE12 && size < SIZE13) list_number = 12;
    else if(size >= SIZE13 && size < SIZE14) list_number = 13;
    else if(size >= SIZE14 && size < SIZE15) list_number = 14;
    else if(size >= SIZE15 && size < SIZE16) list_number = 15;
    else list_number = 16;

    /* Adjust predecessor and successor */
    if(PRED(bp) == NULL){
        if(SUCC(bp) == NULL){ /* Only one block in the list, make the list empty */
            update_list(list_number, NULL);
        }

        else{ /* Remove root block from the list */
            POINT(PRED_PTR(SUCC(bp)), NULL);
        }
    }

    else{
        if(SUCC(bp) == NULL){ /* Remove end block from the list */
            POINT(SUCC_PTR(PRED(bp)), NULL);
            update_list(list_number, PRED(bp));
        }

        else{ /* Normal remove */
            POINT(SUCC_PTR(PRED(bp)), SUCC(bp));
            POINT(PRED_PTR(SUCC(bp)), PRED(bp));
        }
    }

    return;
}


/* Extend heap with given size (bytes) */
static void *extend_heap(size_t size){
    void *bp;
    size_t a_size; /* 8 byte aligned size */

    a_size = ALIGN(size);
    if( (bp = mem_sbrk(a_size)) == (void *)-1 ){ /* Failed to mem_sbrk */
        return NULL;
    }

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(a_size, 0)); /* Free block header */
    PUT(FTRP(bp), PACK(a_size, 0)); /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    add_block(bp, a_size);

    /* Coalesce if the previous block was free */
    return coalesce(bp);    
}

/* Coalesce freed blocks */
static void *coalesce(void *bp){
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc){ /* Case 1 */
        return bp;
    }

    else if(prev_alloc && !next_alloc){ /* Case 2 */
        /* Remove old blocks from the segregated free list */
        remove_block(bp);
        remove_block(NEXT_BLKP(bp)); 

        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        add_block(bp, size); /* Add new coalesced block to the segregated free list */
    }

    else if(!prev_alloc && next_alloc){ /* Case 3 */
        /* Remove old blocks from the segregated free list */
        remove_block(bp); 
        remove_block(PREV_BLKP(bp));

        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);

        add_block(bp, size); /* Add new coalesced block to the segregated free list */
    }

    else{ /* Case 4 */
        /* Remove old blocks from the segregated free list */
        remove_block(bp); 
        remove_block(PREV_BLKP(bp));
        remove_block(NEXT_BLKP(bp));

        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp); 

        add_block(bp, size); /* Add new coalesced block to the segregated free list */
    }

    return bp;
}

/* Place given size to bp : minimum block size is 16 bytes (4 words) */
static void *place(void *bp, size_t asize){
    size_t csize = GET_SIZE(HDRP(bp));

    size_t remain_size = (csize - asize);

    if( remain_size <= (2 * DSIZE)){ /* Do not split since remain_size is too small */
        /* Remove old blocks from the segregated free list */
        remove_block(bp);

        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        return bp;
    }

    /* Split the block if remain_size is larger than minimum block size */
    else if(asize >= threshold){ /* Split and place to the right block */
        /* Remove old blocks from the segregated free list */
        remove_block(bp);
        
        PUT(HDRP(bp), PACK(remain_size, 0));
        PUT(FTRP(bp), PACK(remain_size, 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(asize, 1));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(asize, 1));
        add_block(bp, remain_size); /* Add new coalesced block to the segregated free list */
        
        return NEXT_BLKP(bp); /* Return right block */
    }

    else{ /* Split and place to the left block */
        /* Remove old blocks from the segregated free list */
        remove_block(bp);
        
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(remain_size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(remain_size, 0));
        add_block(NEXT_BLKP(bp), remain_size); /* Add new coalesced block to the segregated free list */

        return bp; /* Return left block */
    }
}



/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{   
    /* Initialization */
    threshold = 0;
    op_num = 0;

    char *heap_listp;

    /* Initialize each segregated free list */
    segregated_free_list0 = NULL;
    segregated_free_list1 = NULL;
    segregated_free_list2 = NULL;
    segregated_free_list3 = NULL;
    segregated_free_list4 = NULL;
    segregated_free_list5 = NULL;
    segregated_free_list6 = NULL;
    segregated_free_list7 = NULL;
    segregated_free_list8 = NULL;
    segregated_free_list9 = NULL;
    segregated_free_list10 = NULL;
    segregated_free_list11 = NULL;
    segregated_free_list12 = NULL;
    segregated_free_list13 = NULL;
    segregated_free_list14 = NULL;
    segregated_free_list15 = NULL;
    segregated_free_list16 = NULL;
    
    /* Create the initial emtpy heap */
    if( (heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1 ) return -1; /* Failed to initialize */

    PUT(heap_listp, 0); /* Alignment padding */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1)); /* Epilogue header */
    heap_listp += (2 * WSIZE);

    return 0; /* Successful initialization */
}


/* mm_malloc - Allocate a block by incrementing the brk pointer.
 * Always allocate a block whose size is a multiple of the alignment.
 * Return 8 byte aligned bp 
 */
void *mm_malloc(size_t size)
{
    size_t a_size; /* 8 byte aligned size */

    if((size + DSIZE) <= 2 * DSIZE) a_size = 2 * DSIZE; /* Minimum block bytes are 16 bytes (2 * DSIZE) */
    else a_size = ALIGN(size + DSIZE); /* DSIZE is used for header and footer */

    void* target_list = NULL;
    int list_number;

    /* Find target segregated free list */
    if(a_size >= SIZE0 && a_size < SIZE1) target_list = segregated_free_list0, list_number = 0;
    else if(a_size >= SIZE1 && a_size < SIZE2) target_list = segregated_free_list1, list_number = 1;
    else if(a_size >= SIZE2 && a_size < SIZE3) target_list = segregated_free_list2, list_number = 2;
    else if(a_size >= SIZE3 && a_size < SIZE4) target_list = segregated_free_list3, list_number = 3;
    else if(a_size >= SIZE4 && a_size < SIZE5) target_list = segregated_free_list4, list_number = 4;
    else if(a_size >= SIZE5 && a_size < SIZE6) target_list = segregated_free_list5, list_number = 5;
    else if(a_size >= SIZE6 && a_size < SIZE7) target_list = segregated_free_list6, list_number = 6;
    else if(a_size >= SIZE7 && a_size < SIZE8) target_list = segregated_free_list7, list_number = 7;
    else if(a_size >= SIZE8 && a_size < SIZE9) target_list = segregated_free_list8, list_number = 8;
    else if(a_size >= SIZE9 && a_size < SIZE10) target_list = segregated_free_list9, list_number = 9;
    else if(a_size >= SIZE10 && a_size < SIZE11) target_list = segregated_free_list10, list_number = 10;
    else if(a_size >= SIZE11 && a_size < SIZE12) target_list = segregated_free_list11, list_number = 11;
    else if(a_size >= SIZE12 && a_size < SIZE13) target_list = segregated_free_list12, list_number = 12;
    else if(a_size >= SIZE13 && a_size < SIZE14) target_list = segregated_free_list13, list_number = 13;
    else if(a_size >= SIZE14 && a_size < SIZE15) target_list = segregated_free_list14, list_number = 14;
    else if(a_size >= SIZE15 && a_size < SIZE16) target_list = segregated_free_list15, list_number = 15;
    else target_list = segregated_free_list16, list_number = 16;

    if(size == 0) return NULL; /* Invalid size: size is 0  */

    int found_flag = 0; /* If found appropriate block successfully, found_flag is 1 */

    /* Search appropriate block in the segregated free lists */
    while(list_number <= MAX_LIST_NUMBER){
        
        while(target_list != NULL){
            if(GET_SIZE(HDRP(target_list)) >= a_size) { /* Found appropriate block */
                found_flag = 1;
                break; 
            }
            target_list = PRED(target_list);
        }

        if(found_flag == 1) break;
        
        /* Failed to find block */
        list_number++;
        target_list = get_list(list_number); /* Move to larger segregated free lists */
    }

    if(target_list == NULL) { /* Failed to find appropriate block */
        target_list = extend_heap(MAX(a_size, CHUNKSIZE));
        if(target_list == NULL) return NULL; /* Failed to extend heap */
    }

    /* Update threshold before place */
    if(op_num > 0 && (op_num %2) == 1){ /* Only update when given op_num is even number */
        threshold = (threshold * op_num + a_size); 
        op_num ++;
        threshold /= op_num; /* Calculate average */
    }

    else op_num++;

    /* Allocate given size to the selected blocks (If needed, splitting is done by place function) */
    target_list = place(target_list, a_size);

    /* Heap consistency check */
    // if(!mm_check('a', target_list)) exit(0);
    
    return target_list;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{   
    size_t size_free = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size_free, 0));
    PUT(FTRP(ptr), PACK(size_free, 0));

    add_block(ptr, size_free);
    ptr = coalesce(ptr);

    /* Heap consistency check */
    // if(!mm_check('f', ptr)) exit(0);

    return;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *new_ptr = ptr; /* new allocated pointer */
    size_t current_block_size = GET_SIZE(HDRP(ptr)); /* Current block size */
    size_t next_block_size; /* Next block size */
    size_t new_block_size; /* Aligned size */

    /* Align size */
    if((size + DSIZE) <= 2 * DSIZE) new_block_size = 2 * DSIZE; /* Minimum block bytes are 16 bytes (2 * DSIZE) */
    else new_block_size = ALIGN(size + DSIZE); /* DSIZE is used for header and footer */

    /* Exceptional case */
    if(ptr == NULL) return mm_malloc(new_block_size); /* If ptr is NULL, realloc functions as malloc */
    else if(size == 0){ /* If size is 0, realloc functions as free */
        mm_free(ptr);
        return NULL;
    }

    int remain_size = current_block_size - new_block_size;
    int extend_size;
    
    if(remain_size < 0){ /* Reallocation byte is larger than current bytes */
        if(!GET_SIZE(HDRP(NEXT_BLKP(ptr)))){ /* Next block is epilogue block */
            /* Extend heap size */
            extend_size = MAX(-remain_size, CHUNKSIZE);
            if(extend_heap(extend_size) == NULL) return NULL; /* Failed to extend heap */

            remove_block(NEXT_BLKP(ptr));
            PUT(HDRP(ptr), PACK(current_block_size + extend_size, 1)); 
            PUT(FTRP(ptr), PACK(current_block_size + extend_size, 1));
            memmove(new_ptr, ptr, current_block_size - DSIZE); /* Copy payload (Use memmove to avoid overlapping) */

            /* Heap consistency check */
            // if(!mm_check('a', new_ptr)) exit(0);
        }


        else if(!GET_ALLOC(HDRP(NEXT_BLKP(ptr)))){ /* Next block is freed block */
            next_block_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
            remain_size = current_block_size + next_block_size - new_block_size;

            if(remain_size >= 0){ /* (Current block + Next block) is large enough for reallocation */
                remove_block(NEXT_BLKP(ptr));
                PUT(HDRP(ptr), PACK(current_block_size + next_block_size, 1)); 
                PUT(FTRP(ptr), PACK(current_block_size + next_block_size, 1)); 
                memmove(new_ptr, ptr, current_block_size - DSIZE); /* Copy payload (Use memmove to avoid overlapping ) */

                /* Heap consistency check */
                // if(!mm_check('a', new_ptr)) exit(0);
            }
        }

        /* Next block is allocated
         * Or Next block is free but (Current block + Next block) is not large enough for reallocation
         * In this case, find free space in the segregated free lists using our mm_malloc
         */
        else{ 
            new_ptr = mm_malloc(size);
            memcpy(new_ptr, ptr, current_block_size - DSIZE); /* Copy payload */
            mm_free(ptr);
        }
    }

    /* Reallocation bytes is smaller than previous bytes */
    else{
        memmove(new_ptr, ptr, new_block_size - DSIZE); /* Copy payload */

        /* Heap consistency check */
        // if(!mm_check('a', new_ptr)) exit(0);
    }

    return new_ptr;
}


// /* Heap consistency check function */
// static int mm_check(char check_mode, void *bp){
//     if(check_mode == 'a'){ /* Allocation mode: check heap consistency except free list */
//         if(!address_valid_check(bp) || !overlap_check(bp) || !header_footer_check(bp)) {
//             printf("%s\n", "Heap consistency violated for allocation!");
//             return 0;
//         }
//     }

//     else if(check_mode == 'f'){ /* Free mode: check heap consistency with free list */
//         if(!address_valid_check(bp) || !overlap_check(bp) || !header_footer_check(bp) || !free_list_check(bp) || !coalesce_check(bp)) {
//             printf("%s\n", "Heap consistency violated for free!");
//             return 0;
//         }
//     }

//     return 1; /* Heap consistency check success! */
// }

// /* Check if address is in heap range */
// static int address_valid_check(void *bp){ 
//     if(bp < mem_heap_lo() || bp > mem_heap_hi()) { /* Address is out of heap range */
//         printf("%s\n", "Address is out of valid heap range!");
//         return 0; 
//     }

//     return 1; 
// }

// /* Check if there is overlap between adjacent blocks */
// static int overlap_check(void *bp){
//     if(HDRP(NEXT_BLKP(bp)) < FTRP(bp) || FTRP(PREV_BLKP(bp)) > HDRP(bp)){ /* Overlapping occurs */
//         printf("%s\n", "Overlap occurred between adjacent blocks!");
//         printf("%p\n", HDRP(NEXT_BLKP(bp)));
//         printf("%p\n", FTRP(bp));

//         printf("%p\n", HDRP(bp));
//         printf("%p\n", FTRP(PREV_BLKP(bp)));
//         return 0;
//     }

//     return 1;
// }   

// /* Check if header and footer information is the same */
// static int header_footer_check(void *bp){
//     if(GET_SIZE(HDRP(bp)) != GET_SIZE(FTRP(bp))){ /* Size information is different */
//         printf("%s\n", "Different size information of header and footer!");
//         return 0;
//     }

//     else if(GET_ALLOC(HDRP(bp)) != GET_ALLOC(FTRP(bp))){ /* Alloc information is different */
//         printf("%s\n", "Different alloc information of header and footer!");
//         return 0;
//     }

//     return 1;
// }

// /* Check validity of free list */
// static int free_list_check(void *bp){
//     size_t size = GET_SIZE(HDRP(bp));

//     void* target_list = NULL;

//     /* Find target segregated free list */
//     if(size >= SIZE0 && size < SIZE1) target_list = segregated_free_list0;
//     else if(size >= SIZE1 && size < SIZE2) target_list = segregated_free_list1;
//     else if(size >= SIZE2 && size < SIZE3) target_list = segregated_free_list2;
//     else if(size >= SIZE3 && size < SIZE4) target_list = segregated_free_list3;
//     else if(size >= SIZE4 && size < SIZE5) target_list = segregated_free_list4;
//     else if(size >= SIZE5 && size < SIZE6) target_list = segregated_free_list5;
//     else if(size >= SIZE6 && size < SIZE7) target_list = segregated_free_list6;
//     else if(size >= SIZE7 && size < SIZE8) target_list = segregated_free_list7;
//     else if(size >= SIZE8 && size < SIZE9) target_list = segregated_free_list8;
//     else if(size >= SIZE9 && size < SIZE10) target_list = segregated_free_list9;
//     else if(size >= SIZE10 && size < SIZE11) target_list = segregated_free_list10;
//     else if(size >= SIZE11 && size < SIZE12) target_list = segregated_free_list11;
//     else if(size >= SIZE12 && size < SIZE13) target_list = segregated_free_list12;
//     else if(size >= SIZE13 && size < SIZE14) target_list = segregated_free_list13;
//     else if(size >= SIZE14 && size < SIZE15) target_list = segregated_free_list14;
//     else if(size >= SIZE15 && size < SIZE16) target_list = segregated_free_list15;
//     else target_list = segregated_free_list16;

//     /* Travel free list until we find bp */
//     while(target_list!=NULL){
//         /* Check allocation information of each block */
//         if(GET_ALLOC(HDRP(target_list))){
//             printf("%s\n", "Some allocated block is in the free list!");
//             return 0;
//         }

//         if(target_list != bp) target_list = PRED(target_list);

//         else return 1; /* Found bp in the free list*/
//     }

//     /* Failed to find bp in the free list */
//     printf("%s\n", "Some freed block is not in the free list!");
//     return 0;
// }

// static int coalesce_check(void *bp){
//     int alloc_left;
//     int alloc_right;

//     alloc_left = GET_ALLOC(HDRP(PREV_BLKP(bp)));
//     alloc_right = GET_ALLOC(HDRP(NEXT_BLKP(bp)));

//     if(!alloc_left || !alloc_right) {
//         printf("%s\n", "More coalescing is needed for freed blocks!");
//         return 0;
//     }

//     return 1;
// }