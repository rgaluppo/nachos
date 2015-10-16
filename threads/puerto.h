#include "copyright.h"
#include "synch.h"


// Esta clase implementa un sistema de mensajes entre hilos, el cual permite que los emisores se
// sincronicen con los receptores.
//
// Puerto::Send(int mensaje) espera atomicamente hasta que se llama a 
// Puerto::Receive(int *mensaje), y luego copia el mensaje int en el buffer de Receive. Una vez 
// hecha la copia, ambos pueden retornar. 
//
// La llamada Receive tambien es bloqueante (espera a que se ejecute un Send, si no habia
// ningun emisor esperando).
//
class Puerto {
    public:
        Puerto(const char* name);
        ~Puerto();
        const char* getName() { return portname; }	// para depuraci�n

        void Send(int mensaje);

        void Receive(int* correo);

    private:
       const char* portname;
       Condition* enviando;   // Variable de condicion para los emisores.
       Condition* recibiendo; // Variable de condicion para los receptores.
       Lock* lockPort;	// Usamos el mismo candado para las dos variables de condicion.
       int* buffer;     // Apunta al buffer que nos da un receptor donde se guardan los mensajes.
       int lengthEnv;	// Nos sirve para indicar si hay emisores que enviaron un mensaje.
};
