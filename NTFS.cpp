#include "NTFS.h"

NTFSBootSector readNTFSBootSec(const wstring& path)
{
	NTFSBootSector vbr;
	BYTE* buffer = new BYTE[512];
	buffer = readBuffer(path, 0, 512);

	extractByte<uint16_t>(buffer, vbr.bytePerSec, 0x0B, 2);
	extractByte<int8_t>(buffer, vbr.secPerClust, 0x0D, 1);
	extractByte<uint64_t>(buffer, vbr.totalSec, 0x28, 8);
	extractByte<uint64_t>(buffer, vbr.startClusMFT, 0x30, 8);
	extractByte<uint64_t>(buffer, vbr.startClusSubMFT, 0x38, 8);
	extractByte<int8_t>(buffer, vbr.entrySize, 0x40, 1);

	return vbr;
}

vector<MFTEntry> readMFT(const wstring& path, NTFSBootSector& bs)
{
	vector<MFTEntry> entryList;
	int entrySize = (bs.entrySize < 0) ? (1 << (abs(bs.entrySize))) : (bs.entrySize * bs.secPerClust * bs.bytePerSec);
	long long clusOffset = bs.secPerClust * bs.bytePerSec * bs.startClusMFT;
	int noEntry = 0;
	long long entryOffset = 0;
	do
	{
		entryOffset = entrySize * noEntry + clusOffset;
		BYTE* buffer = new BYTE[entrySize];
		buffer = readBuffer(path, entryOffset, entrySize);
		MFTEntry temp;
		extractName<uint32_t>(buffer, temp.header.sign, 0, 4);
		if (temp.header.sign == 0x42414144)
		{
			noEntry++;
			delete[]buffer;
			continue;
		}
		else if (temp.header.sign != 0x46494c45)
		{
			delete[]buffer;
			break;
		}

		extractByte(buffer, temp.header.attOffset, 0x14, 2);
		extractByte(buffer, temp.header.flag, 0x16, 2);
		extractByte(buffer, temp.header.entryId, 0x2c, 4);
		if (temp.header.attOffset == 0 || temp.header.flag == 0 || temp.header.flag == 0x02)
		{
			noEntry++;
			delete[]buffer;
			continue;
		}
		//Extract Standard_Infomation Attribute
		int curAttOffset = temp.header.attOffset;
		while (curAttOffset < 1024)
		{
			NonResAttHeader tempH;
			extractByte(buffer, tempH.type, curAttOffset, 4);
			extractByte(buffer, tempH.attLength, curAttOffset + 4, 4);
			extractByte(buffer, tempH.attOffset, curAttOffset + 0x14, 2);
			if (tempH.type == 0x10)
			{
				temp.info.header = tempH;
				int infoOffset = temp.info.header.attOffset + curAttOffset;
				extractByte(buffer, temp.info.createdTime, infoOffset, 8);
			}
			else if (tempH.type == 0x30)
			{
				FileName tempN;
				tempN.header = tempH;
				int nameOffset = tempN.header.attOffset + curAttOffset;
				extractByte(buffer, tempN.parentId, nameOffset, 6);
				uint32_t tempAtt = 0;
				extractByte(buffer, tempAtt, nameOffset + 56, 4);
				if (tempAtt == 0)
				{
					break;
				}
				tempN.readOnly = tempAtt & 1;
				tempN.hidden = (tempAtt >> 1) & 1;
				tempN.system = (tempAtt >> 2) & 1;
				tempN.archive = (tempAtt >> 5) & 1;
				tempN.directory = (tempAtt >> 28) & 1;

				extractByte(buffer, tempN.nameLength, nameOffset + 64, 1);
				for (int i = 0; i < tempN.nameLength; i++)
				{
					WCHAR tempChar = 0;
					extractByte<WCHAR>(buffer, tempChar, nameOffset + 66 + 2 * i, 2);
					tempN.name.push_back(tempChar);
				}
				temp.filename.push_back(tempN);
			}
			else if (tempH.type == 0x80)
			{
				ResAttHeader tempResH;
				extractByte(buffer, tempResH.flag, curAttOffset + 8, 1);
				if (tempResH.flag == 0)
				{
					break;
				}
				extractByte(buffer, temp.dataAtt.dataRunOffset, curAttOffset + 32, 2);
				extractByte(buffer, temp.dataAtt.realFileSize, curAttOffset + 48, 8);
				extractByte(buffer, temp.dataAtt.size, curAttOffset + temp.dataAtt.dataRunOffset, 1);
				int clusCountSize = 0;
				clusCountSize |= temp.dataAtt.size & 0x0F;
				int firstClusSize = 0;
				firstClusSize |= (temp.dataAtt.size >> 4) & 0x0F;

				extractByte(buffer, temp.dataAtt.clusCount, curAttOffset + temp.dataAtt.dataRunOffset + 1, clusCountSize);
				extractByte(buffer, temp.dataAtt.firstClus, curAttOffset + temp.dataAtt.dataRunOffset + 1 + clusCountSize, firstClusSize);
				break;
			}
			else if (tempH.type == 0 || tempH.type > 0x80)
			{
				break;
			}

			curAttOffset += tempH.attLength;
		}
		if (temp.filename.size() == 0)
		{
			noEntry++;
			delete[] buffer;
			continue;
		}

		if (temp.filename[0].system == 1
			&& temp.filename[0].name != L".")
		{
			noEntry++;
			delete[] buffer;
			continue;
		}

		entryList.push_back(temp);
		noEntry++;
		delete[]buffer;
	} while (true);

	return entryList;
}

