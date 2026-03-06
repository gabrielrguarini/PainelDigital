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

    String gerarJsonAtualizacao(GerenciadorDeVotacoes &ger);
    void enviarAtualizacao(GerenciadorDeVotacoes &ger);
};

#endif