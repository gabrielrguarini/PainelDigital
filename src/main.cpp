#include <Arduino.h>
#include "Botao.h"
#include "Votacao.h"
#include "Vereador.h"
#include "ServidorWeb.h"
#include "GerenciadorDeVotacoes.h"

Botao botaoSim(33);
Botao botaoNao(32);
Botao botaoNovaVotacao(12);

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
    if (botaoNovaVotacao.atualizar())
    {
        gerenciador.iniciarNovaVotacao();

        servidor.enviarAtualizacao(gerenciador);

        Serial.println("Iniciada nova votação");
    }
    Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();
    if (botaoSim.atualizar())
    {
        TipoVoto votoAnterior = vereador1.obterVoto();

        vereador1.votar(SIM);

        votacaoAtual->registrarVoto(votoAnterior, SIM);

        servidor.enviarAtualizacao(gerenciador);

        Serial.printf("%s votou SIM\n", vereador1.obterNome());
    }

    if (botaoNao.atualizar())
    {
        TipoVoto votoAnterior = vereador1.obterVoto();

        vereador1.votar(NAO);

        votacaoAtual->registrarVoto(votoAnterior, NAO);

        servidor.enviarAtualizacao(gerenciador);

        Serial.printf("%s votou NÃO\n", vereador1.obterNome());
    }
}