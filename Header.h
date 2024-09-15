#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <Windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <cstdint>
#include <chrono>
#include <ctime>
using namespace std;

const int mbrSize = 512;
const int partTableSize = 64;
const int partTableOffset = 0x1BE;

class PartitionEntry
{
public:
	uint8_t status = 0;
	uint8_t partitionType = 0;
	uint32_t startSec = 0;
	uint32_t secNum = 0;
	void printInfo();
};

//class Folder :public SystemItemFAT
//{
//public:
//	vector<SystemItemFAT*> list;
//	Folder* parent;
//};
//
//class File :public SystemItemFAT
//{
//public:
//	Folder* parent;
//};

vector <PartitionEntry> getPartInfo(BYTE* mbr);
int checkPartType(const wstring& driverPath);
void printContentDebug(BYTE* content, int start, int end);
BYTE* readBuffer(const wstring& path, long long offset, long long size);
BYTE* ReadMBR(const std::wstring& devicePath);
vector <wstring> getUSBPartPath();
string fileTimeToUnixTime(const uint64_t& ft);

template<typename T>
void printVectorDebug(vector<T> content, long long start, long long end)
{
	int count = 1;
	for (int i = start; i < end; i++)
	{
		cout << hex << setw(8) << setfill('0') << (int)content[i] << "  ";
		if (count % 4 == 0)
		{
			cout << endl;
		}
		count++;
	}
	cout << endl;
}

template <typename T>
void extractByte(BYTE* scr, T& des, int offset, int size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (typeid(T) != typeid(uint64_t))
			des |= scr[offset + i] << (i * 8);
		else
		{
			des = (static_cast<uint64_t>(scr[offset + i]) << i * 8) | (des & 0xFFFFFFFFFFFFFFFF >> (size - i) * 8);
		}
	}
}

template <typename T>
void extractName(BYTE* scr, T& des, int offset, int size)
{
	for (int i = 0; i < size; i++)
	{
		des <<= 8;
		char temp = scr[offset + i];
		des |= scr[offset + i];
	}
}