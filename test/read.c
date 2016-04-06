#include "syscall.h"

int main()
{
    char *buff;
    Read(buff,1024, 0);
    Write("holas",5,1);
    return 0;
}
