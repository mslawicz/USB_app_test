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
    uint8_t* getRecieveBuffer(void) const { return const_cast<uint8_t*>(receiveBuffer); }
    bool openConnection(USHORT VID, USHORT PID, uint8_t collection);
    void closeConnection(void);
    void receptionEnable(void);
    bool isDataReceived(void);
    void resetReception(void) { ResetEvent(receiveOverlappedData.hEvent); } // clears the reception event (no signals until enabled again)
private:
    static const size_t SendBufferSize = 64;
    static const size_t ReceiveBufferSize = 256;
    static const size_t ReceivedDataSize = 64;
    uint8_t sendBuffer[SendBufferSize];
    uint8_t receiveBuffer[ReceiveBufferSize];
    GUID hidGuid;       // Human Interface Device
    HANDLE fileHandle;
    bool isOpen;        // true if the device is found and open
    OVERLAPPED sendOverlappedData;
    OVERLAPPED receiveOverlappedData;
    LPDWORD receivedDataCount;
};
