#include "FAT32.h"

FATBootSector readFATBootSec(const wstring& driverPath)
{
    FATBootSector fatBoot;
   
    BYTE* buffer = new BYTE[512];
    buffer = readBuffer(driverPath, 0, 512);

    //printContentDebug(buffer, 0, 512);
    extractByte<uint16_t>(buffer, fatBoot.bytePerSec, 0x0B, 2);
    extractByte<uint8_t>(buffer, fatBoot.secPerClust, 0x0D, 1);
    extractByte<uint16_t>(buffer, fatBoot.secFrontFAT, 0x0E, 2);
    extractByte<uint8_t>(buffer, fatBoot.fatNum, 0x10, 1);
    extractByte<uint32_t>(buffer, fatBoot.volSize, 0x20, 4);
    extractByte<uint32_t>(buffer, fatBoot.secPerFat, 0x24, 4);
    extractByte<uint32_t>(buffer, fatBoot.startRdetClust, 0x2C, 4);
    extractName<uint64_t>(buffer, fatBoot.type, 0x52, 8);

    delete[] buffer;
    return fatBoot;
}

vector<uint32_t> readFat(const wstring& driverPath, FATBootSector& fat)
{
    vector<uint32_t> fatTable;
    
    long long byteFrontFat = fat.secFrontFAT * fat.bytePerSec;

    long long fatSize = fat.secPerFat * fat.bytePerSec;
    BYTE* buffer = new BYTE[fatSize];
    buffer = readBuffer(driverPath, byteFrontFat, fatSize);
    //printContentDebug(buffer, 0, 1024);
    long long i = 0;
    while (i < fatSize)
    {
        uint32_t temp = 0;
        temp |= buffer[i++];
        temp |= buffer[i++] << 8;
        temp |= buffer[i++] << 16;
        temp |= buffer[i++] << 24;
        if (temp == 0)
        {
            break;
        }
        fatTable.push_back(temp);
    }
    delete[] buffer;
    return fatTable;
}

SubFatRdetEntry getLFN(vector<BYTE>& entry)
{
    SubFatRdetEntry lfn;
    int offset1 = 0x1;
    int offset2 = 0xE;
    int offset3 = 0x1C;
    
    for (int i = 0; i < 5; i++)
    {
        lfn.name1[i] = 0;
        lfn.name1[i] |= entry[offset1++];
        lfn.name1[i] |= entry[offset1++] << 8;
    }

    for (int i = 0; i < 6; i++)
    {
        lfn.name2[i] = 0;
        lfn.name2[i] |= entry[offset2++];
        lfn.name2[i] |= entry[offset2++] << 8;
    }

    for (int i = 0; i < 2; i++)
    {
        lfn.name3[i] = 0;
        lfn.name3[i] |= entry[offset3++];
        lfn.name3[i] |= entry[offset3++] << 8;
    }
    return lfn;
}

MainFatRdetEntry getMainEntry(vector<BYTE>& entry)
{
    MainFatRdetEntry main;
    int offsetName = 0;
    int offsetEx = 0x8;
    int offsetAtt = 0xb;
    int offsetTime = 0xE;
    int offsetDate = 0x10;
    int offsetClusH = 0x14;
    int offsetClusL = 0x1a;
    int offsetSize = 0x1C;

    for (int i = 0; i < 8; i++)
    {
        main.filename.push_back(entry[offsetName + i]);
    }
   
    for (int i = 0; i < 3; i++)
    {
        main.exName.push_back(entry[offsetEx + i]);
    }
    
    main.att = entry[offsetAtt];
    main.archive = (main.att & 0x20) >> 5;
    main.directory = (main.att & 0x10) >> 4;
    main.vol = (main.att & 0x08) >> 3;
    main.system = (main.att & 0x04) >> 2;
    main.hidden = (main.att & 0x02) >> 1;
    main.readOnly = main.att & 0x01;

   
    uint16_t time = 0;
    for (int i = 0; i < 2; i++)
    {
        time |= entry[offsetTime + i] << 8*i;
    }
    main.hour = (time >> 11) & 0x1f;
    main.min = (time >> 5) & 0x3f;
    main.sec = (time & 0x1f) * 2;

    uint16_t tempDate = 0;
    for (int i = 0; i < 2; i++)
    {
        tempDate |= entry[offsetDate + i] << 8 * i;
    }
    main.year = ((tempDate >> 9) & 0x7f) + 1980;
    main.month = (tempDate >> 5) & 0x0f;
    main.date = tempDate & 0x1f;
    
    uint16_t startClusHigh = 0;
    uint16_t startClusLow = 0;
    for (int i = 0; i < 2; i++)
    {
        startClusHigh |= entry[offsetClusH + i] << 8 * i;
    }
    for (int i = 0; i < 2; i++)
    {
        startClusLow |= entry[offsetClusL + i] << 8 * i;
    }
    main.startClus |= startClusHigh;
    main.startClus <<= 16;
    main.startClus |= startClusLow;

    for (int i = 0; i < 4; i++)
    {
        main.size |= entry[offsetSize + i] << 8 * i;
    }

    return main;
}

