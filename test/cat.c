#include "syscall.h"

int main(int argc, char ** argv) {
	const int size = 1;
    char buffer[size],
            *originPath = argv[0];
	int i = 1;
    if(argc != 1)
        Exit(-1);
    OpenFileId descriptor = Open(originPath);

    if(descriptor == -1)
        Exit(-1);

    while(i > 0) {
        i = Read(&buffer[0], 1, descriptor);
        Write(&buffer[0], 1, CONSOLE_OUTPUT);
    }
    Close(descriptor);
    Write("\n", 1, CONSOLE_OUTPUT);
	
    Exit(0);
}
