#include "syscall.h"


int main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char *prompt ="Nachos_Shell> ";
    char  ch, buffer[60],argv[60];
    int i,t,join=0,argc=0;
  
    while(1) {
        Write(prompt, 14, output);
                                                    
        i = 0;
        
        while(1) {
            t = Read(&buffer[i], 1, input); 
            Write(&buffer[i], t, output);
                                                                                       
            if(buffer[i] == '\0') {
                break;
            }
            if(buffer[0]=='q' && buffer[1]=='\0') {
                Write("\nSaliendo del Shell\n", 21, output);
                Exit(0);
            }
            if(buffer[i] == ' ') {
           /*Argumentos */
                buffer[i]='\0';
                argc++;
                i=0;
                do {
                    t = Read(&argv[i],1,input);
                    if(argv[i] == ' ') {
                        argc++;
                    }
                    if(argv[i] == '\0') {
                        break;
                    }
                    i++;
                } while(1);

                break;
            }

            if(buffer[i] == '&'&& i==0) {
                join = 1;
            } else
                i++;
        } 

        if( i > 0 && argc == 0) {
            newProc = Exec(buffer, 0,"");
            if(join == 0)
                Join(newProc);
        } else if(i>0 && argc > 0) {
            newProc = Exec(buffer, argc, &argv);
            if(join == 0)
                Join(newProc);
        }
    }
}
