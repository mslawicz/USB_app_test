#pragma once
#pragma comment(lib, "hid.lib")
//#pragma comment(lib, "SetupAPI.lib")

#include "Windows.h"
#include <cstdint>
#include <hidsdi.h>
//#include <SetupAPI.h>

class YokeInterface
{
public:
    YokeInterface();
    ~YokeInterface();
    uint8_t* getSendBuffer(void) const { return const_cast<uint8_t*>(sendBuffer); }
    bool openConnection(USHORT VID, USHORT PID, uint8_t collection);
private:
    static const size_t SendBufferSize = 64;
    static const size_t ReceiveBufferSize = 256;
    uint8_t sendBuffer[SendBufferSize]{};
    uint8_t receiveBuffer[ReceiveBufferSize]{};
    GUID hidGuid;       // Human Interface Device
    HANDLE fileHandle;
};
