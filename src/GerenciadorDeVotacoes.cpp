#include "GerenciadorDeVotacoes.h"

GerenciadorDeVotacoes::GerenciadorDeVotacoes()
{
    _indiceAtual = 0;
}

Votacao *GerenciadorDeVotacoes::obterVotacaoAtual()
{
    return &_historico[_indiceAtual];
}

void GerenciadorDeVotacoes::iniciarNovaVotacao()
{
    if (_indiceAtual < MAX_VOTACOES - 1)
    {
        _indiceAtual++;
    }
}

int GerenciadorDeVotacoes::obterQuantidadeVotacoes()
{
    return _indiceAtual + 1;
}

Votacao *GerenciadorDeVotacoes::obterVotacao(int indice)
{
    if (indice <= _indiceAtual)
    {
        return &_historico[indice];
    }

    return nullptr;
}