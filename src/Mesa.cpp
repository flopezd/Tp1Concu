#include <iostream>
#include "Mesa.h"
#include "Mensaje.h"


Mesa::Mesa(Pipe *canalOperaciones, Pipe *canalesEntrada, Pipe *canalesSalida, int cantidadCanales, Semaforo* manos,
           MemoriaCompartida<int>* cantidadManos) {
    this->canalOperaciones = canalOperaciones;
    this->canalesEntrada = canalesEntrada;
    this->canalesSalida = canalesSalida;
    this->canalOperaciones->setearModo(Pipe::LECTURA);
    for(int i = 0; i < cantidadCanales; i++) {
        this->canalesSalida[i].setearModo(Pipe::ESCRITURA);
        this->canalesEntrada[i].setearModo(Pipe::LECTURA);
    }
    this->cantidadJugadores = cantidadCanales;
    this->manos = manos;
    this->cantidadManos = cantidadManos;
}

void Mesa::repartir() {
    int cantidadDeCartas[cantidadJugadores] = {0};
    int numeroDeJugador = 0;
    for(int i = 0; i < CARTAS_MAXIMAS; i++) {
        cantidadDeCartas[numeroDeJugador]++;
        numeroDeJugador = (numeroDeJugador + 1) % cantidadJugadores;
    }
    for(int i = 0; i < cantidadJugadores; i++) {
        canalesSalida[i].escribir(static_cast<const void *>(&cantidadDeCartas[i]), sizeof(int));
    }
    numeroDeJugador = 0;
    while (pilon.cantidadCartas() > 0) {
        Carta carta = pilon.sacarCarta();
        canalesSalida[numeroDeJugador].escribir(static_cast<const void *>(&carta), sizeof(Carta));
        numeroDeJugador = (numeroDeJugador + 1) % cantidadJugadores;
    }
}

void Mesa::repartirAUnJugador(int numeroDeJugador) {
    int cantidadCartas = (int) pilon.cantidadCartas();
    canalesSalida[numeroDeJugador].escribir(static_cast<const void *>(&cantidadCartas), sizeof(int));
    while (pilon.cantidadCartas() > 0) {
        Carta carta = pilon.sacarCarta();
        canalesSalida[numeroDeJugador].escribir(static_cast<const void *>(&carta), sizeof(Carta));
    }
}

void Mesa::jugar() {
    Carta carta;
    Mensaje mensaje;
    bool terminar = false;
    int timeoutCounter = 0;
    while (!terminar && timeoutCounter < 10000) {
        if (canalOperaciones->leer(static_cast<void *>(&mensaje), sizeof(Mensaje)) > 0) {
            switch (mensaje.tipoOperacion) {
                case PONER_CARTA: {
                    canalesEntrada[mensaje.idCanal].leer(static_cast<void *>(&carta), sizeof(Carta));
                    Carta ultimaCarta = NO_CARTA;
                    if (pilon.cantidadCartas() > 0) {
                        ultimaCarta = pilon.ultimaCarta();
                    }
                    if (carta.numero == 7 || ultimaCarta.numero == carta.numero) {
                        cantidadManos->escribir(0);
                        for(int i = 0; i < cantidadJugadores; i++) {
                            manos->v(0);
                        }
                    }
                    pilon.agregarCarta(carta);
                    timeoutCounter = 0;
                    break;
                }
                case VER_ULTIMA_CARTA: {
                    Carta ultimaCarta = NO_CARTA;
                    if (pilon.cantidadCartas() > 0) {
                        ultimaCarta = pilon.ultimaCarta();
                    }
                    canalesSalida[mensaje.idCanal].escribir(static_cast<const void *>(&ultimaCarta), sizeof(Carta));
                    timeoutCounter = 0;
                    break;
                }
                case JUNTAR_TODO:
                    repartirAUnJugador(mensaje.idCanal);
                    break;
                case TERMINAR_DE_JUGAR:
                    terminar = true;
                    break;
                default:
                    // Code
                    break;
            }
        }
        timeoutCounter++;
    }
}

Mesa::~Mesa() {
    canalOperaciones->cerrar();
    for(int i = 0; i < cantidadJugadores; i++) {
        canalesSalida->cerrar();
        canalesEntrada->cerrar();
    }
    manos->eliminar();
    cantidadManos->liberar();
}