#include <Arduino.h>
#include "Botao.h"
#include "Votacao.h"
#include "Vereador.h"
#include "ServidorWeb.h"
#include "GerenciadorDeVotacoes.h"

// ============================================================
// TIPOS DE EVENTO PARA COMUNICAÇÃO ENTRE CORES
// ============================================================
enum TipoEvento { EVT_VOTO, EVT_NOVA_VOTACAO };

struct Evento {
    TipoEvento tipo;
    int idVereador;
    TipoVoto voto;
};

// ============================================================
// CONFIGURAÇÃO DOS VEREADORES E BOTÕES
// ============================================================
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

// Fila FreeRTOS para comunicação thread-safe entre Core 1 (botões) e Core 0 (WiFi)
QueueHandle_t filaEventos;

// ============================================================
// TASK DO CORE 0: WiFi, WebSocket, processamento de eventos
// ============================================================
void tarefaWiFi(void *parametro)
{
    // A chamada com strings hardcoded foi substituída para suportar seu provedor (ajuste aqui depois)
    servidor.iniciar("Guarini", "1759001996", &gerenciador);

    unsigned long ultimaLimpeza = 0;
    const unsigned long INTERVALO_LIMPEZA = 5000; // 5 segundos
    
    unsigned long ultimoPing = 0;
    const unsigned long INTERVALO_PING = 1000; // 1 segundo ping heartbeat para os clientes conectados

    Evento evento;

    for (;;) // Loop infinito da task do core 0
    {
        // Processa todos os eventos pendentes na fila (não-bloqueante), passando pelo timer timeout de 0 tick
        while (xQueueReceive(filaEventos, &evento, 0) == pdTRUE)
        {
            if (evento.tipo == EVT_NOVA_VOTACAO)
            {
                gerenciador.iniciarNovaVotacao();
                Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();

                // Zera o voto de todos os vereadores para a nova rodada
                for (int i = 0; i < NUM_VEREADORES; i++) {
                    votacaoAtual->registrarVoto(vereadores[i].vereador.obterId(), NAO_VOTOU);
                }

                servidor.enviarNovaVotacao(); // Manda DELTA (N) - sem array string e sem lag
                Serial.println("Iniciada nova votação");
            }
            else if (evento.tipo == EVT_VOTO)
            {
                Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();
                votacaoAtual->registrarVoto(evento.idVereador, evento.voto);
                servidor.enviarVoto(evento.idVereador, evento.voto); // Manda o DELTA (V) instantaneo
                Serial.printf("Vereador %d votou %s\n", evento.idVereador, evento.voto == SIM ? "SIM" : "NAO");
            }
        }

        unsigned long agora = millis();

        // 1 Segundos Heartbeat de ping
        if(agora - ultimoPing >= INTERVALO_PING) {
            servidor.enviarPing();
            ultimoPing = agora;
        }

        // Limpeza periódica de clientes WebSocket desconectados a cada 5 segundos
        if (agora - ultimaLimpeza >= INTERVALO_LIMPEZA)
        {
            servidor.limpezaClientes();
            ultimaLimpeza = agora;
        }

        // Yield para não monopolizar o core. Obrigatorio no freeRtos para nao resetar (Watchdog)
        vTaskDelay(1);
    }
}

// ============================================================
// SETUP (roda no Core 1 default do arduino)
// ============================================================
void setup()
{
    Serial.begin(115200);

    // Cria a fila de eventos com capacidade para 20 eventos
    filaEventos = xQueueCreate(20, sizeof(Evento));

    botaoNovaVotacao.begin();

    // Inicializa todos os botões SIM e NÃO e pré-popula a votação
    Votacao *votacaoAtual = gerenciador.obterVotacaoAtual();

    for (int i = 0; i < NUM_VEREADORES; i++) {
        vereadores[i].botaoSim.begin();
        vereadores[i].botaoNao.begin();
        votacaoAtual->registrarVoto(vereadores[i].vereador.obterId(), NAO_VOTOU);
    }

    // Cria a task do WiFi/WebSocket pinned ao Core 0
    // Stack de 8192 bytes, prioridade 1 (Ideal para a API lib ESPAsync do ws)
    xTaskCreatePinnedToCore(
        tarefaWiFi,     // Função da task
        "WiFiTask",     // Nome para debug
        8192,           // Stack size em bytes
        NULL,           // Parâmetro (não usado)
        1,              // Prioridade
        NULL,           // Handle (não precisamos)
        0               // Roda obrigatoriamente neste Core 0 (WiFi)
    );

    Serial.println("Setup completo - Core 1: Botões, Core 0: WiFi");
}

// ============================================================
// LOOP (roda no Core 1): Apenas leitura contínua e super-rápida de botões de hw
// ============================================================
void loop()
{
    Evento evento;

    // Verifica botão de nova votação
    if (botaoNovaVotacao.atualizar())
    {
        evento.tipo = EVT_NOVA_VOTACAO;
        evento.idVereador = -1;
        evento.voto = NAO_VOTOU;
        xQueueSend(filaEventos, &evento, 0);

        // Limpa estado local dos vereadores no hardware para poderem clicar dinovu
        for (int i = 0; i < NUM_VEREADORES; i++) {
            vereadores[i].vereador.limparVoto();
        }
    }

    // Verifica botões de cada vereador
    for (int i = 0; i < NUM_VEREADORES; i++) {

        // Botão SIM
        if (vereadores[i].botaoSim.atualizar())
        {
            vereadores[i].vereador.votar(SIM);
            evento.tipo = EVT_VOTO;
            evento.idVereador = vereadores[i].vereador.obterId();
            evento.voto = SIM;
            xQueueSend(filaEventos, &evento, 0);
        }

        // Botão NÃO
        if (vereadores[i].botaoNao.atualizar())
        {
            vereadores[i].vereador.votar(NAO);
            evento.tipo = EVT_VOTO;
            evento.idVereador = vereadores[i].vereador.obterId();
            evento.voto = NAO;
            xQueueSend(filaEventos, &evento, 0);
        }
    }
}