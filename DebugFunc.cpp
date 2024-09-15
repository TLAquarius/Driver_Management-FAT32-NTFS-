#include "FAT32.h"
#include "NTFS.h"

void FATBootSector::printInfo()
{
    cout << "Byte/sec: " << hex << setw(2) << setfill('0') << (unsigned int)bytePerSec << endl;
    cout << "Sec/clus: " << hex << setw(2) << setfill('0') << (unsigned int)secPerClust << endl;
    cout << "Sec front FAT: " << hex << setw(2) << setfill('0') << (unsigned int)secFrontFAT << endl;
    cout << "FAT num: " << hex << setw(2) << setfill('0') << (unsigned int)fatNum << endl;
    cout << "Vol size: " << hex << setw(2) << setfill('0') << (unsigned int)volSize << endl;
    cout << "Sec/FAT: " << hex << setw(2) << setfill('0') << (unsigned int)secPerFat << endl;
    cout << "Start RDET clus: " << hex << setw(2) << setfill('0') << (unsigned int)startRdetClust << endl;
    cout << "Type: " << hex << setw(2) << setfill('0') << type << endl;
}

void PartitionEntry::printInfo()
{
    cout << "Status: " << hex << setw(2) << setfill('0') << (unsigned int)status << endl;
    cout << "Type: " << hex << setw(2) << setfill('0') << (unsigned int)partitionType << endl;
    cout << "Star: " << hex << setw(2) << setfill('0') << (unsigned int)startSec << endl;
    cout << "Num: " << hex << setw(2) << setfill('0') << (unsigned int)secNum << endl;
}

void printContentDebug(BYTE* content, int start, int end)
{
    int count = 1;
    for (int i = start; i < end && content != NULL; i++)
    {
        cout << hex << setw(2) << setfill('0') << (int)content[i] << "  ";
        if (count % 16 == 0)
        {
            cout << endl;
        }
        count++;
    }
    cout << endl;
}

void MainFatRdetEntry::printInfo()
{
    cout << "H: " << (int)hour << endl;
    cout << "M: " << (int)min << endl;
    cout << "S: " << (int)sec << endl;
    cout << "Y: " << (int)year << endl;
    cout << "M: " << (int)month << endl;
    cout << "D: " << (int)date << endl;
    cout << "Start: " << (int)startClus << endl;
    cout << "Size: " << (int)size << endl;

    cout << "Att: " << hex << setw(2) << setfill('0') << (int)att << endl;
    cout << "Arc: " << archive << endl;
    cout << "Dic: " << directory << endl;
    cout << "Vol: " << vol << endl;
    cout << "Sys: " << system << endl;
    cout << "Hid: " << hidden << endl;
    cout << "Rea: " << readOnly << endl;
}

void NTFSBootSector::printInfo()
{
    cout << "Byte/sec: " << hex << setw(2) << setfill('0') << (int)bytePerSec << endl;
    cout << "Sec/clus: " << hex << setw(2) << setfill('0') << (int)secPerClust << endl;
    cout << "Total sec: " << hex << setw(2) << setfill('0') << (int)totalSec << endl;
    cout << "Start RDET clus: " << hex << setw(2) << setfill('0') << (int)startClusMFT << endl;
    cout << "Start RDET clus: " << hex << setw(2) << setfill('0') << (int)startClusSubMFT << endl;
    cout << "Entry size: " << hex << setw(2) << setfill('0') << entrySize << endl;
}

void MFTEntry::printMFT()
{
    cout << "Id: " << hex << setw(2) << setfill('0') << header.entryId << endl;
    wcout << "Name: " << filename.back().name << endl;
    cout << "Parent: " << hex << setw(2) << setfill('0') << filename.back().parentId << endl;
}

void SystemItemFAT::printInfoDebug()
{
	name.resize(0);
	if (sub.size() > 0)
	{
		for (int i = sub.size() - 1; i >= 0; i--)
		{
			for (int j = 0; j < 5; j++)
			{
				if (sub[i].name1[j] == 0xff)
				{
					break;
				}
				name.push_back(sub[i].name1[j]);
			}
			for (int j = 0; j < 6; j++)
			{
				if (sub[i].name2[j] == 0xff)
				{
					break;
				}
				name.push_back(sub[i].name2[j]);
			}
			for (int j = 0; j < 2; j++)
			{
				if (sub[i].name3[j] == 0xff)
				{
					break;
				}
				name.push_back(sub[i].name3[j]);
			}
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (main.filename[i] == 0x20 || main.filename[i] == 0xff)
			{
				break;
			}
			name.push_back(main.filename[i]);
		}
		name.push_back('.');
		for (int i = 0; i < 3; i++)
		{
			if (main.filename[i] == 0x20 || main.filename[i] == 0xff)
			{
				break;
			}
			name.push_back(main.exName[i]);
		}
	}
	wcout << name << endl;
	for (int i = 0; i < child.size(); i++)
	{
		cout << "	";
		child[i].printInfoDebug();
	}
}