#include "Vereador.h"

Vereador::Vereador(const char *nome)
{
    _nome = nome;
    _voto = NAO_VOTOU;
};

void Vereador::votar(TipoVoto voto)
{
    _voto = voto;
};

void Vereador::limparVoto()
{
    _voto = NAO_VOTOU;
};

TipoVoto Vereador::obterVoto()
{
    return _voto;
};

const char *Vereador::obterNome()
{
    return _nome;
};