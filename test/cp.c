#include "syscall.h"

int main(int argc, char ** argv) {
    char *originPath = argv[0],
            *destinyPath = argv[1];
    char buffer[1];
    int i = 1;
    OpenFileId from,
            to;


    from = Open(originPath);
    if(from == -1)
        Exit(-1);

    Create(destinyPath);
    to = Open(destinyPath);
    if(to == -1)
        Exit(-1);

    while(i > 0) {
        i = Read(&buffer[0], 1, from);
        Write(&buffer[0], 1, to);
    }

    Close(from);
    Close(to);
    Write("\n", 1, CONSOLE_OUTPUT);

    Exit(0);
}
