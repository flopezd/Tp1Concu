#include <iostream>
#include <sys/wait.h>
#include "src/Pipe.h"
#include "src/Pilon.h"
#include "src/Mesa.h"
#include "src/Jugador.h"
#include "src/Arbitro.h"

using namespace std;

int simular(int cantidadDeJugadores) {
    string archivo("/bin/bash");
    string archivo2("/bin/chmod");
    MemoriaCompartida<int> turno;
    turno.crear(archivo, 'R');
    turno.escribir(0);
    MemoriaCompartida<int> cantidadManos;
    cantidadManos.crear(archivo2, 'R');
    cantidadManos.escribir(0);

    MemoriaCompartida<int> cantidadCartasPorJugador[cantidadDeJugadores];
    for(int i = 0; i < cantidadDeJugadores; i++) {
        cantidadCartasPorJugador[i].crear(archivo2, i);
    }
    static const string NOMBRE = "/bin/date";
    static const string NOMBRE2 = "/bin/cat";
    static const string NOMBRE3 = "/bin/chown";
    LockFile lock( "Jugador.h" );

    Semaforo tirarCarta(NOMBRE, 0, cantidadDeJugadores);
    Semaforo mirarCarta(NOMBRE2, 0, cantidadDeJugadores);
    Semaforo manos(NOMBRE3, 0, 1);
    Pipe canalOperaciones;
    Pipe canalHaciaLaMesa[cantidadDeJugadores];
    Pipe canalDesdeLaMesa[cantidadDeJugadores];
    Pipe chat[cantidadDeJugadores];

    for (int i = 0; i < cantidadDeJugadores; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            Jugador jugador(chat, &canalOperaciones, &(canalDesdeLaMesa[i]), &(canalHaciaLaMesa[i]), i, cantidadDeJugadores, &turno,
                            &cantidadManos, &tirarCarta, &mirarCarta, &manos, &lock, &(cantidadCartasPorJugador[i]));
            jugador.juntar();
            jugador.jugar();
            exit (0);
        }
    }

    Arbitro arbitro(cantidadCartasPorJugador, cantidadDeJugadores);

    Mesa mesa(&canalOperaciones, canalHaciaLaMesa, canalDesdeLaMesa, cantidadDeJugadores, &manos, &cantidadManos);
    mesa.repartir();
    mesa.jugar();

    turno.liberar();
    tirarCarta.eliminar();
    mirarCarta.eliminar();
    cantidadManos.liberar();

    for (int i = 0; i < cantidadDeJugadores; i++) {
        wait( NULL );
    }
    Logger::getInstance()->loguear("Termino mesa");
}

int main(int argc, char* argv[]) {
    if (argc < 2 || atoi(argv[1]) <= 4 || atoi(argv[1]) % 2 == 1) {
        return -1;
    }
    simular(atoi(argv[1]));
    return 0;
}