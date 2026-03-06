#include "Vereador.h"

Vereador::Vereador(int id)
{
    _id = id;
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

int Vereador::obterId()
{
    return _id;
};