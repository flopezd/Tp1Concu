#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string>

class Semaforo {

private:
    int id;
    int valorInicial;

    int inicializar () const;

public:
    Semaforo ( const std::string& nombre, const int valorInicial, const int cantidadSemaforos);
    ~Semaforo();

    int p (const unsigned short numeroSemaforo) const; // decrementa
    int v (const unsigned short numeroSemaforo) const; // incrementa
    int ultimoProceso () const; // pid del ultimo proceso que uso el sem
    void eliminar () const;
};

#endif /* SEMAFORO_H_ */