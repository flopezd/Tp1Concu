#ifndef TP1_ARBITRO_H
#define TP1_ARBITRO_H


#include "MemoriaCompartida.h"

class Arbitro {
private:
    MemoriaCompartida<int>* cantidadCartasPorJugador;
    int cantidadJugadores;
public:
    int preguntarCantidadCartas(int numeroJugardor);
    Arbitro(MemoriaCompartida<int>* cantidadCartasPorJugador, int cantidadJugadores);
    ~Arbitro();

};


#endif //TP1_ARBITRO_H
