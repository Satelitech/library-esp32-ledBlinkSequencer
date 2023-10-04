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

enum sequencerMode
{
    STEPS_8 = 0,
    STEPS_16 = 1
};

typedef struct
{
    blinkPattern periodType;
    bool setPeriod;
    uint8_t phase;
} blinkContext;

#define INTERVAL_8 125
#define INTERVAL_16 62.5

class sequencer
{

public:
    sequencer()
        : _phase(0),
          _oldMillis(0),
          _firstTrigger(false),
          _mode(sequencerMode::STEPS_8),
          _interval(_mode == sequencerMode::STEPS_8 ? INTERVAL_8 : INTERVAL_16)
    {
    }
    sequencer(sequencerMode mode)
        : _phase(0),
          _oldMillis(0),
          _firstTrigger(false),
          _mode(mode),
          _interval(_mode == sequencerMode::STEPS_8 ? INTERVAL_8 : INTERVAL_16)
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
        if (millis() - this->_oldMillis > _interval)
        {
            this->_oldMillis = millis();
            this->incrementCounter();
        }
    }

    void setPattern(blinkPattern _periodType, bool force = false)
    {
        if (!this->context.setPeriod || force)
        {
            if (_periodType != this->context.periodType)
            {
                this->_firstTrigger = true;
                this->_phase = 0;
            }
            this->context.periodType = _periodType;
            this->context.setPeriod = true;
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
            if (this->_phase == doubleIt(0) || this->_phase == doubleIt(4))
                return true;
            return false;
            break;

        case TRI_BANG:
            if (this->_phase == doubleIt(0) || this->_phase == doubleIt(3) || this->_phase == doubleIt(6))
                return true;
            return false;
            break;
        case QUAD_BANG:
            if (this->_phase == doubleIt(0) || this->_phase == doubleIt(2) || this->_phase == doubleIt(4) || this->_phase == doubleIt(6))
                return true;
            return false;
            break;

        case BLINK:
            // if (this->_phase ==  doubleIt(0) || this->_phase == doubleIt(1))
            if (this->_phase < firstQuarter())
                return true;
            return false;
            break;
        case LONG_BLINK:
            // if (this->_phase ==  doubleIt(0) || this->_phase == doubleIt(1) || this->_phase == doubleIt(2)  || this->_phase == doubleIt(3))
            if (this->_phase < firstHalf())
                return true;
            return false;
            break;
        case DOUBLE_BLINK:
            // if (this->_phase == doubleIt(0) || this->_phase == doubleIt(1) || this->_phase == doubleIt(4) || this->_phase == doubleIt(5))
            if (this->_phase < firstQuarter() || (this->_phase > firstHalf() && this->_phase < thirdQuarter()))
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
    sequencerMode _mode;
    long _interval;

    uint8_t firstQuarter()
    {
        return (this->_mode == sequencerMode::STEPS_8 ? 2 : 4);
    }
    uint8_t thirdQuarter()
    {
        return (this->_mode == sequencerMode::STEPS_8 ? 6 : 12);
    }
    uint8_t firstHalf()
    {
        return (this->_mode == sequencerMode::STEPS_8 ? 4 : 8);
    }

    void incrementCounter()
    {
        this->_phase++;
        if (this->_phase > (this->_mode == sequencerMode::STEPS_8 ? 7 : 15))
        {
            this->context.setPeriod = false;
            this->_phase = 0;
        }
    }
    uint8_t doubleIt(uint8_t value)
    {
        return (this->_mode == sequencerMode::STEPS_8 ? (value) : (value * 2));
    }
};

#endif