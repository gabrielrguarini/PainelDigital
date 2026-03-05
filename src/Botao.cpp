#include "Botao.h"

Botao::Botao(int pino){
    _pino = pino;
    _estadoAnterior = HIGH;
    _ultimoTempoDebounce = 0;
    pinMode(_pino, INPUT_PULLUP);
}

bool Botao::atualizar()
{
    bool leitura = digitalRead(_pino);

    if ((millis() - _ultimoTempoDebounce) < _delayDebounce)
    {
        return false;
    }

    if (_estadoAnterior == HIGH && leitura == LOW)
    {
        _ultimoTempoDebounce = millis();
        _estadoAnterior = leitura;
        return true;
    }

    _estadoAnterior = leitura;
    return false;
}