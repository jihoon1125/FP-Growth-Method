#pragma once
#include "FPNode.h"
#include <list>

class HeaderTable
{
private:
	list<pair<int, string> > indexTable;
	map<string, FPNode*> dataTable;
public:
	HeaderTable() { }
	~HeaderTable();
	void insertTable(char* item, int frequency);//insert list, map in indexTable, dataTable
	list<pair<int, string>> getindexTable() 
	{ return indexTable; }
	map<string, FPNode*> getdataTable() { return dataTable; }
	FPNode* getNode(string item) { return dataTable.find(item)->second; }
	void descendingIndexTable() { indexTable.sort(greater<pair<int, string>>()); }//indextable in descending order
	void ascendingIndexTable() { indexTable.sort(); }//indexTable in ascending order
	int find_frequency(string item);//return item's frequency
};


