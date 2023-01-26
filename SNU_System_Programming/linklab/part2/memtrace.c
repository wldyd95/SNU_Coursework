//------------------------------------------------------------------------------
//
// memtrace
//
// trace calls to the dynamic memory manager
//
#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memlog.h>
#include <memlist.h>
#include "callinfo.h"

//
// function pointers to stdlib's memory management functions
//
static void *(*mallocp)(size_t size) = NULL;
static void (*freep)(void *ptr) = NULL;
static void *(*callocp)(size_t nmemb, size_t size);
static void *(*reallocp)(void *ptr, size_t size);

//
// statistics & other global variables
//
static unsigned long n_malloc  = 0;
static unsigned long n_calloc  = 0;
static unsigned long n_realloc = 0;
static unsigned long n_allocb  = 0;
static unsigned long n_freeb   = 0;
static item *list = NULL;

//
// init - this function is called once when the shared library is loaded
//
__attribute__((constructor))
void init(void)
{
  char *error;

  LOG_START();

  // initialize a new list to keep track of all memory (de-)allocations
  // (not needed for part 1)
  list = new_list();

  // ...
}

//
// Customize memory functions for memory tracing 
//

// All codes are referenced from textbook
void *malloc(size_t size){
  char *error;
  void *res; // resulting pointer

  /* get address of libc malloc*/
  if(!mallocp){
    mallocp = dlsym(RTLD_NEXT, "malloc");
    if((error = dlerror()) != NULL){
      fputs(error, stderr);
      exit(1);
    }
  }

  res = mallocp(size);
  LOG_MALLOC(size, res);

  n_allocb += (unsigned long)size;
  n_malloc += (unsigned long)1;

  // memory block list handle
  alloc(list, res, size);
  
  return res;
}

void free(void *ptr){
  char *error;

  /* get address of libc free*/
  if(!freep){
    freep = dlsym(RTLD_NEXT, "free");
    if((error = dlerror()) != NULL){
      fputs(error, stderr);
      exit(1);
    }
  }
  
  LOG_FREE(ptr);

  // memory block list handle
  item *freed_block = dealloc(list, ptr);
  n_freeb += (*freed_block).size;

  freep(ptr);
}

void *calloc(size_t nmemb, size_t size){
  char *error;
  void *res; // resulting pointer

  /* get address of libc calloc*/
  if(!callocp){
    callocp = dlsym(RTLD_NEXT, "calloc");
    if((error = dlerror()) != NULL){
      fputs(error, stderr);
      exit(1);
    }
  }

  res = callocp(nmemb, size);
  LOG_CALLOC(nmemb, size, res);

  // memory block list handle
  alloc(list, res, nmemb * size);

  n_allocb += (unsigned long)(size * nmemb);
  n_calloc += (unsigned long)1;

  return res;
}

void *realloc(void *ptr, size_t size){
  char *error;
  void *res; // resulting pointer

  /* get address of libc calloc*/
  if(!reallocp){
    reallocp = dlsym(RTLD_NEXT, "realloc");
    if((error = dlerror()) != NULL){
      fputs(error, stderr);
      exit(1);
    }
  }
  // memory block handle
  // for realloc, we must free first and allocate sequentially
  item *freed_block = dealloc(list, ptr);
  n_freeb += (*freed_block).size;

  res = reallocp(ptr, size);
  LOG_REALLOC(ptr, size, res);

  alloc(list, res, size);

  n_allocb += (unsigned long)(size);
  n_realloc += (unsigned long)1;

  return res;
}

//
// fini - this function is called once when the shared library is unloaded
//
__attribute__((destructor))
void fini(void)
{
  // ...  
  LOG_STATISTICS(n_allocb, (unsigned long)(n_allocb / (n_malloc + n_calloc + n_realloc)), n_freeb); 

  LOG_NONFREED_START();

  // iteration for print
  item *iter = list;

  while(1){
    // only print unfreed memory (If reference count is 0, that memory is already freed, so we consider only nonzero reference count)
    if((*iter).cnt != 0) {
      LOG_BLOCK((*iter).ptr, (*iter).size, (*iter).cnt, (*iter).fname, (*iter).ofs);
    }
    
    if((*iter).next == NULL) {
      break;      
    }

    iter = (*iter).next;
  }

  LOG_STOP();

  // free list (not needed for part 1)
  free_list(list);
}

// ...
