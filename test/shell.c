#include "syscall.h"

int main()
{
    SpaceId newProc;
    char prompt[12] = "our_shell> ";
    char divideChar = ' ';
    char filepath[32], argv[64];
    int argc = 0, i = 0, j = 0;


    while(1) {
        Write(prompt, 12, ConsoleOutput);
        
        do {
            Read(&filepath[i], 1, ConsoleInput); 
            if(filepath[i] == divideChar) {
                argc++;
                while(1){
                    Read(&argv[i], 1, ConsoleInput); 
                    if(argv[j] == divideChar) {
                        argc++;
                    }
                    if(argv[j] == '\n')
                        break;
                }
            }

        } while( filepath[i++] != '\n');
        filepath[--i] = '\0';

        if(argc > 0) {
            newProc = Exec(filepath, argc, &argv);
            Join(newProc);
        } else {
            newProc = Exec(filepath, 0, '\0');
            Join(newProc);
        }
    }
}