SystemItemFAT getObjectInfo(vector<vector<BYTE>>& det, int& noEntry)
{
    SystemItemFAT root;

    for (int i = noEntry; i < det.size(); i++)
    {
        if (det[i][0] == 0xe5)
        {
            continue;
        }
        else if (det[i][0] == 0)
        {
            break;
        }
        if (det[i][0xB] == 0xF)
        {
            root.sub.push_back(getLFN(det[i]));
        }
        else
        {
            root.main = getMainEntry(det[i]);
            noEntry = i + 1;
            break;
        }
    }
    return root;
}

vector <SystemItemFAT> readDet(const wstring& path, vector<vector<BYTE>>& det, vector<uint32_t>& fat, FATBootSector& fbs, int& noEntry)
{
    vector <SystemItemFAT> root;
    while (noEntry < det.size())
    {
        SystemItemFAT item = getObjectInfo(det, noEntry);
        if (item.main.filename == "" || item.main.att == 0)
        {
            break;
        }
        if (item.main.directory && !item.main.system)
        {
            vector<uint32_t> subDetClus = getClusTrailFAT(fat, item.main.startClus);
            vector<vector<BYTE>> subDet = readClusTrailFAT(path, subDetClus, fbs);
            int tempNo = 2;
            item.child = readDet(path, subDet, fat, fbs, tempNo);
        }
        
        root.push_back(item);
    }
    return root;
}


vector <uint32_t> getClusTrailFAT(vector <uint32_t>& fatTable, long long startClus)
{
    vector <uint32_t> rdetClus;
    uint32_t temp = startClus;
    rdetClus.push_back(temp);
    while (fatTable[temp + 8] != 0x0FFFFFFF && fatTable[temp + 8] != 0x0FFFFFF7 && temp > 1)
    {
        rdetClus.push_back(fatTable[temp + 8]);
        temp = fatTable[temp + 8];
    }
    //printVectorDebug<uint32_t>(rdetClus, 0, rdetClus.size());
    return rdetClus;
}

vector<vector<BYTE>> readClusTrailFAT(const wstring& driverPath, vector <uint32_t>& fatClusTrail, FATBootSector& fat)
{
    vector<vector<BYTE>> rdet;
    long long byteOfFat = fat.fatNum * fat.secPerFat * fat.bytePerSec;
    long long byteFrontFat = fat.secFrontFAT * fat.bytePerSec;
    long long byteFrontClus = 0;
    long long clusOffset = 0;
    int clusSize = fat.secPerClust * fat.bytePerSec;
    BYTE* buffer = new BYTE[clusSize];
    for (int i = 0; i < fatClusTrail.size(); i++)
    {
        byteFrontClus = (fatClusTrail[i] - 2) * clusSize;
        clusOffset = byteFrontFat + byteOfFat + byteFrontClus;
        buffer = readBuffer(driverPath, clusOffset, clusSize);

        int offset = 0;
        while (offset <= clusSize - 32)
        {
            vector<BYTE> temp;
            int j = 0;
            while (j < 32)
            {
                temp.push_back(buffer[offset + j]);
                j++;
            }
            rdet.push_back(temp);
            offset += 32;
        }
    }
    delete[] buffer;
    return rdet;
}