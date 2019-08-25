#pragma comment(lib, "SetupAPI.lib")

#include "YokeInterface.h"
#include <SetupAPI.h>
#include <iostream>

YokeInterface::YokeInterface()
{
    hidGuid = CLSID_NULL;

}

YokeInterface::~YokeInterface()
{

}

// check all connected HID devices and open desired USB connection
bool YokeInterface::openConnection(void)
{
    HidD_GetHidGuid(&hidGuid);

    // SetupDiGetClassDevs function returns a handle to a device information set that contains requested device information elements for a local computer
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        std::cout << ": couldn't get device interface details; error code = " << GetLastError();
        return false;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    // SetupDiEnumDeviceInfo function returns a SP_DEVINFO_DATA structure that specifies a device information element in a device information set
    for (int deviceIndex = 0; SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData); deviceIndex++)
    {
        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        // SetupDiEnumDeviceInterfaces function enumerates the device interfaces that are contained in a device information set
        for (int interfaceIndex = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, &deviceInfoData, &hidGuid, interfaceIndex, &devInterfaceData); interfaceIndex++)
        {
            std::cout << deviceIndex << "/" << interfaceIndex;

            DWORD bufferSize = 0;
            // SetupDiGetDeviceInterfaceDetail function returns details about a device interface
            // this first call is for getting required size of the DeviceInterfaceDetailData buffer
            SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &devInterfaceData, NULL, 0, &bufferSize, &deviceInfoData);

            // pointer to buffer that receives information about the device that supports the requested interface
            PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(bufferSize);
            if (pDeviceInterfaceDetailData != nullptr)
            {
                pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            }
            else
            {
                std::cout << " memory allocation error" << std::endl;
                continue;
            }

            // SetupDiGetDeviceInterfaceDetail function returns details about a device interface
            // this second call is for getting actual information about the device that supports the requested interface
            if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &devInterfaceData, pDeviceInterfaceDetailData, bufferSize, &bufferSize, &deviceInfoData))
            {
                std::wcout << "," << pDeviceInterfaceDetailData->DevicePath << ",";

                SECURITY_ATTRIBUTES securityAttributes;
                memset(&securityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
                securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
                securityAttributes.bInheritHandle = true;

                // Creates or opens a file or I/O device
                // query metadata such as file, directory, or device attributes without accessing device 
                HANDLE fileHandle = CreateFile(pDeviceInterfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    &securityAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                std::cout << ":" << fileHandle;

                if (fileHandle != INVALID_HANDLE_VALUE)
                {
                    HIDD_ATTRIBUTES	attributes;
                    memset(&attributes, 0, sizeof(HIDD_ATTRIBUTES));
                    attributes.Size = sizeof(HIDD_ATTRIBUTES);
                    if (!HidD_GetAttributes(fileHandle, &attributes))
                    {
                        std::cout << ", cannot get attributes!!!";
                    }
                    std::cout << std::hex << " VID=" << attributes.VendorID << " PID=" << attributes.ProductID << " ver=" << attributes.VersionNumber;

                    if (CloseHandle(fileHandle) == 0)
                    {
                        std::cout << ": cannot close the file, error code = " << GetLastError();
                    }

                    // Creates or opens a file or I/O device
                    // this time for read/write operations in asynchronous mode (not all HID devices allow it)
                    fileHandle = CreateFile(pDeviceInterfaceDetailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                        &securityAttributes, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
                    //fileHandle = CreateFile(pDeviceInterfaceDetailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    //    &securityAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (fileHandle != INVALID_HANDLE_VALUE)
                    {
                        std::cout << ", open OK";
                        if ((attributes.VendorID == 0x483) && (wcsstr(pDeviceInterfaceDetailData->DevicePath, L"col03")))
                        {
                            //LPDWORD rdDataCount = new DWORD;
                            //LPDWORD wrDataCount = new DWORD;
                            //memset(&readOverlappedData, 0, sizeof(readOverlappedData));
                            //memset(&writeOverlappedData, 0, sizeof(writeOverlappedData));
                            //readOverlappedData.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                            //writeOverlappedData.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                            //writeBuffer[0] = 3; // report_ID
                            //writeBuffer[1] = 0x0A;
                            //writeBuffer[2] = 0x0B;
                            //auto startTime = std::chrono::high_resolution_clock::now();
                            //auto rdResult = ReadFile(fileHandle, readBuffer, 64, rdDataCount, &readOverlappedData);
                            //auto wrResult = WriteFile(fileHandle, writeBuffer, 64, wrDataCount, &writeOverlappedData); //takes ~200us
                            ////auto result = WriteFile(fileHandle, dataBuffer, 65, dataCount, NULL); // message length must be 1+(no of bytes in report descriptor); takes ~30ms
                            //auto stopTime = std::chrono::high_resolution_clock::now();
                            //// for ReadFile res=0, cnt=0 and err=0x3E5 (ERROR_IO_PENDING) are expected
                            //std::cout << std::dec << ", rRes=" << rdResult << ", wRes=" << wrResult << " wrCnt=" << *wrDataCount << " err=" << GetLastError();
                            //std::cout << " r-wTime=" << std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count();
                            //std::cout << ", wrOvr=" << GetOverlappedResult(fileHandle, &writeOverlappedData, wrDataCount, FALSE);
                            //while (!GetOverlappedResult(fileHandle, &writeOverlappedData, wrDataCount, FALSE));
                            //std::cout << ", WR completed";
                            //auto waitResult = WaitForSingleObject(readOverlappedData.hEvent, 5000);
                            //auto endTime = std::chrono::high_resolution_clock::now();
                            ////std::cout << " cnt=" << *dataCount << " err=" << GetLastError();
                            //std::cout << " res=" << waitResult << " err=" << GetLastError();
                            //std::cout << " endTime=" << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
                            //if (waitResult == WAIT_OBJECT_0)
                            //{
                            //    std::cout << std::endl;
                            //    for (int n = 0; n < 16; n++)
                            //    {
                            //        std::cout << std::hex << (int)readBuffer[n] << " ";
                            //    }
                            //    std::cout << std::endl;
                            //}
                        }
                        CloseHandle(fileHandle);
                    }
                    else
                    {
                        std::cout << ", cannot open for read/write, error code = " << GetLastError();
                    }
                }
                else
                {
                    std::cout << ": error code = " << GetLastError();
                }
            }
            else
            {
                std::cout << ": couldn't get device interface details; error code = " << GetLastError();
            }
            std::cout << std::endl;
            free(pDeviceInterfaceDetailData);
        }
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);

}

