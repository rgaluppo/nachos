#include "syscall.h"
//#include <stdio.h>

int main()
{
    char *buff[24];
    Read(buff, 10, 0);
    Write(buff, 10, 1);
 //   printf("esto es lo que escribimos en la consola: \n\t%s",buff);
    return 0;
}
