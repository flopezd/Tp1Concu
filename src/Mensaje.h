#ifndef TP1_MENSAJE_H
#define TP1_MENSAJE_H

enum TipoOperacion {
    PONER_CARTA, VER_ULTIMA_CARTA, TERMINAR_DE_JUGAR, JUNTAR_TODO
};

struct Mensaje {
    TipoOperacion tipoOperacion;
    int idCanal;
};

#endif //TP1_MENSAJE_H
