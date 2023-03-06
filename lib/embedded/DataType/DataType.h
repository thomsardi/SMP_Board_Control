#ifndef DATA_TYPE_H
#define DATA_TYPE_H

enum CommandType{
    RELAY = 1
};

struct LineData
{
    int voltage;
    int current;
    int power;
};

struct Command
{
    int commandType;
    int line;
    int value;
};

#endif