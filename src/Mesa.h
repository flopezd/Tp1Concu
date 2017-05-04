//
// Created by fran on 30/04/17.
//

#ifndef TP1_MESA_H
#define TP1_MESA_H

#include "Pipe.h"
#include "Pilon.h"
#include "Semaforo.h"
#include "MemoriaCompartida.h"

const Carta NO_CARTA = {0, BASTO};

class Mesa {
private:
    Pipe* canalOperaciones;
    Pipe* canalesEntrada;
    Pipe* canalesSalida;
    Pilon pilon = Pilon(true);
    int cantidadJugadores;
    Semaforo* manos;
    MemoriaCompartida<int>* cantidadManos;

    void repartirAUnJugador(int numeroDeJugador);
public:
    Mesa(Pipe *canalOperaciones, Pipe *canalesEntrada, Pipe *canalesSalida, int cantidadCanales, Semaforo* manos,
         MemoriaCompartida<int>* cantidadManos);
    void repartir();
    void jugar();
    ~Mesa();
};


#endif //TP1_MESA_H