vector<string> getAttListNTFS(FileName& scr)
{
	vector<string> list;
	if (scr.readOnly == 1)
		list.push_back("Read Only");
	else if (scr.hidden == 1)
		list.push_back("Hidden");
	else if (scr.system == 1)
		list.push_back("System");
	else if (scr.archive == 1)
		list.push_back("Archive");
	else if (scr.directory == 1)
		list.push_back("Directory");
	return list;
}

string changeSize(uint64_t size)
{
	string temp = "";
	if (size / (1024 * 1024 * 1024) > 0)
	{
		temp = to_string((double)size / (1024 * 1024 * 1024));
		temp += " GB";
	}
	else if (size / (1024 * 1024) > 0)
	{
		temp = to_string((double)size / (1024 * 1024));
		temp += " MB";
	}
	else if (size / 1024 > 0)
	{
		temp = to_string((double)size / 1024);
		temp += " KB";
	}
	else
	{
		temp = to_string(size);
		temp += " BYTE";
	}
	return temp;
}

void getNTFSItemInfo(MFTEntry& scr, NTFSItem& des)
{
	des.entry = scr;
	des.name = des.entry.filename.back().name;
	des.att = getAttListNTFS(des.entry.filename[0]);
	des.dateTime = fileTimeToUnixTime(des.entry.info.createdTime);
	des.size = changeSize(des.entry.dataAtt.realFileSize + 1024);
}

vector <NTFSItem> buildNTFSTree(vector<MFTEntry>& mft, NTFSItem& root, int& index)
{
	for (int i = 0; i < mft.size(); i++)
	{
		if (root.entry.header.entryId == mft[i].header.entryId)
		{
			continue;
		}
		if (mft[i].filename[0].parentId == root.entry.header.entryId)
		{
			NTFSItem temp;
			getNTFSItemInfo(mft[i], temp);
			root.child.push_back(temp);
			mft.erase(mft.begin() + i);
			mft.resize(mft.size());
			i--;
		}
		if (root.child.size() > 0)
		{
			if (root.child.back().entry.filename[0].directory)
			{
				i++;
				root.child.back().child = buildNTFSTree(mft, root.child.back(), i);
				i--;
			}
		}
	}
	return root.child;
}