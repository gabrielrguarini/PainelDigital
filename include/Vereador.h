#ifndef VEREADOR_H
#define VEREADOR_H

#include <Arduino.h>
#include "Votacao.h"

class Vereador
{
private:
    const char *_nome;
    TipoVoto _voto;

public:
    Vereador(const char *_nome);
    void votar(TipoVoto voto);
    TipoVoto obterVoto();
    const char *obterNome();
};

#endif