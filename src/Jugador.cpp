#include <iostream>
#include <cstring>
#include "Jugador.h"
#include "Mensaje.h"
#include "Semaforo.h"

const int BUFFSIZE = 100;

Jugador::Jugador(Pipe* chat, Pipe* canalOperaciones, Pipe* canalEntrada, Pipe* canalSalida, unsigned short identificador, int cantidadJugadores,
                 MemoriaCompartida<int>* turno, MemoriaCompartida<int>* cantidadManos, Semaforo* tirarCarta, Semaforo* mirarCarta,
                 Semaforo* manos, LockFile* lock, MemoriaCompartida<int>* cantidadCartas):identificador(identificador) {
    this->canalOperaciones = canalOperaciones;
    this->canalEntrada = canalEntrada;
    this->canalSalida = canalSalida;
    this->canalOperaciones->setearModo(Pipe::ESCRITURA);
    this->canalSalida->setearModo(Pipe::ESCRITURA);
    this->canalEntrada->setearModo(Pipe::LECTURA);
    this->cantidadJugadores = cantidadJugadores;
    this->turno = turno;
    this->tirarCarta = tirarCarta;
    this->mirarCarta = mirarCarta;
    this->cantidadManos = cantidadManos;
    this->manos = manos;
    this->ultimaCarta = NO_CARTA;
    this->lock = lock;
    this->chat = chat;
    for(int i = 0; i < cantidadJugadores; i++) {
        if (i == identificador) {
            this->chat[i].setearModo(Pipe::LECTURA);
        } else {
            this->chat[i].setearModo(Pipe::ESCRITURA);
        }
    }
    this->cantidadCartas = cantidadCartas;
    this->cantidadCartas->escribir(0);
}

void Jugador::juntar() {
    Carta carta;
    int cantCartas;
    this->canalEntrada->leer(static_cast<void *>(&cantCartas), sizeof(int));
    std::cout<< "Jugador " << this->identificador << " junta "<< cantCartas << " cartas." << std::endl;
    for(int i=0; i < cantCartas; i++){
        this->canalEntrada->leer(static_cast<void *>(&carta), sizeof(Carta));
        this->pilon.agregarCarta(carta);
    }
}

void Jugador::ponerCarta(int nroTurno) {
    // Espera que todos hayan mirado la carta
    for (unsigned short i=0; i < this->cantidadJugadores; i++) {
        this->mirarCarta->p(i);
    }
    Mensaje mensaje = {PONER_CARTA, this->identificador};
    canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
    Carta carta = pilon.sacarCarta();
    canalSalida->escribir(static_cast<const void *>(&carta), sizeof(Carta));
    std::cout<< "Jugador " << this->identificador << " jugo "<< carta.numero << " de "<< carta.palo << std::endl;
    if (pilon.cantidadCartas() == 0) {
        std::cout<< "Jugador " << this->identificador << " gano." << std::endl;
        // Avisa a los otros procesos que termino el juego
        turno->escribir(-1);
    } else {
        turno->escribir((nroTurno+1) % this->cantidadJugadores);
    }
    // Libera el semaforo de que jugo para que todos pueda ver la carta
    for (unsigned short i=0; i < this->cantidadJugadores; i++) {
        this->tirarCarta->v(i);
    }
}

void Jugador::comunicar(std::string mensaje) {
    for(int i = 0; i < cantidadJugadores; i++) {
        if (i != identificador) {
            this->chat[i].escribir(static_cast<const void *>(mensaje.c_str()), BUFFSIZE) ;
        }
    }
}

void Jugador::escucharYMirar() {
    char buffer[BUFFSIZE];
    for(int i = 0; i < cantidadJugadores-1; i++) {
        ssize_t bytesLeidos = this->chat[this->identificador].leer(static_cast<void *>(buffer), BUFFSIZE) ;
        std :: string mensaje = buffer;
        mensaje.resize(bytesLeidos);
//        std::cout<<"Jugador "<< this->identificador << ": " << mensaje << std::endl;
    }
}

void Jugador::ponerLasManos() {
    std::string mensaje = "Escucha a jugador ";
    mensaje.append(std::to_string(this->identificador));
    mensaje.append(" decir: 'Atrevido'.");
    this->comunicar(mensaje);
    this->lock->tomarLock();
    this->manos->p(0);
    this->cantidadManos->escribir(this->cantidadManos->leer()+1);
    std::cout<< "Jugador " << this->identificador << " puso la mano." << std::endl;
    this->lock->liberarLock();
    while (this->cantidadManos->leer() < this->cantidadJugadores ) {
    }
    int lastPid = this->manos->ultimoProceso();
    if (lastPid == getpid()) {
        Mensaje mensaje = {JUNTAR_TODO, this->identificador};
        this->canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
        this->juntar();
    }
    this->ultimaCarta = NO_CARTA;
    this->escucharYMirar();
}

void Jugador::actuar() {
    Mensaje mensaje = {VER_ULTIMA_CARTA, this->identificador};
    this->canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
    Carta carta;
    this->canalEntrada->leer(static_cast<void *>(&carta), sizeof(Carta));
    switch (carta.numero) {
        case 10: {
            std::string mensaje = "Escucha a jugador ";
            mensaje.append(std::to_string(this->identificador));
            mensaje.append(" decir: 'Buenos dı́as señorita'.");
            this->comunicar(mensaje);
            this->escucharYMirar();
            break;
        }
        case 11: {
            std::string mensaje = "Escucha a jugador ";
            mensaje.append(std::to_string(this->identificador));
            mensaje.append(" decir: 'Buenos dı́as caballero'.");
            this->comunicar(mensaje);
            this->escucharYMirar();
            break;
        }
        case 12: {
            std::string mensaje = "Ve a jugador ";
            mensaje.append(std::to_string(this->identificador));
            mensaje.append(" hacer la venia.");
            this->comunicar(mensaje);
            this->escucharYMirar();
            break;
        }
        case 7:
            ponerLasManos();
            break;
        default:break;
    }
    if (this->ultimaCarta.numero == carta.numero && carta.numero != 7) {
        ponerLasManos();
    } else {
        this->ultimaCarta = carta;
    }
}

void Jugador::jugar() {
    // Libera el semaforo de que miro la carta
    this->mirarCarta->v(this->identificador);
    while(pilon.cantidadCartas() > 0 && turno->leer() > -1) {
        int nroTurno = turno->leer();
        if(nroTurno == this->identificador) {
            this->ponerCarta(nroTurno);
        }
        // Espera a que tiren la carta para mirar
        this->tirarCarta->p(this->identificador);
        if (turno->leer() > -1) {
            this->actuar();
            // Libera el semaforo de que miro la carta
            this->mirarCarta->v(this->identificador);
            this->cantidadCartas->escribir(this->pilon.cantidadCartas());
        }
    }
    std::cout<< "Jugador " << this->identificador << " le quedan "<< pilon.cantidadCartas() << " cartas." << std::endl;
    if (pilon.cantidadCartas() == 0) {
        Mensaje mensaje = {TERMINAR_DE_JUGAR, this->identificador};
        canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
    }
}

Jugador::~Jugador() {
    this->canalOperaciones->cerrar();
    this->canalEntrada->cerrar();
    this->canalSalida->cerrar();
    this->turno->liberar();
    this->cantidadManos->liberar();
    this->mirarCarta->eliminar();
    this->tirarCarta->eliminar();
    this->manos->eliminar();
    this->cantidadCartas->liberar();
    for(int i = 0; i < cantidadJugadores; i++) {
        this->chat[i].cerrar();
    }
}