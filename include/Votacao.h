#ifndef VOTACAO_H
#define VOTACAO_H

#include <Arduino.h>

enum TipoVoto
{
    NAO_VOTOU = -1,
    NAO = 0,
    SIM = 1
};

class Votacao
{
private:
    int _totalSim;
    int _totalNao;

public:
    Votacao();
    void registrarVoto(TipoVoto votoAnterior, TipoVoto voto);
    void mostrarResultado();
    int obterSim();
    int obterNao();
    int obterTotalSim();
    int obterTotalNao();
};

#endif