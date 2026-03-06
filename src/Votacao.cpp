#include "Votacao.h"
#include <string.h>

Votacao::Votacao()
{
    limparVotos();
}

void Votacao::limparVotos()
{
    _totalSim = 0;
    _totalNao = 0;
    _numVotos = 0;
    for (int i = 0; i < MAX_VEREADORES_VOTACAO; i++) {
        _votos[i].voto = NAO_VOTOU;
        _votos[i].id = -1;
    }
}

int Votacao::obterTotalSim()
{
    return _totalSim;
}

int Votacao::obterTotalNao()
{
    return _totalNao;
}

int Votacao::obterNumVotos()
{
    return _numVotos;
}

VotoIndividual Votacao::obterVoto(int indice)
{
    return _votos[indice];
}

void Votacao::registrarVoto(int idVereador, TipoVoto votoAtual)
{
    // Procura o vereador na lista de votos já existentes
    for (int i = 0; i < _numVotos; i++)
    {
        if (_votos[i].id == idVereador)
        {
            TipoVoto votoAnterior = _votos[i].voto;
            if (votoAnterior == votoAtual) {
                return; // Não alterou
            }

            // Remove o voto antigo das parciais
            if (votoAnterior == SIM) _totalSim--;
            else if (votoAnterior == NAO) _totalNao--;

            // Adiciona o voto novo
            _votos[i].voto = votoAtual;
            if (votoAtual == SIM) _totalSim++;
            else if (votoAtual == NAO) _totalNao++;
            
            mostrarResultado();
            return;
        }
    }

    // Se o vereador ainda não estava na lista e há espaço
    if (_numVotos < MAX_VEREADORES_VOTACAO)
    {
        _votos[_numVotos].id = idVereador;
        _votos[_numVotos].voto = votoAtual;
        _numVotos++;

        if (votoAtual == SIM) _totalSim++;
        else if (votoAtual == NAO) _totalNao++;
        
        mostrarResultado();
    }
}

void Votacao::mostrarResultado()
{
    Serial.println("----- PARCIAL -----");
    Serial.print("SIM: ");  Serial.println(_totalSim);
    Serial.print("NAO: "); Serial.println(_totalNao);
    Serial.println("-------------------");
}