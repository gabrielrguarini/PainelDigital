#ifndef VEREADOR_H
#define VEREADOR_H

#include <Arduino.h>
#include "Votacao.h"

class Vereador
{
private:
    int _id;
    TipoVoto _voto;

public:
    Vereador(int id);
    void votar(TipoVoto voto);
    void limparVoto();
    TipoVoto obterVoto();
    int obterId();
};

#endif