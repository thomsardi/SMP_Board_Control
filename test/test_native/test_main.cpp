#include <unity.h>
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdint.h>

const int numberOfShiftRegister = 4;
const int numberOfLed = 6;

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void parser(std::string input, char delimiter, std::string value[], size_t arrSize)
{
    int index = 0;
    int lastIndex = 0;
    std::string stringVal = input;
    std::string temp = "";
    int i = 0;
    bool isContinue = true;
    while(isContinue)
    {
        printf("parsing\n");
        lastIndex = stringVal.find(delimiter, index);
        printf("Found!\n");
        printf("Start index : %i\n", index);
        printf("Last index : %i\n", lastIndex);
        printf("%s\n", stringVal.substr(index, lastIndex - index).c_str());
        i++;
        if (lastIndex <= 0)
        {
            isContinue = false;
        }
        index = lastIndex+1;
    }
}

void parserTest()
{
    std::string value[12];
    int arrSize = 12;
    parser("1,23444,3123,41", ',', value, arrSize);
}

void pinConvert(int line)
{
    int lineToPin = line - 1;
    int multiplier = lineToPin / 3;
    int totalPin = (8 + 8*(multiplier)) - 1;
    int substract = (lineToPin - (multiplier*3)) * 2;
    int pin = totalPin - substract;
    int ledPin = line - 1;
    printf("Pin Set: %i\n", pin);
    printf("Pin Reset: %i\n", pin-1);
}

void convertTest()
{
    for (size_t i = 1; i <= 12; i++)
    {
        pinConvert(i);
    }
    
}

void relay_test_1()
{
    for (int j = 0; j < numberOfShiftRegister; j++)
    {
        for (int i = 7; i > 1; i--)
        {
            int pin = i + (j*7);
            printf("Pin : %i", pin);
            std::cout << std::endl;
            int ledString = (3 - (i/2)) + 3*j;
            printf("Led String %i ", ledString);
            if (i%2)
            {
                printf("set to Green\n");
            }
            else
            {
                printf("set to Black\n");
            }
        }
    }
}

void relay_test_2()
{
    for (size_t i = 0; i < numberOfLed; i++)
    {
        printf("Set led number %i to", i);
        if (i%2)
        {
            
        }
        else
        {

        }
    }
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    // RUN_TEST(relay_test_1);
    
    // RUN_TEST(convertTest);
    RUN_TEST(parserTest);
    
    UNITY_END();
    // cout << "test" << endl;
    return 0;
}