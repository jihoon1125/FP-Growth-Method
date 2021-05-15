#include "FPGrowth.h"
using namespace std;

FPGrowth::~FPGrowth() {

}

void FPGrowth::createFPtree(FPNode* root, HeaderTable* table, list<string> item_array, int frequency) {
	FPNode* curNode = root;	

	/* create FP_tree */
	for (auto it = item_array.begin(); it != item_array.end(); it++)
	{
		
			if (curNode->getChildrenNode(*it) == NULL)//if no it existed in child, insert new node and update curNode
			{
				FPNode* newNode = new FPNode();
				newNode->setItem((char*)(it->c_str()));
				newNode->updateFrequency(frequency);
				newNode->setParent(curNode);
				curNode->pushchildren(*it, newNode);
				connectNode(table, *it, newNode);//connect node with dataTable
				curNode = newNode;
			}

			else//just increase frequency and update curNode
			{
				curNode->getChildrenNode(*it)->updateFrequency(frequency);
				curNode = curNode->getChildrenNode(*it);
			}
		
	}

}

void FPGrowth::connectNode(HeaderTable* table, string item, FPNode* node) {
	FPNode* cur = table->getNode(item);
	FPNode* prev = NULL;
	while (cur != NULL) {
		prev = cur;
		cur = cur->getNext();
	}
	prev->setNext(node);
}

/* check if it contatins single path */
bool FPGrowth::contains_single_path(FPNode* pNode) {
	if (pNode->getChildren().size() == 0) return true;
	else if (pNode->getChildren().size() > 1) return false;
	return contains_single_path(pNode->getChildren().begin()->second);
}

map<set<string>, int> FPGrowth::getFrequentPatterns(HeaderTable* pTable, FPNode* pTree) {
	list<pair<int, string>> indextable = (pTable->getindexTable());
	vector<string> combination;
	

	if (indextable.size() == 0) {
		cout << "Error: index 테이블 존재하지 않음" << endl;		
		return frequentPatterns;
	}

	HeaderTable prepathTable;
	auto it_m = indextable.rbegin();
	for (auto iter = indextable.rbegin(); iter!= indextable.rend(); iter++)
	{
		if (pTable->getNode(iter->second)->getNext() == NULL) {
			prepathTable.insertTable((char*)iter->second.c_str(), iter->first);
			combination.push_back(iter->second);
			it_m++;
			continue;
		}
		else
			break;
	}

	list<pair<int, string>> prepath = prepathTable.getindexTable();


	if (contains_single_path(pTree))//when single path in direct way
	{
		vector<string>data, data_2;
		string item;
		FPNode* child, prev;

		int frequency;		

		for (auto it = prepath.begin(); it != prepath.end(); it++)
		{
			item += it->second; item += "\t";			
		}

		
		for (auto it = indextable.begin(); it!=indextable.end(); it++)
			{
			if (pTable->getNode(it->second)->getNext() != NULL) {			
				data.push_back(it->second);
			}
			else
				break;
			}
		
		if(item.size()!=0)
			item.resize(item.size() - 1, item.size());	
		
		
		frequency = pTree->getChildren().begin()->second->getFrequency();
		
		int* ptr = new int[pTable->getindexTable().size()];				
		
		powerSet(&frequentPatterns, data, item, frequency, ptr, 0);


	}

	else//multi-path
	{				
		
		for (; it_m != indextable.rend(); it_m++)
		{						
			HeaderTable newTreeTable = prepathTable;				

			list<list<string>> item_arrayset;
			list<int> item_frequency;
			FPNode* curnode_ver, * curnode_hor;			
			HeaderTable table;
			curnode_hor = pTable->getdataTable().find(it_m->second)->second->getNext();
			
			
			/*new table for making new conditional FPtree*/			
			
			while (curnode_hor != NULL) {
				list<string> item_array;
				curnode_ver = curnode_hor;					

				while (curnode_ver->getFrequency()) {//until not root
					table.insertTable(curnode_ver->getItem(), curnode_hor->getFrequency());
					item_array.push_front(curnode_ver->getItem());					
					curnode_ver = curnode_ver->getParent();
				}				
				item_arrayset.push_back(item_array);//insert conditional transaction
				item_frequency.push_back(curnode_hor->getFrequency());
				curnode_hor = curnode_hor->getNext();				
			}

			//arranging table										
			list<pair<int, string>> idxtable = table.getindexTable();
						

			for (auto it = idxtable.rbegin(); it != idxtable.rend(); it++)
			{
				if (it->first >= threshold)
					newTreeTable.insertTable((char*)it->second.c_str(), it->first);		//arranged table		
			}					


				FPNode* cond_root = new FPNode();//conditional tree's root

				for (auto it_2 = item_arrayset.begin(); it_2 != item_arrayset.end(); it_2++)
				{
					list<string> arranged_itemarr;
					int frequency = item_frequency.front();					
					it_2->pop_back();
					for (auto it_3 = it_2->begin(); it_3 != it_2->end(); it_3++)//arranging itemset
					{
						if (newTreeTable.find_frequency(*it_3) != 0)
							arranged_itemarr.push_back(*it_3);
					}

					item_frequency.pop_front();
					createFPtree(cond_root, &newTreeTable, arranged_itemarr, frequency);
				}					

								
				int* ptr = new int[newTreeTable.getindexTable().size()];
				powerSet(&frequentPatterns, combination, it_m->second, it_m->first, ptr, 0);

				if(cond_root->getChildren().size()!=0)
					getFrequentPatterns(&newTreeTable, cond_root);

			}
		}	
	return {frequentPatterns};
}


