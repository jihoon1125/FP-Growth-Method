#pragma once
#include "FPGrowth.h"
#include "BpTree.h"
#include "cstdlib"

class Manager
{
private:
	char* cmd;
	FPGrowth* fpgrowth;
	BpTree* bptree;
	FPGrowth* table_setting;
public:
	Manager(int threshold, int bpOrder)	//constructor
	{		
		cmd = NULL;
		flog.open("log.txt", ios::app);
		fpgrowth = new FPGrowth(&flog, 2);
		bptree = new BpTree(&flog, 3);
		table_setting = new FPGrowth(&flog, 2);
	}


	~Manager()//destructor
	{		
		delete fpgrowth;
		delete bptree;
	}

	ifstream fin;
	ofstream flog;	

	void run(const char* command);
	bool LOAD();
	bool BTLOAD();
	
	bool PRINT_ITEMLIST();
	bool PRINT_FPTREE();

	bool PRINT_RANGE(char* item, int start, int end);
	bool PRINT_CONFIDENCE(char* item, double rate);
	bool PRINT_MIN(char* item, int min_frequency);


	void printErrorCode(int n);
	void printSuccessCode();

};

