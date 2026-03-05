#ifndef GERENCIADOR_DE_VOTACOES_H
#define GERENCIADOR_DE_VOTACOES_H

#include <Arduino.h>
#include "Votacao.h"

#define MAX_VOTACOES 20

class GerenciadorDeVotacoes
{
private:
    Votacao _historico[MAX_VOTACOES];
    int _indiceAtual;

public:
    GerenciadorDeVotacoes();

    Votacao *obterVotacaoAtual();

    void iniciarNovaVotacao();

    int obterQuantidadeVotacoes();

    Votacao *obterVotacao(int indice);
};

#endif