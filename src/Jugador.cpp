#include <iostream>
#include <cstring>
#include "Jugador.h"
#include "Mensaje.h"

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
    this->logger = Logger::getInstance();
}

void Jugador::juntar() {
    Carta carta;
    int cantCartas;
    canalEntrada->leer(static_cast<void *>(&cantCartas), sizeof(int));

    std::string log = "Jugador ";
    log.append(std::to_string(identificador)).append(" junta ").append(std::to_string(cantCartas)).append(" cartas.");
    logger->loguear(log.c_str());
    for(int i=0; i < cantCartas; i++){
        canalEntrada->leer(static_cast<void *>(&carta), sizeof(Carta));
        pilon.agregarCarta(carta);
    }
}

void Jugador::ponerCarta(int nroTurno) {
    // Espera que todos hayan mirado la carta
    for (unsigned short i=0; i < cantidadJugadores; i++) {
        mirarCarta->p(i);
    }
    Mensaje mensaje = {PONER_CARTA, identificador};
    canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
    Carta carta = pilon.sacarCarta();
    canalSalida->escribir(static_cast<const void *>(&carta), sizeof(Carta));
    std::string log = "Jugador ";
    log.append(std::to_string(identificador)).append(" jugo ").append(std::to_string(carta.numero)).append(" de ").append(std::to_string(carta.palo));
    logger->loguear(log.c_str());
    if (pilon.cantidadCartas() == 0) {
        std::string log = "Jugador ";
        log.append(std::to_string(identificador)).append(" gano.");
        logger->loguear(log.c_str());
        // Avisa a los otros procesos que termino el juego
        turno->escribir(-1);
    } else {
        turno->escribir((nroTurno+1) % cantidadJugadores);
    }
    // Libera el semaforo de que jugo para que todos pueda ver la carta
    for (unsigned short i=0; i < cantidadJugadores; i++) {
        tirarCarta->v(i);
    }
}

void Jugador::comunicar(std::string mensaje) {
    for(int i = 0; i < cantidadJugadores; i++) {
        if (i != identificador) {
            chat[i].escribir(static_cast<const void *>(mensaje.c_str()), BUFFSIZE) ;
        }
    }
}

void Jugador::escucharYMirar() {
    char buffer[BUFFSIZE];
    for(int i = 0; i < cantidadJugadores-1; i++) {
        ssize_t bytesLeidos = chat[identificador].leer(static_cast<void *>(buffer), BUFFSIZE) ;
        std :: string mensaje = buffer;
        mensaje.resize(bytesLeidos);
        std::string log = "Jugador ";
        log.append(std::to_string(identificador)).append(": ").append(mensaje);
        logger->loguear(log.c_str());
    }
}

void Jugador::ponerLasManos() {
    std::string mensaje = "Escucha a jugador ";
    mensaje.append(std::to_string(identificador)).append(" decir: 'Atrevido'.");
    comunicar(mensaje);
    lock->tomarLock();
    manos->p(0);
    cantidadManos->escribir(cantidadManos->leer()+1);
    std::string log = "Jugador ";
    log.append(std::to_string(identificador)).append(" puso la mano.");
    logger->loguear(log.c_str());
    lock->liberarLock();
    while (cantidadManos->leer() < cantidadJugadores ) {
    }
    int lastPid = manos->ultimoProceso();
    if (lastPid == getpid()) {
        Mensaje mensaje = {JUNTAR_TODO, identificador};
        canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
        juntar();
    }
    ultimaCarta = NO_CARTA;
    escucharYMirar();
}

void Jugador::actuar() {
    Mensaje mensaje = {VER_ULTIMA_CARTA, identificador};
    canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
    Carta carta;
    canalEntrada->leer(static_cast<void *>(&carta), sizeof(Carta));
    switch (carta.numero) {
        case 10: {
            std::string mensaje = "Escucha a jugador ";
            mensaje.append(std::to_string(identificador));
            mensaje.append(" decir: 'Buenos dı́as señorita'.");
            comunicar(mensaje);
            escucharYMirar();
            break;
        }
        case 11: {
            std::string mensaje = "Escucha a jugador ";
            mensaje.append(std::to_string(identificador));
            mensaje.append(" decir: 'Buenos dı́as caballero'.");
            comunicar(mensaje);
            escucharYMirar();
            break;
        }
        case 12: {
            std::string mensaje = "Ve a jugador ";
            mensaje.append(std::to_string(identificador));
            mensaje.append(" hacer la venia.");
            comunicar(mensaje);
            escucharYMirar();
            break;
        }
        case 7:
            ponerLasManos();
            break;
        default:break;
    }
    if (ultimaCarta.numero == carta.numero && carta.numero != 7) {
        ponerLasManos();
    } else {
        ultimaCarta = carta;
    }
}

void Jugador::jugar() {
    // Libera el semaforo de que miro la carta
    mirarCarta->v(identificador);
    while(pilon.cantidadCartas() > 0 && turno->leer() > -1) {
        int nroTurno = turno->leer();
        if(nroTurno == identificador) {
            ponerCarta(nroTurno);
        }
        // Espera a que tiren la carta para mirar
        tirarCarta->p(identificador);
        if (turno->leer() > -1) {
            actuar();
            // Libera el semaforo de que miro la carta
            mirarCarta->v(identificador);
            cantidadCartas->escribir(pilon.cantidadCartas());
        }
    }
    std::string log = "Jugador ";
    log.append(std::to_string(identificador)).append(" le quedan ").append(std::to_string(pilon.cantidadCartas())).append(" cartas.");
    logger->loguear(log.c_str());
    if (pilon.cantidadCartas() == 0) {
        Mensaje mensaje = {TERMINAR_DE_JUGAR, identificador};
        canalOperaciones->escribir(static_cast<const void *>(&mensaje), sizeof(Mensaje));
    }
    log = "Termino jugador ";
    log.append(std::to_string(identificador));
    logger->loguear(log.c_str());
}

Jugador::~Jugador() {
    canalOperaciones->cerrar();
    canalEntrada->cerrar();
    canalSalida->cerrar();
    turno->liberar();
    cantidadManos->liberar();
    mirarCarta->eliminar();
    tirarCarta->eliminar();
    manos->eliminar();
    cantidadCartas->liberar();
    for(int i = 0; i < cantidadJugadores; i++) {
        chat[i].cerrar();
    }
}