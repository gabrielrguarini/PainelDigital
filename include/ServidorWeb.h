#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "GerenciadorDeVotacoes.h"

class ServidorWeb
{
private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    GerenciadorDeVotacoes *_gerenciador;

public:
    ServidorWeb();

    void iniciar(const char *ssid, const char *senha, GerenciadorDeVotacoes *ger);

    // Envia delta: apenas o voto individual que mudou
    void enviarVoto(int idVereador, int voto);

    // Envia evento de nova votação
    void enviarNovaVotacao();

    // Envia estado completo APENAS da votação atual para um cliente novo
    void enviarEstadoCompleto(AsyncWebSocketClient *client);

    // Heartbeat periódico ping para monitorar conexão e evitar queda
    void enviarPing();

    // Limpeza periódica de clientes WebSocket desconectados
    void limpezaClientes();
};

#endif