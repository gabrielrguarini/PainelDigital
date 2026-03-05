#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

class ServidorWeb
{
private:
    AsyncWebServer server;
    AsyncWebSocket ws;

public:
    ServidorWeb();

    void iniciar(const char *ssid, const char *senha);

    void enviarAtualizacao(int sim, int nao);
};

#endif