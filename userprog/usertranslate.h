#include <machine.h>
#include "system.h"

// Lee una string desde la memoria de usuario.
void readStrFromUsr(int usrAddr, char *outStr);

// Lee un array desde la memoria del usuario.
void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount);

// Traduce una string hacia la memoria de usuario.
void writeStrToUsr(char *str, int usrAddr);

// Traduce un array hacia la memoria de usuario.
void writeBuffToUsr(char *str, int usrAddr, int byteCount);

// Dado un string con argumentos separados por 'divide',
//traduce un argumento y retorna la direccion del proximo
//argumento a traducir.
int readSpecialStringFromUser(int usrAddr, char *outStr, char divide);