/* getting frequent pattern set */
void FPGrowth::powerSet(map<set<string>, int>* FrequentPattern, vector<string> data, string item, int frequency, int* ptr, int depth) {
	if (data.size() == depth) {
		char* token;		
		set<string> set;

		if (item.size() != 0) {
			if (item.find("\t") == string::npos)
				set.insert(item);

			else {
				token = strtok((char*)item.c_str(), "\t");
				set.insert(token);
				while ((token = strtok(NULL, "\t")) != NULL)
					set.insert(token);
			}
		}
		for (int i = 0; i < data.size(); i++) { if (ptr[i] == 1) set.insert(data[i]); }			

		if (FrequentPattern->insert(make_pair(set, frequency)).second == false)//if overlaped, update frequency
		{
			if (FrequentPattern->find(set)->second < frequency) {
				FrequentPattern->at(set) = frequency;
			}
		}
			return;
	}
	ptr[depth] = 1;
	powerSet(FrequentPattern, data, item, frequency, ptr, depth + 1);
	ptr[depth] = 0;
	powerSet(FrequentPattern, data, item, frequency, ptr, depth + 1);	
}

bool FPGrowth::printList() {
	if (table->getindexTable().empty() == true)
		return false;
	else {
		table->descendingIndexTable();
		auto idxtable = table->getindexTable();
		*fout << "Item     Frequency" << endl;
		for (auto it = idxtable.begin(); it != idxtable.end(); it++)
		{
			*fout << it->second << " " << it->first << endl;
		}
		
	}
	return true;
}
bool FPGrowth::printTree() {
	if (getTree()->getChildren().size() == 0)//empty tree
	{
		return false;
	}

	else {
		FPNode* curnode_ver, * curnode_hor;
		table->ascendingIndexTable();
		list<pair<int, string>> indextable = table->getindexTable();
		*fout << "{StandardItem, Frequency} (Path_Item, Frequency)" << endl;
		for (auto it = indextable.begin(); it != indextable.end(); it++)
		{
			curnode_hor = table->getdataTable().find(it->second)->second->getNext();

			*fout << "{" << it->second << "," << it->first << "}" << endl;

			/* printing fptree in table's ascending order */
			while (curnode_hor != NULL) {
				curnode_ver = curnode_hor;
				while (curnode_ver != fpTree) {//until not root	
					*fout << '(' << curnode_ver->getItem() << ',' << curnode_ver->getFrequency() << ") ";
					curnode_ver = curnode_ver->getParent();
				}
				curnode_hor = curnode_hor->getNext();
				*fout << endl;
			}
		}

		
	}
	return true;
}
void FPGrowth::saveFrequentPatterns(){
	if (frequentPatterns.empty() == true)
		*fout << "========" << " ERROR 800 " << "=========" << endl;
	else {
		for (auto it = frequentPatterns.begin(); it != frequentPatterns.end(); it++)
		{
			int frequency = it->second;			
			if (it->first.size() == 0)
				continue;
			else if (it->first.size() == 1) {
				if(it->first.begin()->find("\t")==string::npos)
					continue;
			}
			
			flog << frequency;
			for (auto it_2 = it->first.begin(); it_2 != it->first.end(); it_2++)
				flog << "\t" << *it_2;
			flog << '\n';
		}
		*fout << "Success" << endl;
		flog.close();
	}

	*fout << "======================" << endl << endl;
}

