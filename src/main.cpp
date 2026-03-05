#include <Arduino.h>
#include "Botao.h"
#include "Votacao.h"
#include "Vereador.h"
#include "ServidorWeb.h"
#include "GerenciadorDeVotacoes.h"

Botao botaoSim(33);
Botao botaoNao(32);
Botao botaoNovaVotacao(25);

Vereador vereador1("Gabriel");

GerenciadorDeVotacoes gerenciador;

Votacao votacaoAtual;

ServidorWeb servidor;

void setup()
{
    Serial.begin(115200);

    servidor.iniciar("Camara Municipal", "legislativoEFcm2024");
}

void loop()
{
    if (botaoSim.atualizar())
    {
        TipoVoto votoAnterior = vereador1.obterVoto();

        vereador1.votar(SIM);

        votacaoAtual.registrarVoto(votoAnterior, SIM);

        servidor.enviarAtualizacao(
            votacaoAtual.obterTotalSim(),
            votacaoAtual.obterTotalNao());

        Serial.printf("%s votou SIM\n", vereador1.obterNome());
    }

    if (botaoNao.atualizar())
    {
        TipoVoto votoAnterior = vereador1.obterVoto();

        vereador1.votar(NAO);

        votacaoAtual.registrarVoto(votoAnterior, NAO);

        servidor.enviarAtualizacao(
            votacaoAtual.obterTotalSim(),
            votacaoAtual.obterTotalNao());

        Serial.printf("%s votou NÃO\n", vereador1.obterNome());
    }
}