#ifndef BOTAO_H
#define BOTAO_H
    #include <Arduino.h>
    class Botao
    {
        private:
            int _pino;
            bool _estadoAnterior;
            unsigned long _ultimoTempoDebounce;
            const unsigned long _delayDebounce = 500;
    
        public:
            Botao(int pino);
            bool atualizar();
    };


#endif