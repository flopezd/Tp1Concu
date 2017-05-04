#include "Arbitro.h"

Arbitro::Arbitro(MemoriaCompartida<int>* cantidadCartasPorJugador, int cantidadJugadores) {
    this->cantidadJugadores = cantidadJugadores;
    this->cantidadCartasPorJugador = cantidadCartasPorJugador;
}

int Arbitro::preguntarCantidadCartas(int numeroJugardor) {
    return cantidadCartasPorJugador[numeroJugardor].leer();
}

Arbitro::~Arbitro() {
    for (int i = 0; i < cantidadJugadores; i++) {
        cantidadCartasPorJugador[i].liberar();
    }
}
