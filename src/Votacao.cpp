#include "Votacao.h"

Votacao::Votacao()
{
    _totalSim = 0;
    _totalNao = 0;
}
int Votacao::obterSim()
{
    return _totalSim;
}

int Votacao::obterNao()
{
    return _totalNao;
}
int Votacao::obterTotalSim()
{
    return _totalSim;
}

int Votacao::obterTotalNao()
{
    return _totalNao;
}

void Votacao::registrarVoto(TipoVoto votoAnterior, TipoVoto votoAtual)

{
    if (votoAnterior == votoAtual)
    {
        return;
    }

    if (votoAnterior == SIM)
    {
        _totalSim--;
    }
    else if (votoAnterior == NAO)
    {
        _totalNao--;
    }

    if (votoAtual == SIM)
    {
        _totalSim++;
    }
    else if (votoAtual == NAO)
    {
        _totalNao++;
    }

    mostrarResultado();
}

void Votacao::mostrarResultado()
{
    Serial.println("----- RESULTADO -----");

    Serial.print("SIM: ");
    Serial.println(_totalSim);

    Serial.print("NAO: ");
    Serial.println(_totalNao);

    Serial.println("---------------------");
}