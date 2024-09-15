#include "FAT32.h"
#include "NTFS.h"

vector <wstring> getUSBPartPath()
{
    vector <wstring> driverList;
    // Enumerate disk drives
    WCHAR diskDrives[MAX_PATH] = { 0 };
    DWORD dwResult = GetLogicalDriveStringsW(MAX_PATH, diskDrives);
    if (dwResult > 0 && dwResult <= MAX_PATH) {
        WCHAR* driverName = diskDrives;
        while (*driverName) {
            // Check if the drive is a removable USB drive
            UINT driveType = GetDriveTypeW(driverName);
            if (driveType == DRIVE_REMOVABLE) {
                std::wcout << L"USB drive found: " << driverName << std::endl;
                wstring temp = driverName;
                wstring temp2 = L"\\\\.\\";
                temp.pop_back();
                temp = temp2 + temp;
                driverList.push_back(temp);
            }
            driverName += wcslen(driverName) + 1; // Move to the next drive
        }
    }
    else {
        cerr << "Failed to enumerate logical drives." << std::endl;
    }
    return driverList;
}

BYTE* ReadMBR(const wstring& devicePath)
{
    BYTE* mbr = new BYTE[mbrSize];
    mbr = readBuffer(devicePath, 0, mbrSize);
    return mbr;
}

vector <PartitionEntry> getPartInfo(BYTE* mbr)
{
    vector <PartitionEntry> partList;
    BYTE partTable[64];
    for (int i = 0; i < partTableSize; i++)
    {
        partTable[i] = mbr[partTableOffset + i];
    }

    for (int i = 0; i < 4; i++)
    {
        int offset = i * 16;
        PartitionEntry temp;
        extractByte<uint8_t>(partTable, temp.status, offset + 0x00, 1);
        extractByte<uint8_t>(partTable, temp.partitionType, offset + 0x04, 1);
        extractByte<uint32_t>(partTable, temp.startSec, offset + 0x08, 4);
        extractByte<uint32_t>(partTable, temp.secNum, offset + 0x0C, 4);
        partList.push_back(temp);
    }
    return partList;
}

int checkPartType(const wstring& driverPath)
{
    BYTE* buffer = new BYTE[512];
    buffer = readBuffer(driverPath, 0, 512);

    //Check NTFS
    uint64_t name;
    extractName(buffer, name, 0x03, 8);
    if (name == 0x4E54465320202020)
    {
        delete[] buffer;
        return 1;
    }

    //Check FAT
    name = 0;
    extractName(buffer, name, 0x52, 8);
    delete[] buffer;
    if (name == 0x4641543332202020)
        return 2;
    else
        return 0;
}

BYTE* readBuffer(const wstring& path, long long offset, long long size)
{
    HANDLE hDevice = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        cerr << "Failed to open USB driver: " << GetLastError() << endl;
        return NULL;
    }

    BYTE* buffer = new BYTE[size]();
    LARGE_INTEGER byteOffset;
    byteOffset.QuadPart = offset;

    if (offset != 0)
    {
        if (!SetFilePointer(hDevice, byteOffset.LowPart, &byteOffset.HighPart, FILE_BEGIN))
        {
            cerr << "Can't set file pointer: " << GetLastError() << endl;
        }
    }

    DWORD bytesRead;
    if (!ReadFile(hDevice, buffer, size, &bytesRead, NULL))
    {
        cerr << "Failed to read into buffer: " << GetLastError() << endl;
        CloseHandle(hDevice);
        delete[] buffer;
        return NULL;
    }
    CloseHandle(hDevice);
    return buffer;
}

string fileTimeToUnixTime(const uint64_t& ft)
{
    ULARGE_INTEGER ull;
    ull.QuadPart = ft;

    time_t dateTime = ull.QuadPart / 10000000ULL - 11644473600ULL;
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&dateTime));
    return buffer;
}