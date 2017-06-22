#include "syscall.h"

#define amount_arguments 10
#define arguments_length 50
int main() {
    SpaceId commandDescriptor;
    char *prompt = "Nachos_Shell> ";
    char  buffer[arguments_length],
            argv[amount_arguments];
    int i,j,t,
            join = 0,
            argc = 0,
            bufferSize = 0;

	while(1) {
        Write(prompt, 14, CONSOLE_OUTPUT);
		i = 0;
		while(1) {
            t = Read(&buffer[i], 1, CONSOLE_INPUT);
            while(buffer[0] == ' ') // Quitamos espacios en blanco del principio
                t = Read(&buffer[0], 1, CONSOLE_INPUT);
		
            if(buffer[i] == '\0') // Fin de linea
				break;
            if(buffer[i] == ' ') { // Detectamos el primer argumento
                buffer[i] = '\0';
				argc++;
				bufferSize = i;
				i=0;
				do {
                    t = Read(&argv[i], 1, CONSOLE_INPUT);
                    if(argv[i] == ' ') // Proximo argumento.
						argc++;
                    if(argv[i] == '\0') // Fin de los argumentos.
						break;
					i++;
				} while(1);
				break;
			}
		
            if(buffer[i] == '&' && i==0) { // Ejecutamos en background
				join = 1;
			} else
				i++;
		}
	
        if(buffer[0] == 'q') { // Salimos de la consola
            Write("\nSaliendo del Nachos_Shell\n", 27, CONSOLE_OUTPUT);
			break;
		}

        Write(buffer, bufferSize, CONSOLE_OUTPUT);
        if(i > 0 && argc == 0) {
            commandDescriptor = Exec(buffer, argc, '\0');
        } else if(i>0 && argc > 0) {
            commandDescriptor = Exec(buffer, argc, (void *) argv);
		}
        if(join == 0 && commandDescriptor != -1) // Ejecucion normal (not background)
            Join(commandDescriptor);
        for(j = 0; j < amount_arguments; j++) {
            argv[j] = '\0';
		}
        for(j = 0; j < arguments_length; j++) {
                buffer[j] = '\0';
        }
		argc = 0;
	}
	Exit(0);
}
