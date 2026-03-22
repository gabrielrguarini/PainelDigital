#include "GerenciadorDeVotacoes.h"

GerenciadorDeVotacoes::GerenciadorDeVotacoes()
{
    _idVotacao = 1;
}

Votacao *GerenciadorDeVotacoes::obterVotacaoAtual()
{
    return &_votacaoAtual;
}

void GerenciadorDeVotacoes::iniciarNovaVotacao()
{
    // Limpa a instância existente em vez de criar uma nova num array
    _votacaoAtual.limparVotos();
    _idVotacao++;
}

int GerenciadorDeVotacoes::obterIdVotacaoAtual()
{
    return _idVotacao;
}