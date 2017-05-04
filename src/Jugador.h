#ifndef TP1_JUGADOR_H
#define TP1_JUGADOR_H

#include "Pipe.h"
#include "Pilon.h"
#include "Mesa.h"
#include "MemoriaCompartida.h"
#include "Semaforo.h"
#include "LockFile.h"
#include "Logger.h"

class Jugador {
private:
    Pipe* canalOperaciones;
    Pipe* chat;
    Pipe* canalEntrada;
    Pipe* canalSalida;
    Pilon pilon = Pilon(false);
    int cantidadJugadores;
    const unsigned short identificador;
    MemoriaCompartida<int>* turno;
    MemoriaCompartida<int>* cantidadManos;
    Semaforo* tirarCarta;
    Semaforo* mirarCarta;
    Semaforo* manos;
    Carta ultimaCarta;
    LockFile* lock;
    MemoriaCompartida<int>* cantidadCartas;
    Logger* logger;

    void ponerCarta(int nroTurno);
    void actuar();
    void ponerLasManos();
    void comunicar(std::string mensaje);
    void escucharYMirar();
public:
    Jugador(Pipe* chat, Pipe* canalOperaciones, Pipe* canalEntrada, Pipe* canalSalida, unsigned short identificador, int cantidadJugadores,
            MemoriaCompartida<int>* turno, MemoriaCompartida<int>* cantidadManos, Semaforo* tirarCarta, Semaforo* mirarCarta,
            Semaforo* manos, LockFile* lock,  MemoriaCompartida<int>* cantidadCartas);
    void juntar();
    void jugar();
    ~Jugador();
};


#endif //TP1_JUGADOR_H
