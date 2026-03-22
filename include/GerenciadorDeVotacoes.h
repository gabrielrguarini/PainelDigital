#ifndef GERENCIADOR_DE_VOTACOES_H
#define GERENCIADOR_DE_VOTACOES_H

#include <Arduino.h>
#include "Votacao.h"

class GerenciadorDeVotacoes
{
private:
    Votacao _votacaoAtual;
    int _idVotacao; // Identificador sequencial da votação para referência

public:
    GerenciadorDeVotacoes();

    Votacao *obterVotacaoAtual();

    void iniciarNovaVotacao();

    int obterIdVotacaoAtual();
};

#endif