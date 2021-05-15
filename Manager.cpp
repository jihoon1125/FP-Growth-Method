#include "Manager.h"

void Manager::run(const char* command)
{
	fin.open(command);
	cmd = new char[32];
	
	if (!fin)
	{
		flog << "File Open Error" << endl;
		return;
	}
	while (!fin.eof())
	{
	
		fin.getline(cmd, 32);
		
		char* tmp = strtok(cmd, " ");
		if (tmp == NULL)
			break;
		if (strcmp(tmp, "LOAD") == 0)//LOAD CASE
		{
			flog << "========" << "LOAD" << "========" << endl;
			if (LOAD() && (strtok(NULL, " ") ==NULL))
			{
				printSuccessCode();
			}
			else {
				printErrorCode(100);
			}


		}

		else if (strcmp(tmp, "BTLOAD") == 0)//BTLOAD CASE
		{
			flog << "========" << "BTLOAD" << "========" << endl;
			if (BTLOAD() && (strtok(NULL, " ") == NULL))
			{
				printSuccessCode();
			}
			else {
				printErrorCode(200);
			}
		}

		else if (strcmp(tmp, "PRINT_ITEMLIST") == 0)//PRINT_ITEMLIST CASE
		{

			flog << "========" << "PRINT_ITEMLIST" << "========" << endl;
			if (PRINT_ITEMLIST() && (strtok(NULL, " ") == NULL))
			{
				flog << "======================" << endl << endl;
			}
			else {
				printErrorCode(300);
			}
		}

		else if (strcmp(tmp, "PRINT_FPTREE") == 0)//PRINT_FPTREE CASE
		{

			flog << "========" << "PRINT_FPTREE" << "========" << endl;
			if (PRINT_FPTREE() && (strtok(NULL, " ") == NULL))
			{
				flog << "======================" << endl << endl;
			}
			else {
				printErrorCode(400);
			}
		}

		else if (strcmp(tmp, "PRINT_MIN") == 0)//PRINT_MIN CASE
		{
			char* item;
			char* min_frequency;
			
			item = strtok(NULL, " ");
			min_frequency = strtok(NULL, " ");
			flog << "========" << "PRINT_MIN" << "========" << endl;			
			if ((item == NULL || min_frequency == NULL) || strtok(NULL, " ") != NULL)
			{
				printErrorCode(500);
			}		

			else if (PRINT_MIN(item, atoi(min_frequency)))
			{
				flog << "======================" << endl << endl;
			}
			else {
				printErrorCode(500);
			}
		}

		else if (strcmp(tmp, "PRINT_CONFIDENCE") == 0)//PRINT_CONFIDENCE CASE
		{
			char* item;
			char* min_confidence;
			item = strtok(NULL, " ");
			min_confidence = strtok(NULL, " ");

			flog << "========" << "PRINT_CONFIDENCE" << "========" << endl;
			if ((item == NULL || min_confidence == NULL) || strtok(NULL, " ") != NULL)
			{
				printErrorCode(600);
			}

			else if (PRINT_CONFIDENCE(item, atof(min_confidence)))
			{
				flog << "======================" << endl << endl;
			}
			else {
				printErrorCode(600);
			}
		}

		else if (strcmp(tmp, "PRINT_RANGE") == 0)//PRINT_RANGE CASE
		{
			char* item, *min, *max;			
			item = strtok(NULL, " ");
			min = (strtok(NULL, " "));
			max = (strtok(NULL, " "));

			flog << "========" << "PRINT_RANGE" << "========" << endl;
			
			if ((item == NULL || (min == NULL || max==NULL)) || strtok(NULL, " ") != NULL)
			{
				printErrorCode(700);
			}

			else if (PRINT_RANGE(item, atoi(min), atoi(max)))
			{
				flog << "======================" << endl << endl;
			}
			else {
				printErrorCode(700);
			}
		}

		else if (strcmp(tmp, "SAVE") == 0)
		{
		flog << "========" << "SAVE" << "========" << endl;
		if (strtok(NULL, " ") != NULL)
			printErrorCode(800);
		else
		{
			fpgrowth->FrequentPatternSetting();
			fpgrowth->saveFrequentPatterns();
		}
		}

		else if (strcmp(tmp, "EXIT") == 0)
		{
		flog << "========" << "EXIT" << "========" << endl;

		printSuccessCode();
		break;
		}

	}

		fin.close();
		return;
	
}
bool Manager::LOAD()
{
	if (fpgrowth->getHeaderTable()->getindexTable().size() != 0)//if data structure preexists
		return false;

	ifstream datain;
	char data[500];
	char* context = NULL;
	char* str;
	datain.open("market.txt");
	
	list<list<string>> transaction_set;
	if (!datain.is_open()) return false;   //file open failed

	while (!datain.eof())
	{				
		set<string> set;
		list<string> transaction;
		datain.getline(data, 500);
		
		
		str = strtok(data, "\t");
		if (str == NULL)
		{
			break;
		}

		set.insert(str);

		while (1)
		{			
			if ((str = strtok(NULL, "\t")) == NULL) break;
			
			set.insert(str);			
		}						

		for (auto it = set.begin(); it != set.end(); it++)
			transaction.push_back(*it);

		/* making raw index table */
		for (auto it = transaction.begin(); it != transaction.end(); it++)
			{			
				table_setting->createTable((char*)(*it).c_str(), 1);				
			}

		/* save each transactions */
		transaction_set.push_back(transaction);		
	}
	
	auto rawtable = table_setting->getHeaderTable()->getindexTable();

	/* cut off index which is lower than frequency */
	for (auto it = rawtable.begin(); it != rawtable.end(); it++)
	{		if(it->first>=fpgrowth->getThreshold())
				fpgrowth->createTable((char*)it->second.c_str(), it->first);		
	}

	fpgrowth->getHeaderTable()->descendingIndexTable();

	for (auto it = transaction_set.begin(); it != transaction_set.end(); it++) {
		list<string> sorted_item;
		list<pair<int, string>> index_table = fpgrowth->getHeaderTable()->getindexTable();//index_table is alreay refined

		/* sorting transaction must be done */
		for (auto it_2 = index_table.begin(); it_2 != index_table.end(); it_2++)
		{
			for (auto arr = it->begin(); arr != it->end(); arr++)
			{
				if ((it_2->second == *arr) && (it_2->first >= fpgrowth->getThreshold()))
					sorted_item.push_back(*arr);
			}
		}
		fpgrowth->createFPtree(fpgrowth->getTree(), fpgrowth->getHeaderTable(), sorted_item, 1);
	}
	
	//fpgrowth->FrequentPatternSetting();
	return true;
	
}




