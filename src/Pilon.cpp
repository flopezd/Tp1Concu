#include <cstdlib>
#include <random>
#include "Pilon.h"


int _random() {
    srand (std::random_device()());
    return rand();
}

Pilon::Pilon(bool completo) {
    if (completo) {
        Palo palos[] = {BASTO, ESPADA, ORO, COPA};
        Carta cartasAMezclar[CARTAS_MAXIMAS];
        for(int i=0; i<4; i++) {
            for(int j=0; j<12; j++) {
                cartasAMezclar[i*12+j].numero = j+1;
                cartasAMezclar[i*12+j].palo = palos[i];
            }
        }

        int intercambios = _random() % 1000 + 1000;
        for(int j=0; j<intercambios; j++) {
            int nroCarta1 = _random() % CARTAS_MAXIMAS;
            int nroCarta2 = (_random() % CARTAS_MAXIMAS + nroCarta1) % CARTAS_MAXIMAS;
            Carta aux = cartasAMezclar[nroCarta1];
            cartasAMezclar[nroCarta1] = cartasAMezclar[nroCarta2];
            cartasAMezclar[nroCarta2] = aux;
        }

        for(int j=0; j<CARTAS_MAXIMAS; j++) {
            this->cartas.push(cartasAMezclar[j]);
        }
    }
}

Carta Pilon::ultimaCarta() {
    return cartas.top();
}

long Pilon::cantidadCartas() {
    return cartas.size();
}

void Pilon::agregarCarta(Carta carta) {
    cartas.push(carta);
}

Carta Pilon::sacarCarta() {
    Carta carta = cartas.top();
    cartas.pop();
    return carta;
}

Pilon::~Pilon() {

}
