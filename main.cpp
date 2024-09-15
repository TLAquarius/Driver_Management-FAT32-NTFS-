#include "FAT32.h"
#include "NTFS.h"

int main() {
    // Assuming the USB device path is known or obtained from enumeration
    //std::wstring usbDevicePath = L"\\\\.\\PhysicalDrive1"; // Replace with the correct USB device path
    //// Read the MBR from the USB device
    //BYTE* mbr = new BYTE[mbrSize]();
    //mbr = ReadMBR(usbDevicePath);
    //if (mbr == NULL) {
    //    cerr << "Failed to read MBR from USB device." << std::endl;
    //    return 1;
    //}
    //printContentDebug(mbr, 0x1BE, 512);
    //vector <PartitionEntry> partEntry = getPartInfo(mbr);
    //partEntry[0].printInfo();

    //delete[] mbr;
    //cout << "MBR read successfully from USB device." << std::endl;

    vector <wstring> partPath = getUSBPartPath();

    for (wstring path : partPath)
    {
        int type = checkPartType(path);

        if (type == 1)
        {
            NTFSBootSector vbr = readNTFSBootSec(path);
            vector<MFTEntry> entryList = readMFT(path, vbr);
            NTFSItem root;
            for (int i = 0; i < entryList.size(); i++)
            {
                if (entryList[i].filename[0].name == L"."
                    && entryList[i].filename[0].system == 1
                    && entryList[i].filename[0].directory == 1)
                {
                    root.entry = entryList[i];
                    getNTFSItemInfo(entryList[i], root);
                    root.name = L"";
                    root.name.push_back(path[path.size() - 2]);
                    root.name.push_back(path[path.size() - 1]);
                    root.name.push_back(L'\\');
                    break;
                }
            }
            int tempNum = 0;
            root.child = buildNTFSTree(entryList, root, tempNum);
           // vbr.printInfo();
        }
        else if (type == 2)
        {
            FATBootSector fbs = readFATBootSec(path);
            //fbs.printInfo();
            vector<uint32_t> fat;
            fat = readFat(path, fbs);
            printVectorDebug<uint32_t>(fat, 0, fat.size());
            vector<uint32_t> rdetClus = getClusTrailFAT(fat,0x3d22);
            vector<vector<BYTE>> rdet = readClusTrailFAT(path, rdetClus, fbs);
            /*for (int i = 0; i < 16; i++)
            {
                printVectorDebug(rdet[i], 0, 32);
            }*/
            int a = 0;
            vector <SystemItemFAT> temp;
            temp = readDet(path, rdet, fat, fbs, a);
            for (int i = 0; i < temp.size(); i++)
            {
                temp[i].printInfoDebug();
                cout << endl;
            }
        }
        else
        {
            cout << "Driver format not supported\n";
        }
    }
    return 0;
}