bool Manager::BTLOAD()
{
	if (bptree->getRoot()!=NULL)//if data structure preexists
		return false;

	ifstream datain;
	char data[500];
	char* context = NULL;
	char* str;
	datain.open("result.txt");	
	if (!datain.is_open()) return false;   //file open failed

	while (!datain.eof())
	{
		set<string> set;		
		int frequency;
		datain.getline(data, 500);
		

		str = strtok(data, "\t");
		
		if (str == NULL) break;
		
		frequency = atoi(str);
		

		while (1)
		{
			
			if ((str = strtok(NULL, "\t")) == NULL) { break;}
			
			set.insert(str);			
		}
		
		bptree->Insert(frequency, set);
	
	}
	return true;
}

bool Manager::PRINT_ITEMLIST() {
	if (table_setting->printList())
		return true;
	else
		return false;	
}

bool Manager::PRINT_FPTREE() {
	if (fpgrowth->printTree())
		return true;
	else
		return false;	
}

bool Manager::PRINT_MIN(char* item, int min_frequency) {
	if (bptree->printFrequency(item, min_frequency))
		return true;
	else
		return false;
}

bool Manager::PRINT_CONFIDENCE(char* item, double rate) {
	if (bptree->printConfidence(item, (double)fpgrowth->getHeaderTable()->find_frequency(item), rate))
		return true;
	else
		return false;
}

bool Manager::PRINT_RANGE(char* item, int start, int end) {
	if(bptree->printRange(item,start,end))
		return true;
	else
		return false;
}

void Manager::printErrorCode(int n) {				//ERROR CODE PRINT
	flog << "======== ERROR " << n << " ========" << endl;
	flog << "=======================" << endl << endl;
}

void Manager::printSuccessCode() {//SUCCESS CODE PRINT 
	flog << "Success" << endl;
	flog << "=======================" << endl << endl;
}

