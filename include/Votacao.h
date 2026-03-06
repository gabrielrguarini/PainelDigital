#ifndef VOTACAO_H
#define VOTACAO_H

#include <Arduino.h>

enum TipoVoto
{
    NAO_VOTOU = -1,
    NAO = 0,
    SIM = 1
};

#define MAX_VEREADORES_VOTACAO 20

struct VotoIndividual {
    int id;
    TipoVoto voto;
};

class Votacao
{
private:
    int _totalSim;
    int _totalNao;

    VotoIndividual _votos[MAX_VEREADORES_VOTACAO];
    int _numVotos;

public:
    Votacao();
    void limparVotos();
    void registrarVoto(int idVereador, TipoVoto voto);
    void mostrarResultado();
    int obterTotalSim();
    int obterTotalNao();
    int obterNumVotos();
    VotoIndividual obterVoto(int indice);
};

#endif