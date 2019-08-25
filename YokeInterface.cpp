#pragma comment(lib, "SetupAPI.lib")

#include "YokeInterface.h"
#include <SetupAPI.h>
#include <iostream>
#include <string>

YokeInterface::YokeInterface()
{
    hidGuid = CLSID_NULL;
    fileHandle = INVALID_HANDLE_VALUE;
    isOpen = false;
}

YokeInterface::~YokeInterface()
{

}

// check all connected HID devices and open desired USB connection
bool YokeInterface::openConnection(USHORT VID, USHORT PID, uint8_t collection)
{
    bool found = false;     // mark that the device has been found
    bool isOpen = false;
    HidD_GetHidGuid(&hidGuid);

    // SetupDiGetClassDevs function returns a handle to a device information set that contains requested device information elements for a local computer
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        std::cout << "Invalid handle to device information set, error code=" << GetLastError() << std::endl;
        return isOpen;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    // SetupDiEnumDeviceInfo function returns a SP_DEVINFO_DATA structure that specifies a device information element in a device information set
    for (int deviceIndex = 0; SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData) && !found; deviceIndex++)
    {
        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        // SetupDiEnumDeviceInterfaces function enumerates the device interfaces that are contained in a device information set
        for (int interfaceIndex = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, &deviceInfoData, &hidGuid, interfaceIndex, &devInterfaceData) && !found; interfaceIndex++)
        {
            std::cout << "testing device " << deviceIndex << " / interface " << interfaceIndex << std::endl; //qqq

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
                std::cout << " memory allocation error" << std::endl; //qqq
                continue;
            }

            // SetupDiGetDeviceInterfaceDetail function returns details about a device interface
            // this second call is for getting actual information about the device that supports the requested interface
            if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &devInterfaceData, pDeviceInterfaceDetailData, bufferSize, &bufferSize, &deviceInfoData))
            {
                std::wcout << "," << pDeviceInterfaceDetailData->DevicePath << std::endl; //qqq

                SECURITY_ATTRIBUTES securityAttributes;
                memset(&securityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));
                securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
                securityAttributes.bInheritHandle = true;

                // Creates or opens a file or I/O device
                // query metadata such as file, directory, or device attributes without accessing device 
                fileHandle = CreateFile(pDeviceInterfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    &securityAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                std::cout << "handle=" << fileHandle << std::endl; //qqq

                if (fileHandle != INVALID_HANDLE_VALUE)
                {
                    HIDD_ATTRIBUTES	attributes;
                    memset(&attributes, 0, sizeof(HIDD_ATTRIBUTES));
                    attributes.Size = sizeof(HIDD_ATTRIBUTES);
                    if (HidD_GetAttributes(fileHandle, &attributes))
                    {
                        CloseHandle(fileHandle);
                        std::wstring collectionStr = L"&col";
                        if (collection < 10)
                        {
                            collectionStr += L"0";
                        }
                        collectionStr += std::to_wstring(collection);
                        std::cout << std::hex << " VID=" << attributes.VendorID << " PID=" << attributes.ProductID << " ver=" << attributes.VersionNumber; //qqq
                        std::wcout << L" col=" << collectionStr << std::endl; //qqq   

                        if((attributes.VendorID == VID) &&
                            (attributes.ProductID == PID) &&
                            (wcsstr(pDeviceInterfaceDetailData->DevicePath, collectionStr.c_str())))
                        {
                            // device with proper collection found
                            // Creates or opens a file or I/O device - this time for read/write operations in asynchronous mode
                            fileHandle = CreateFile(pDeviceInterfaceDetailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                &securityAttributes, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
                            if (fileHandle != INVALID_HANDLE_VALUE)
                            {
                                std::cout << ", open OK"; //qqq
                                isOpen = true;
                            }
                            else
                            {
                                std::cout << ", cannot open for read/write, error code = " << GetLastError();
                            }
                            // mark that the device has been found regardless if it has been opened
                            found = true;
                        }
                        else //qqq
                        {
                            std::cout << "not looking for this device/collection" << std::endl; //qqq
                        }
                    }
                    else //qqq
                    {
                        std::cout << "cannot get attributes!!!" << std::endl;
                    }
                }
                else //qqq
                {
                    std::cout << "invalid file handle, error code = " << GetLastError() << std::endl;
                }
            }
            else //qqq
            {
                std::cout << "couldn't get device interface details; error code = " << GetLastError() << std::endl;
            }
            std::cout << std::endl; //qqq
            free(pDeviceInterfaceDetailData);
        }
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return isOpen;
}

void YokeInterface::closeConnection(void)
{
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        std::cout << "handle before close=" << fileHandle << std::endl; //qqq
        CloseHandle(fileHandle);
        fileHandle = INVALID_HANDLE_VALUE;
        std::cout << "handle after close=" << fileHandle << std::endl; //qqq
    }
    isOpen = false;
}

