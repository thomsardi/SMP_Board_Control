#ifndef DATA_TYPE_H
#define DATA_TYPE_H

enum CommandType{
    RELAY = 1,
};

struct LineData
{
    int voltage;
    int current;
    int power;
};

struct RelayData
{
    int number;
    int lineList[30];
    int valueList[30];
};

struct Command 
{
    int type;
    RelayData relayData;
};

#endif