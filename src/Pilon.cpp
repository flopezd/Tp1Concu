//
// Created by fran on 29/03/17.
//

#include <cstdlib>
#include <time.h>
#include <random>
#include "Pilon.h"


int _random() {
    srand (std::random_device()());
    return rand();
}

Pilon::Pilon(bool completo) {
    if (completo) {
        Palo palos[] = {BASTO, ESPADA, ORO, COPA};
        Carta cartas[CARTAS_MAXIMAS];
        for(int i=0; i<4; i++) {
            for(int j=0; j<12; j++) {
                cartas[i*12+j].numero = j+1;
                cartas[i*12+j].palo = palos[i];
            }
        }

        int intercambios = _random() % 1000 + 1000;
        for(int j=0; j<intercambios; j++) {
            int nroCarta1 = _random() % CARTAS_MAXIMAS;
            int nroCarta2 = (_random() % CARTAS_MAXIMAS + nroCarta1) % CARTAS_MAXIMAS;
            Carta aux = cartas[nroCarta1];
            cartas[nroCarta1] = cartas[nroCarta2];
            cartas[nroCarta2] = aux;
        }

        for(int j=0; j<CARTAS_MAXIMAS; j++) {
            this->cartas.push(cartas[j]);
        }
    }
}

Carta Pilon::ultimaCarta() {
    return this->cartas.top();
}

long Pilon::cantidadCartas() {
    return this->cartas.size();
}

void Pilon::agregarCarta(Carta carta) {
    this->cartas.push(carta);
}

Carta Pilon::sacarCarta() {
    Carta carta = this->cartas.top();
    this->cartas.pop();
    return carta;
}

Pilon::~Pilon() {

}
