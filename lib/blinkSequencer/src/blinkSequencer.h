#ifndef ledsequencer_h
#define ledsequencer_h
#include "Arduino.h"

typedef enum
{
    OFF,          //* --------
    BANG,         //* x-------
    DOUBLE_BANG,  //* x---x---
    TRI_BANG,     //* x--x--x-
    QUAD_BANG,    //* x-x-x-x-
    BLINK,        //* xx------
    LONG_BLINK,   //* xxxx----
    DOUBLE_BLINK, //* xx--xx--
    ON            //* xxxxxxxx
} ledBlinkPeriod;

typedef struct
{
    ledBlinkPeriod periodType;
    bool setPeriod;
    uint8_t phase;
} ledPeriod;

#define STEP_INTERVAL 125

class leds
{

public:
    leds()
        : _phase(0),
          oldMillis(0),
          _firstTrigger(false)
    {
    }
    void handler()
    {
        //* cuando recien lo seteo, reinicio y lo hago correr desde 0
        if (this->_firstTrigger)
        {
            this->_firstTrigger = false;
            this->oldMillis = millis();
        }
        //* en ejecuciones siguientes, respeto el momento de la llamada del metodo
        if (millis() - this->oldMillis > STEP_INTERVAL)
        {
            this->oldMillis = millis();
            this->incrementCounter();
        }
    }



    void setPattern(ledBlinkPeriod _periodType, bool force = false)
    {
        if (!this->_periodStruct.setPeriod || force)
        {
            this->_periodStruct.periodType = _periodType;
            this->_periodStruct.setPeriod = true;
            this->_firstTrigger = true;
            this->_phase = 0;
        }
    }

    void forceOn()
    {
        this->setPattern(ON, true);
    }

    void forceOff()
    {
        this->setPattern(OFF, true);
    }

    bool getState()
    {
        if (!this->_periodStruct.setPeriod)
            return false;

        uint8_t phase = this->_phase;
        switch (this->_periodStruct.periodType)
        {
        case OFF:
            return false;
            break;

        case ON:
            return true;
            break;

        case BANG:
            if (phase == 0)
                return true;
            return false;
            break;

        case DOUBLE_BANG:
            if (phase == 0 || phase == 4)
                return true;
            return false;
            break;

        case TRI_BANG:
            if (phase == 0 || phase == 3 || phase == 6)
                return true;
            return false;
            break;
        case QUAD_BANG:
            if (phase == 0 || phase == 2 || phase == 4 || phase == 6)
                return true;
            return false;
            break;
        case BLINK:
            if (phase == 0 || phase == 1)
                return true;
            return false;
            break;
        case LONG_BLINK:
            if (phase == 0 || phase == 1 || phase == 2 || phase == 3)
                return true;
            return false;
            break;
        case DOUBLE_BLINK:
            if (phase == 0 || phase == 1 || phase == 4 || phase == 5)
                return true;
            return false;
            break;
        }
    }

private:
    uint8_t _phase;
    ledPeriod _periodStruct;
    unsigned long oldMillis;
    bool _firstTrigger;

        void incrementCounter()
    {
        _phase++;
        if (_phase > 7)
        {
            this->_periodStruct.setPeriod = false;
            _phase = 0;
        }
    }
};

#endif