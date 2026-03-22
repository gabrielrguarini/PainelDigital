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
            Serial.printf("Cliente WebSocket conectado (id: %u)\n", client->id());
            enviarEstadoCompleto(client);
        }
    });

    server.addHandler(&ws);

    server.begin();

    Serial.println("Servidor iniciado");
}

// Envia apenas o delta do voto: {"t":"v","id":X,"voto":Y} (aprox 30 bytes)
void ServidorWeb::enviarVoto(int idVereador, int voto)
{
    char buffer[64];
    StaticJsonDocument<64> doc;
    doc["t"] = "v";
    doc["id"] = idVereador;
    doc["voto"] = voto;

    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    ws.textAll(buffer, len);
}

// Envia evento de nova votação: {"t":"n","id":X} (aprox 20 bytes)
void ServidorWeb::enviarNovaVotacao()
{
    char buffer[32];
    StaticJsonDocument<32> doc;
    doc["t"] = "n";
    doc["id"] = _gerenciador->obterIdVotacaoAtual(); // Envia ID para frontend saber separar

    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    ws.textAll(buffer, len);
}

// Envia estado completo APENAS da votação atual para constuir os valores dos cards de quem conectar atrasado
void ServidorWeb::enviarEstadoCompleto(AsyncWebSocketClient *client)
{
    if (_gerenciador == nullptr || client == nullptr) return;

    // Buffer menor já que tem apenas 1 votação!
    StaticJsonDocument<512> doc;
    doc["t"] = "e";
    doc["idVotacao"] = _gerenciador->obterIdVotacaoAtual();

    Votacao *v = _gerenciador->obterVotacaoAtual();
    doc["sim"] = v->obterTotalSim();
    doc["nao"] = v->obterTotalNao();

    JsonArray individuais = doc.createNestedArray("individuais");
    for (int j = 0; j < v->obterNumVotos(); j++) {
        VotoIndividual vi = v->obterVoto(j);
        JsonObject vp = individuais.createNestedObject();
        vp["id"] = vi.id;
        vp["voto"] = vi.voto;
    }

    // Não usa 'String' e alocação dinâmica. Escreve o JSON direto na memoria da stack no char array.
    char buffer[768];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    client->text(buffer, len);
}

// Ping para usar de heartbeat e previnir as quedas de tcp/wifi
void ServidorWeb::enviarPing()
{
    char buffer[16];
    StaticJsonDocument<16> doc;
    doc["t"] = "p"; // p = ping

    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    ws.textAll(buffer, len);
}

// Limpeza periódica de clientes desconectados do socket
void ServidorWeb::limpezaClientes()
{
    ws.cleanupClients();
}