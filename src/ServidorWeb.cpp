#include "ServidorWeb.h"
#include <ArduinoJson.h>

ServidorWeb::ServidorWeb()
    : server(80), ws("/ws")
{
}

void ServidorWeb::iniciar(const char *ssid, const char *senha)
{
    WiFi.begin(ssid, senha);

    Serial.print("Conectando no WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.addHandler(&ws);

    server.begin();

    Serial.println("Servidor iniciado");
}

void ServidorWeb::enviarAtualizacao(GerenciadorDeVotacoes &ger)
{
    JsonDocument doc = StaticJsonDocument<512>();

    doc["atual"] = ger.obterQuantidadeVotacoes() - 1;

    JsonArray votacoes = doc["votacoes"].to<JsonArray>();

    int total = ger.obterQuantidadeVotacoes();

    for (int i = 0; i < total; i++)
    {
        Votacao *v = ger.obterVotacao(i);

        JsonObject voto = votacoes.createNestedObject();
        voto["sim"] = v->obterTotalSim();
        voto["nao"] = v->obterTotalNao();
    }

    String json;
    serializeJson(doc, json);

    ws.textAll(json);
}