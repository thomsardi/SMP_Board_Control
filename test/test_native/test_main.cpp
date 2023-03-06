#include <unity.h>
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdint.h>

const int numberOfShiftRegister = 2;
const int numberOfLed = 6;

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void pinConvert(int line)
{
    int multiplier = (line - 1) / 3;
    int pin = 7 + 7*(multiplier) - ((line-1)*2 % 6);
    int ledPin = line - 1;
    printf("Pin Set: %i\n", pin);
    printf("Pin Reset: %i\n", pin-1);
}

void convertTest()
{
    for (size_t i = 1; i < 7; i++)
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
    RUN_TEST(relay_test_1);
    
    RUN_TEST(convertTest);
    
    UNITY_END();
    // cout << "test" << endl;
    return 0;
}