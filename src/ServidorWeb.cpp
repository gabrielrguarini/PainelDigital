#include "ServidorWeb.h"
#include <ArduinoJson.h>

ServidorWeb::ServidorWeb()
    : server(80), ws("/ws")
{
}

void ServidorWeb::iniciar(const char *ssid, const char *senha, GerenciadorDeVotacoes *ger)
{
    _gerenciador = ger;
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

    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            Serial.printf("Cliente WebSocket conectado\n");
            if (_gerenciador != nullptr) {
                String json = gerarJsonAtualizacao(*_gerenciador);
                client->text(json);
            }
        }
    });

    server.addHandler(&ws);

    server.begin();

    Serial.println("Servidor iniciado");
}

String ServidorWeb::gerarJsonAtualizacao(GerenciadorDeVotacoes &ger)
{
    // Aumentamos o tamanho de memória do Json para comportar votações longas
    DynamicJsonDocument doc(4096); 

    doc["atual"] = ger.obterQuantidadeVotacoes() - 1;

    JsonArray votacoes = doc.createNestedArray("votacoes");

    int total = ger.obterQuantidadeVotacoes();

    for (int i = 0; i < total; i++)
    {
        Votacao *v = ger.obterVotacao(i);

        JsonObject voto = votacoes.createNestedObject();
        voto["sim"] = v->obterTotalSim();
        voto["nao"] = v->obterTotalNao();

        JsonArray individuais = voto.createNestedArray("individuais");
        for (int j = 0; j < v->obterNumVotos(); j++) {
            VotoIndividual vi = v->obterVoto(j);
            JsonObject vp = individuais.createNestedObject();
            vp["id"] = vi.id; // Envia o ID numérico ao invés da string
            vp["voto"] = vi.voto; // 1 = SIM, 0 = NAO, -1 = NAO_VOTOU
        }
    }

    String json;
    serializeJson(doc, json);
    return json;
}

void ServidorWeb::enviarAtualizacao(GerenciadorDeVotacoes &ger)
{
    String json = gerarJsonAtualizacao(ger);
    ws.textAll(json);
}