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
                    uint8_t userDataBuffer[YokeInterface::SendBufferSize];
                    userDataBuffer[0] = REPORT_ID;
                    float fval = 0.5f;
                    memcpy(userDataBuffer+4, &fval, sizeof(fval));
                    fval = 0.2f;
                    memcpy(userDataBuffer + 8, &fval, sizeof(fval));
                    nucleoYoke.sendData(userDataBuffer);
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