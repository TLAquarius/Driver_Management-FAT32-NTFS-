#include "FAT32.h"
#include "NTFS.h"


//File::File() :SystemItemFAT(""), size(0), parent(NULL) {}
//File::File(string name = "", double size = 0) :SystemItemFAT(name), size(size), parent(NULL) {}
//File::File(string name, double size, Folder* parent)
//	:SystemItemFAT(name), size(size), parent(parent)
//{
//	bool flag = false;
//	if (parent == NULL)
//		return;
//	for (int i = 0; i < parent->getList().size(); i++)
//	{
//		if (this->name == parent->getList()[i]->getName())
//		{
//			flag = true;
//			break;
//		}
//	}
//	if (flag == false)
//	{
//		parent->setList(this);
//	}
//}
//double File::sizeCal()
//{
//	return size;
//}
//void File::printInfo()
//{
//	cout << name << "\t" << size << " B\n\n";
//}
//
//Folder::Folder() :SystemItemFAT("") {}
//Folder::Folder(string name = "") :SystemItemFAT(name), parent(NULL) {}
//Folder::Folder(string name, vector<SystemItemFAT*> src) : SystemItemFAT(name), parent(NULL)
//{
//	for (int i = 0; i < src.size(); i++)
//	{
//		if (src[i] != NULL)
//		{
//			list.push_back(src[i]);
//		}
//	}
//}
//Folder::Folder(string name, Folder* parent) :SystemItemFAT(name), parent(parent)
//{
//	bool flag = false;
//	if (parent == NULL)
//		return;
//	for (int i = 0; i < parent->list.size(); i++)
//	{
//		if (this->name == parent->list[i]->getName())
//		{
//			flag = true;
//			break;
//		}
//	}
//	if (flag == false)
//	{
//		parent->list.push_back(this);
//	}
//}
//Folder::Folder(string name, vector<SystemItemFAT*> list, Folder* parent) :Folder(name, list)
//{
//	bool flag = false;
//	if (parent == NULL)
//		return;
//	for (int i = 0; i < parent->list.size(); i++)
//	{
//		if (this->name == parent->list[i]->getName())
//		{
//			flag = true;
//			break;
//		}
//	}
//	if (flag == false)
//	{
//		parent->list.push_back(this);
//	}
//}
//void Folder::addItem(SystemItemFAT* item)
//{
//	if (item != NULL)
//	{
//		list.push_back(item);
//	}
//}
//void Folder::addList(vector<SystemItemFAT*> src)
//{
//	for (int i = 0; i < src.size(); i++)
//	{
//		if (src[i] != NULL)
//		{
//			list.push_back(src[i]);
//		}
//	}
//}
//
//Folder* Folder::findFolder(string name)
//{
//	for (int i = 0; i < list.size(); i++)
//	{
//		Folder* temp = dynamic_cast<Folder*>(list[i]);
//		if (temp && temp->name == name)
//		{
//			return temp;
//		}
//		else if (temp)
//		{
//			return temp->findFolder(name);
//		}
//	}
//	return NULL;
//}
//
//double sizeCal(Folder* root)
//{
//	double sum = 0;
//	if (root != NULL)
//	{
//		for (int i = 0; i < root->list.size(); i++)
//		{
//			Folder* temp = dynamic_cast<Folder*>(root->list[i]);
//			if (temp)
//				sizeCal(temp);
//			else
//				sum += root->list[i]->sizeCal();
//		}
//	}
//	else
//		sum = -1;
//	return sum;
//}
//void Folder::printInfo(int level)
//{
//	cout << '[' << name << "]\n\n";
//	for (int i = 0; i < list.size(); i++)
//	{
//		cout << setw(level * 6) << ' ';
//		Folder* temp = dynamic_cast<Folder*>(list[i]);
//		if (temp)
//			temp->printInfo(level + 1);
//		else
//			list[i]->printInfo();
//
//	}
//}
//void Folder::countFileFolder()
//{
//	int file = 0;
//	int folder = 0;
//	for (int i = 0; i < list.size(); i++)
//	{
//		if (dynamic_cast<Folder*>(list[i]))
//			folder++;
//		else if (dynamic_cast<File*>(list[i]))
//			file++;
//	}
//	cout << "Thu muc [" << name << "] co: " << folder << " thu muc va " << file << " file con\n";
//}
//vector<SystemItemFAT*> Folder::getList()
//{
//	return list;
//}
//void Folder::setList(SystemItemFAT* src)
//{
//	list.push_back(src);
//}