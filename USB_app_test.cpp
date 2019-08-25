// USB_app_test.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "YokeInterface.h"
#include <iostream>
#include <conio.h>

int main()
{
    YokeInterface nucleoYoke;
    bool opened = nucleoYoke.openConnection(VENDOR_ID, PRODUCT_ID, REPORT_ID);
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
                if (ch == 115) // 's'
                {
                    // send data here
                    static uint8_t cnt = 0;
                    uint8_t* buffer = nucleoYoke.getSendBuffer();
                    int val = 0x12345678;
                    memcpy(buffer+1, &val, sizeof(val));
                    buffer[1 + sizeof(val)] = ++cnt;
                    nucleoYoke.sendData();
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