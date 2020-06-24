#include <stdlib.h>

int main(void){
    void *a = malloc(1024);
    free(a);
    return 0;
}