#pragma once
#include "Header.h"
struct SubFatRdetEntry
{
public:
	wchar_t name1[5];
	wchar_t name2[6];
	wchar_t name3[2];
};

struct MainFatRdetEntry
{
public:
	string filename = "";
	string exName = "";
	uint8_t att = 0;
	bool archive = 0;
	bool directory = 0;
	bool vol = 0;
	bool system = 0;
	bool hidden = 0;
	bool readOnly = 0;
	uint8_t hour = 0;
	uint8_t min = 0;
	uint8_t sec = 0;
	uint16_t year = 0;
	uint16_t month = 0;
	uint16_t date = 0;
	uint32_t startClus = 0;
	uint32_t size = 0;
	void printInfo();
};

struct FATBootSector
{
public:
	uint16_t bytePerSec = 0;
	uint8_t secPerClust = 0;
	uint16_t secFrontFAT = 0;
	uint8_t fatNum = 0;
	uint32_t volSize = 0;
	uint32_t secPerFat = 0;
	uint32_t startRdetClust = 0;
	uint64_t type = 0;
	void printInfo();
};

class SystemItemFAT
{
public:
	wstring name = L"";
	MainFatRdetEntry main;
	vector<SubFatRdetEntry> sub;
	vector<SystemItemFAT> child;
	void printInfoDebug();
};

FATBootSector readFATBootSec(const wstring& driverPath);
vector<uint32_t> readFat(const wstring& driverPath, FATBootSector& fat);
MainFatRdetEntry getMainEntry(vector<BYTE>& entry);
SubFatRdetEntry getLFN(vector<BYTE>& entry);
SystemItemFAT getObjectInfo(vector<vector<BYTE>>& det, int& noEntry);
vector<vector<BYTE>> readClusTrailFAT(const wstring& driverPath, vector <uint32_t>& fatClusTrail, FATBootSector& fat);
vector <SystemItemFAT> readDet(const wstring& path, vector<vector<BYTE>>& det, vector<uint32_t>& fat, FATBootSector& fbs,int& noEntry);
vector <uint32_t> getClusTrailFAT(vector <uint32_t>& fatTable, long long startClus);
