#include "RelayControl.h"

RelayControl::RelayControl()
{

}

int RelayControl::write(int line, bool state)
{
    if (state > 0)
    {
        return relaySet(line);
    }
    return relayReset(line);
}

int RelayControl::relaySet(int line)
{
    int multiplier = (line - 1) / 3;
    int pin = 7 + 7*(multiplier) - ((line-1)*2 % 6);
    return pin;
}

int RelayControl::relayReset(int line)
{
    int multiplier = (line - 1) / 3;
    int pin = 7 + 7*(multiplier) - ((line-1)*2 % 6);
    return pin - 1;
}

RelayControl::~RelayControl()
{

}