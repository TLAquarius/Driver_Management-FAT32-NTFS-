#pragma once
#include "Header.h"

struct NonResAttHeader
{
	uint32_t type = 0;
	uint32_t attLength = 0;
	uint16_t attOffset = 0;
};

struct ResAttHeader
{
	uint8_t flag = 0;
};

struct StandardInfo
{
	uint64_t createdTime = 0;
	NonResAttHeader header;
};

struct FileName
{
	uint64_t parentId = 0;
	wstring name = L"";
	NonResAttHeader header;
	uint8_t nameLength = 0;
	bool readOnly = 0;
	bool hidden = 0;
	bool system = 0;
	bool archive = 0;
	bool directory = 0;
};

struct Data
{
	uint64_t realFileSize = 0;
	uint16_t dataRunOffset = 0;
	uint8_t size = 0;
	uint64_t clusCount = 0;
	uint64_t firstClus = 0;
};

struct MFTHeader
{
	uint32_t sign = 0;
	uint32_t entryId = 0;
	uint16_t flag = 0;
	uint16_t attOffset = 0;
};

struct MFTEntry
{
	MFTHeader header;
	vector<FileName> filename;
	Data dataAtt;
	StandardInfo info;
	void printMFT();
};

class NTFSItem
{
public:
	wstring name = L"";
	MFTEntry entry;
	string size = "";
	string dateTime = "";
	vector<string> att;
	vector<NTFSItem> child;
};

class NTFSBootSector
{
public:
	uint16_t bytePerSec = 0;
	int8_t secPerClust = 0;
	uint64_t totalSec = 0;
	uint64_t startClusMFT = 0;
	uint64_t startClusSubMFT = 0;
	int8_t entrySize = 0;
	void printInfo();
};

NTFSBootSector readNTFSBootSec(const wstring& path);
vector<string> getAttListNTFS(FileName& scr);
string changeSize(uint64_t size);
vector<MFTEntry> readMFT(const wstring& path, NTFSBootSector& bs);
vector <NTFSItem> buildNTFSTree(vector<MFTEntry>& mft, NTFSItem& root, int& index);
void getNTFSItemInfo(MFTEntry& scr, NTFSItem& des);