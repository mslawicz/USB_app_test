// USB_app_test.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "YokeInterface.h"
#include <iostream>
#include <conio.h>

int main()
{
    YokeInterface nucleoYoke;
    bool opened = nucleoYoke.openConnection(0x483, 0x5710, 3);
    if (opened)
    {
        std::cout << "connection to yoke is opened" << std::endl;
        nucleoYoke.receptionEnable();
        while (1)
        {
            if (nucleoYoke.isDataReceived())
            {
                for (int k = 0; k < 16; k++)
                {
                    std::cout << std::hex << (int)(*(nucleoYoke.getRecieveBuffer()+k)) << " ";
                }
                std::cout << std::endl;
                nucleoYoke.receptionEnable();
            }
            if (_kbhit())
            {
                auto ch = _getch();
                if (ch == 27)
                {
                    break;
                }
            }
        }
        nucleoYoke.closeConnection();
    }
    else
    {
        std::cout << "failed to open yoke USB connection" << std::endl;
    }
}