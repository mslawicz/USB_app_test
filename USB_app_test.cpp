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
    bool opened = nucleoYoke.openConnection(0x483, 0x5710, 3);
    if (opened)
    {
        std::cout << "connection to yoke is opened" << std::endl;
        nucleoYoke.closeConnection();
    }
    else
    {
        std::cout << "failed to open yoke USB connection" << std::endl;
    }
}