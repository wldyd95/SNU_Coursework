#define  UNW_LOCAL_ONLY
#include <stdlib.h>
#include <stdio.h>
#include <libunwind.h>  
#include <string.h>

// In our environment, size of call instruction is 5 byte
#define size_of_call_instruction 5 

// reference from https://eli.thegreenplace.net/2015/programmatic-access-to-the-call-stack-in-c/

int get_callinfo(char *fname, size_t fnlen, unsigned long long *ofs)
{
  unw_cursor_t cursor;
  unw_context_t context;

  // Initialize cursor to current frame for local unwinding
  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  // Unwind frames one by one, going up the frame stack
  while (unw_step(&cursor) > 0) {
    unw_word_t offset;

    char sym[256];
    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) { // return 0 if successful

      if(strcmp(sym, "main") == 0){ // Meet main function in test case
        strcpy(fname, sym); // fname becomes main, use strcpy(destination, origin)
        *ofs = offset - size_of_call_instruction; // calculated offset indicates next intruction PC, so we should substract size of call instruction
        return 0; // successfully get call info
      }

    } else { // error while unwinding
      return -1;
    }
  }

  return -1; // failed to get callinfo
}
