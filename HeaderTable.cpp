#include "HeaderTable.h"

HeaderTable::~HeaderTable() {

}


void HeaderTable::insertTable(char* item, int frequency) {//inserting table
	string Item(item);
	FPNode* newFP; 	

	if (indexTable.size() != 0)
	{
		for (auto it = indexTable.begin(); it != indexTable.end(); it++)
		{
			if (it->second == item) {//if preexists, just update frequency
				it->first += frequency;
				dataTable[item]->updateFrequency(frequency);
				return;
			}
		}
	}
	/* else insert new table */
	newFP = new FPNode();
	newFP->setItem(item);	newFP->updateFrequency(frequency);
	indexTable.push_front(pair<int, string>(frequency, Item));
	dataTable.insert(pair<string, FPNode*>(Item, newFP));
}

/*finding curresoponding item*/
int HeaderTable::find_frequency(string item){
	auto map = getdataTable();
	if (map.find(item) == map.end())
		return 0;
	else
		return map.find(item)->second->getFrequency();
}
