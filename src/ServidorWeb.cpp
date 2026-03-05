#include "ServidorWeb.h"

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

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", R"rawliteral(

<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Votação</title>
</head>
<body>

<h1>Resultado</h1>

<h2>SIM: <span id="sim">0</span></h2>
<h2>NÃO: <span id="nao">0</span></h2>

<script>

let ws = new WebSocket(`ws://${location.host}/ws`);

ws.onmessage = function(event)
{
    let dados = JSON.parse(event.data);

    document.getElementById("sim").innerText = dados.sim;
    document.getElementById("nao").innerText = dados.nao;
};

</script>

</body>
</html>

)rawliteral"); });

    server.addHandler(&ws);

    server.begin();

    Serial.println("Servidor iniciado");
}

void ServidorWeb::enviarAtualizacao(int sim, int nao)
{
    String json = "{";
    json += "\"sim\":";
    json += sim;
    json += ",";
    json += "\"nao\":";
    json += nao;
    json += "}";

    ws.textAll(json);
}