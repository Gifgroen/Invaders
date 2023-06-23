#include <stdio.h>

int main(int Argc, char *Args[])
{
#if DEBUG 
    printf("[Debug] %d arguments passed. ", Argc);
#endif
    printf("Invaders!\n");
    return 0;
}
