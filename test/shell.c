#include "syscall.h"

int main()
{
    SpaceId newProc;
    char prompt[12] = "our_shell> ";
    char divideChar = ' ';
    char filepath[32], argv[64];
    int argc = 0, i, j;


    while(1) {
        Write(prompt, 12, ConsoleOutput);
        i = 0;

        do {
            Read(&filepath[i], 1, ConsoleInput); 
            if(filepath[i] == '\0') {
                break;
            } 
            if(filepath[i] == '\n') {
                filepath[i] = '\0';
                break;
            } 
            if(filepath[i] == divideChar) {
                // Arguments...
                filepath[i] = '\0';
                argc++;
                j = 0;
                do {
                    Read(&argv[j], 1, ConsoleInput); 
                    if(argv[j] == divideChar) {
                        argc++;
                    } else if(argv[j] == '\0') {
                        break;
                   
                    } else if(argv[j] == '\n') {
                        argv[j] = '\0';
                        break;
                    }
                    j++;
                } while(1);
                break;
            }
            i++;
        } while(1);

        if(argc > 0) {
            newProc = Exec(filepath, argc, &argv);
            Join(newProc);
        } else {
            newProc = Exec(filepath, 0, '\0');
            Join(newProc);
        }
    }
}
