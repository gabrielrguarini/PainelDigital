#include <Arduino.h>
#include "Botao.h"
#include "Votacao.h"
#include "Vereador.h"
#include "ServidorWeb.h"
#include "GerenciadorDeVotacoes.h"

struct ConfigVereador {
    Vereador vereador;
    Botao botaoSim;
    Botao botaoNao;
};

// Crie a lista de pinos de botões dos vereadores correspondente as posições na lista do index.html.
// Vereador 0, Vereador 1, Vereador 2... até onde precisar!
ConfigVereador vereadores[] = {
    // Altere este nome caso você queira testar a mesa com outro!
    {Vereador(0), Botao(33), Botao(32)}
    // Exemplo para futuras portas: {Vereador(1), Botao(34), Botao(35)}, {Vereador(2), ...
};

// O compilador calcula automaticamente a quantidade de vereadores baseada no array acima
const int NUM_VEREADORES = sizeof(vereadores) / sizeof(ConfigVereador);

Botao botaoNovaVotacao(25);

GerenciadorDeVotacoes gerenciador;
ServidorWeb servidor;

void setup()
{
    Serial.begin(115200);

    botaoNovaVotacao.begin();

    // Inicializa todos os botões SIM e NÃO automaticamente e pré-popula a placa
    Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();

    for (int i = 0; i < NUM_VEREADORES; i++) {
        vereadores[i].botaoSim.begin();
        vereadores[i].botaoNao.begin();
        votacaoAtual->registrarVoto(vereadores[i].vereador.obterId(), NAO_VOTOU);
    }

    // O último parâmetro é a referência do gerenciador para permitir enviar os votos a clientes que conectaram tarde!
    servidor.iniciar("Camara Municipal", "legislativoEFcm2024", &gerenciador);
}

void loop()
{
    // Verifica botão de nova votação
    if (botaoNovaVotacao.atualizar())
    {
        gerenciador.iniciarNovaVotacao();
        Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();
        
        // Zera o voto de todos os vereadores para a nova rodada
        for (int i = 0; i < NUM_VEREADORES; i++) {
            vereadores[i].vereador.limparVoto();
            votacaoAtual->registrarVoto(vereadores[i].vereador.obterId(), NAO_VOTOU);
        }

        servidor.enviarAtualizacao(gerenciador);
        Serial.println("Iniciada nova votação");
    }

    Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();

    // Verifica apertos dos botões de cada um dos vereadores iterando um por um
    for (int i = 0; i < NUM_VEREADORES; i++) {
        
        // Se o botão SIM deste vereador foi apertado...
        if (vereadores[i].botaoSim.atualizar())
        {
            vereadores[i].vereador.votar(SIM);
            votacaoAtual->registrarVoto(vereadores[i].vereador.obterId(), SIM);
            servidor.enviarAtualizacao(gerenciador);
            Serial.printf("Vereador %d votou SIM\n", vereadores[i].vereador.obterId());
        }

        // Se o botão NAO deste vereador foi apertado...
        if (vereadores[i].botaoNao.atualizar())
        {
            vereadores[i].vereador.votar(NAO);
            votacaoAtual->registrarVoto(vereadores[i].vereador.obterId(), NAO);
            servidor.enviarAtualizacao(gerenciador);
            Serial.printf("Vereador %d votou NÃO\n", vereadores[i].vereador.obterId());
        }
    }
}