#ifndef TP1_PILON_H
#define TP1_PILON_H
#include <stack>

const int CARTAS_MAXIMAS = 48;

enum Palo {
    BASTO, ESPADA, COPA, ORO
};

struct Carta {
    int numero;
    Palo palo;
};

class Pilon {
private:
    std::stack<Carta> cartas;
public:
    Pilon(bool completo);
    Carta ultimaCarta();
    long cantidadCartas();
    void agregarCarta(Carta carta);
    Carta sacarCarta();
    ~Pilon();
};


#endif //TP1_PILON_H
