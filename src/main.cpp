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

// Crie a lista de vereadores aqui com os nomes e os respectivos pinos dos botões SIM e NÃO.
// CUIDADO: O ESP32 possui poucos pinos totalmente seguros para botões com INPUT_PULLUP.
// Para 11 vereadores (22 botões), você precisará de expansores (ex: 74HC165, PCF8574),
// ou utilizar pinos como 34, 35, 36 e 39 (que OBRIGATORIAMENTE exigem resistores de pull-up físicos externos de 10k).
ConfigVereador vereadores[] = {
    {Vereador("Gabriel"), Botao(33), Botao(32)}
    // Quando for utilizar os expansores I2C, você poderá adicionar os outros aqui novamente!
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

    // Inicializa todos os botões SIM e NÃO automaticamente
    for (int i = 0; i < NUM_VEREADORES; i++) {
        vereadores[i].botaoSim.begin();
        vereadores[i].botaoNao.begin();
    }

    servidor.iniciar("Camara Municipal", "legislativoEFcm2024");
}

void loop()
{
    // Verifica botão de nova votação
    if (botaoNovaVotacao.atualizar())
    {
        gerenciador.iniciarNovaVotacao();
        
        // Zera o voto de todos os vereadores para a nova rodada
        for (int i = 0; i < NUM_VEREADORES; i++) {
            vereadores[i].vereador.limparVoto();
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
            TipoVoto votoAnterior = vereadores[i].vereador.obterVoto();
            vereadores[i].vereador.votar(SIM);
            votacaoAtual->registrarVoto(votoAnterior, SIM);
            servidor.enviarAtualizacao(gerenciador);
            Serial.printf("%s votou SIM\n", vereadores[i].vereador.obterNome());
        }

        // Se o botão NAO deste vereador foi apertado...
        if (vereadores[i].botaoNao.atualizar())
        {
            TipoVoto votoAnterior = vereadores[i].vereador.obterVoto();
            vereadores[i].vereador.votar(NAO);
            votacaoAtual->registrarVoto(votoAnterior, NAO);
            servidor.enviarAtualizacao(gerenciador);
            Serial.printf("%s votou NÃO\n", vereadores[i].vereador.obterNome());
        }
    }
}