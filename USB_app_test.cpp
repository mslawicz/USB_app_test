// USB_app_test.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "YokeInterface.h"
#include <iostream>

//uint8_t readBuffer[256];
//uint8_t writeBuffer[64];
//OVERLAPPED readOverlappedData;
//OVERLAPPED writeOverlappedData;

int main()
{
    YokeInterface nucleoYoke;
    bool err = nucleoYoke.openConnection();
    if (err)
    {
        std::cout << "failed to open yoke USB connection" << std::endl;
    }
    else
    {
        std::cout << "connection to yoke is opened" << std::endl;
    }
}