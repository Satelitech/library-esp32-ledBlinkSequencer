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
} blinkPattern;

typedef struct
{
    blinkPattern periodType;
    bool setPeriod;
    uint8_t phase;
} blinkContext;

#define STEP_INTERVAL 125

class sequencer
{

public:
    sequencer()
        : _phase(0),
          _oldMillis(0),
          _firstTrigger(false)
    {
    }
    void handler()
    {
        //* cuando recien lo seteo, reinicio y lo hago correr desde 0
        if (this->_firstTrigger)
        {
            this->_firstTrigger = false;
            this->_oldMillis = millis();
        }
        //* en ejecuciones siguientes, respeto el momento de la llamada del metodo
        if (millis() - this->_oldMillis > STEP_INTERVAL)
        {
            this->_oldMillis = millis();
            this->incrementCounter();
        }
    }

    void setPattern(blinkPattern _periodType, bool force = false)
    {
        if (!this->context.setPeriod || force)
        {
            this->context.periodType = _periodType;
            this->context.setPeriod = true;
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
        if (!this->context.setPeriod)
            return false;

        // uint8_t phase = this->_phase;
        switch (this->context.periodType)
        {
        case OFF:
            return false;
            break;

        case ON:
            return true;
            break;

        case BANG:
            if (this->_phase == 0)
                return true;
            return false;
            break;

        case DOUBLE_BANG:
            if (this->_phase == 0 || this->_phase == 4)
                return true;
            return false;
            break;

        case TRI_BANG:
            if (this->_phase == 0 || this->_phase == 3 || this->_phase == 6)
                return true;
            return false;
            break;
        case QUAD_BANG:
            if (this->_phase == 0 || this->_phase == 2 || this->_phase == 4 || this->_phase == 6)
                return true;
            return false;
            break;
        case BLINK:
            if (this->_phase == 0 || this->_phase == 1)
                return true;
            return false;
            break;
        case LONG_BLINK:
            if (this->_phase == 0 || this->_phase == 1 || this->_phase == 2 || this->_phase == 3)
                return true;
            return false;
            break;
        case DOUBLE_BLINK:
            if (this->_phase == 0 || this->_phase == 1 || this->_phase == 4 || this->_phase == 5)
                return true;
            return false;
            break;
        default:
            return false;
            break;
        }
        return false;
    }

private:
    uint8_t _phase;
    blinkContext context;
    unsigned long _oldMillis;
    bool _firstTrigger;

    void incrementCounter()
    {
        this->_phase++;
        if (this->_phase > 7)
        {
            this->context.setPeriod = false;
            this->_phase = 0;
        }
    }
};

#endif