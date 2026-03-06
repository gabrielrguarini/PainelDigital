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
    // Altere este nome caso você queira testar a mesa com outro!
    {Vereador("LAIR BOIADEIRO"), Botao(33), Botao(32)}
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

    // Cria a Votação Inicial #1 assim que o painel liga, para a tela não começar travada e sem dados
    gerenciador.iniciarNovaVotacao();

    // Inicializa todos os botões SIM e NÃO automaticamente e pré-popula a placa
    Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();

    for (int i = 0; i < NUM_VEREADORES; i++) {
        vereadores[i].botaoSim.begin();
        vereadores[i].botaoNao.begin();
        votacaoAtual->registrarVoto(vereadores[i].vereador.obterNome(), NAO_VOTOU);
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
            votacaoAtual->registrarVoto(vereadores[i].vereador.obterNome(), NAO_VOTOU);
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
            votacaoAtual->registrarVoto(vereadores[i].vereador.obterNome(), SIM);
            servidor.enviarAtualizacao(gerenciador);
            Serial.printf("%s votou SIM\n", vereadores[i].vereador.obterNome());
        }

        // Se o botão NAO deste vereador foi apertado...
        if (vereadores[i].botaoNao.atualizar())
        {
            vereadores[i].vereador.votar(NAO);
            votacaoAtual->registrarVoto(vereadores[i].vereador.obterNome(), NAO);
            servidor.enviarAtualizacao(gerenciador);
            Serial.printf("%s votou NÃO\n", vereadores[i].vereador.obterNome());
        }
    }
}