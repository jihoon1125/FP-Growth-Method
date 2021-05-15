#include "BpTree.h"
#include "limits.h"
#include "cmath"
bool BpTree::Insert(int key, set<string> set) {

	if (root == NULL)//at first
	{
		BpTreeNode* newnode = new BpTreeDataNode();
		FrequentPatternNode* newfreq = new FrequentPatternNode();
		newfreq->InsertList(set);
		newfreq->setFrequency(key);
		newnode->insertDataMap(key, newfreq);
		root = newnode;
		return true;
	}


	auto search_result = searchDataNode(key);

	if (search_result->getDataMap()->find(key) != search_result->getDataMap()->end())//if key overlapped
	{
		search_result->getDataMap()->find(key)->second->InsertList(set);
		search_result->getDataMap()->find(key)->second->setFrequency(key);
		return true;
	}

	/*if not existed, merge data */
	else
	{
		FrequentPatternNode* newfreq = new FrequentPatternNode();
		newfreq->InsertList(set);
		newfreq->setFrequency(key);
		search_result->insertDataMap(key, newfreq);
		if (excessDataNode(search_result) == false)//not overflowed
			return true;
		else {//if overflowed
			splitDataNode(search_result);
			auto curnode = search_result->getParent();
			while (curnode != NULL)//keep overflowing
			{
				if (excessIndexNode(curnode) == true) {
					splitIndexNode(curnode);
					curnode = curnode->getParent();
				}
				else
					break;

			}

		}


	}

	   

	return true;//overflow handling finished
}

BpTreeNode* BpTree::searchDataNode(int n) {
	BpTreeNode* pCur = root;		
	
	if (root == NULL)
		return NULL;

	
	for (; pCur->getDataMap() == NULL; )
	{
	
		auto idxmap = pCur->getIndexMap();

		int* A = new int[idxmap->size() + 2];
		int num = 1;

		A[0] = INT_MIN;
		A[idxmap->size() + 1] = INT_MAX;

		for (auto it = idxmap->begin(); it != idxmap->end(); it++)
		{
			A[num] = it->first;	num++;
		}

		for (int i = 0; i < idxmap->size() + 1; i++)
		{
			
			if (n >= A[i] && n < A[i + 1])
			{
				if (i == 0)
				{					
					pCur = pCur->getMostLeftChild();
				}
				else
					pCur = idxmap->find(A[i])->second;
				break;
			}
		}
	}

	
		return pCur;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode) {
	int split_loc = ceil((order - 1) / 2.0 + 1);
	BpTreeNode* newidxnode = new BpTreeIndexNode();
	BpTreeNode* newdatanode = new BpTreeDataNode();
	auto datamap = pDataNode->getDataMap();
	auto datamap_iter = datamap->begin();

	/* go to split location */
	for (int i = 0; i < split_loc - 1; i++)
		datamap_iter++;


	/* now split! */
	for (; datamap_iter != datamap->end(); )
	{
		newdatanode->insertDataMap(datamap_iter->first, datamap_iter->second);
		pDataNode->deleteMap((datamap_iter++)->first);
	}


	pDataNode->setNext(newdatanode);
	newdatanode->setPrev(pDataNode);

	/* create new indexnode */
	newidxnode->insertIndexMap(newdatanode->getDataMap()->begin()->first, newdatanode);


	if (pDataNode->getParent() == NULL)//it's first split
	{
		newdatanode->setParent(newidxnode);//setting newdatanode's parent as indexnode
		pDataNode->setParent(newidxnode);//setting pDataNode's parent if orphan
		pDataNode->getParent()->setMostLeftChild(pDataNode);
		root = newidxnode;
	}
	else {
		newdatanode->setParent(pDataNode->getParent());//setting newdatanode's parent as indexnode
		pDataNode->getParent()->insertIndexMap(newidxnode->getIndexMap()->begin()->first, newidxnode->getIndexMap()->begin()->second);
	}



}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
	int split_loc = ceil((order - 1) / 2.0 + 1);
	BpTreeNode* centernode = new BpTreeIndexNode();
	BpTreeNode* rightnode = new BpTreeIndexNode();

	auto indexmap = pIndexNode->getIndexMap();
	auto indexmap_iter = indexmap->begin();

	/* go to split location */
	for (int i = 0; i < split_loc - 1; i++)
		indexmap_iter++;

	centernode->insertIndexMap(indexmap_iter->first, rightnode);
	rightnode->setMostLeftChild(indexmap_iter->second);
	//pIndexNode->deleteMap(indexmap_iter->first);

	pIndexNode->deleteMap((indexmap_iter++)->first);

	/* now split and create rightnodes */
	for (; indexmap_iter != indexmap->end(); )
	{
		rightnode->insertIndexMap(indexmap_iter->first, indexmap_iter->second);
		pIndexNode->deleteMap((indexmap_iter++)->first);
	}


	/* let's merge */
	if (pIndexNode->getParent() == NULL)//no parent, create a new root
	{
		pIndexNode->setParent(centernode);
		centernode->setMostLeftChild(pIndexNode);
		rightnode->setParent(centernode);
		root = centernode;
	}

	else
	{
		rightnode->setParent(pIndexNode->getParent());
		pIndexNode->getParent()->insertIndexMap(centernode->getIndexMap()->begin()->first, centernode->getIndexMap()->begin()->second);
	}


	
}
bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
	if (pDataNode->getDataMap()->size() > order - 1) return true;//order is equal to the number of elements + 1
	else return false;
}

bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
	if (pIndexNode->getIndexMap()->size() > order - 1)return true;//order is equal to the number of elements + 1
	else return false;
}


/* Print item whose confidence is upper than min_confidence */
bool BpTree::printConfidence(string item, double item_frequency, double min_confidence)
{
	if ((root == NULL)||item_frequency==0) return false;

	int threshold = 0;
	bool pattern_exists = false;
	threshold = (item_frequency * min_confidence) + 1;

	auto node = searchDataNode(threshold);
	auto nodemap = node->getDataMap();
	auto map_iter = nodemap->begin();
	
	while (node != NULL)
	{
		nodemap = node->getDataMap();
		map_iter = nodemap->begin();

		for (; map_iter != nodemap->end(); map_iter++)
		{
			if (map_iter->first >= threshold)
			{
				auto list = map_iter->second->getList();
				for (auto list_iter = list.begin(); list_iter != list.end(); list_iter++)
				{
					if (list_iter->second.find(item) != list_iter->second.end())
					{
						if (pattern_exists == false)
							*fout << "StandardItem FrequentPattern Frequency Confidence" << endl;

						*fout << item << " -> ";
						printFrequentPatterns(list_iter->second, item);
						*fout << map_iter->first << " "<< map_iter->first / item_frequency << endl;
						pattern_exists = true;
					}
				}
			}
		}

		node = node->getNext();		
	}
	return pattern_exists;
}

/*print items whose frequency is upper than min_frequency */
bool BpTree::printFrequency(string item, int min_frequency)
{
	if (root == NULL) return false;
	auto node = searchDataNode(min_frequency);
	auto nodemap = node->getDataMap();
	auto map_iter = nodemap->begin();
	bool pattern_exists = false;
	while (node != NULL)
	{
		nodemap = node->getDataMap();
		map_iter = nodemap->begin();

		for (; map_iter != nodemap->end(); map_iter++)
		{
			if (map_iter->first >= min_frequency)
			{
				auto list = map_iter->second->getList();
				for (auto list_iter = list.begin(); list_iter != list.end(); list_iter++)
				{
					if (list_iter->second.find(item) != list_iter->second.end())
					{
						if(pattern_exists == false)
							*fout << "StandardItem FrequentPattern Frequency" << endl;
						*fout << item << " -> ";
						printFrequentPatterns(list_iter->second, item);
						*fout <<  map_iter->first << endl;
						pattern_exists = true;
					}
				}
			}
		}		
		
		node = node->getNext();		

	}

	return pattern_exists;
}

/* print items in range of (min,max) */
bool BpTree::printRange(string item, int min, int max) {
	if (root == NULL) return false;
	auto node = searchDataNode(min);
	auto nodemap = node->getDataMap();
	auto map_iter = nodemap->begin();
	bool finish = false;
	bool pattern_exists = false;
	while (node!=NULL)
	{
		nodemap = node->getDataMap();
		map_iter = nodemap->begin();

		for (; map_iter != nodemap->end(); map_iter++)
		{
			if (map_iter->first >= min && map_iter->first <= max)
			{
				auto list = map_iter->second->getList();
				for (auto list_iter = list.begin(); list_iter != list.end(); list_iter++)
				{
					if (list_iter->second.find(item) != list_iter->second.end())
					{
						if (pattern_exists == false)
							*fout << "StandardItem FrequentPattern Frequency" << endl;

						*fout << item << " -> ";
						printFrequentPatterns(list_iter->second, item);
						*fout <<  map_iter->first << endl;
						pattern_exists = true;
					}
				}
			}

			else if (map_iter->first > max)
			{
				finish = true;
				break;
			}

		}

		if (finish == true)
			break;

		node = node->getNext();
	}
	
	return pattern_exists;
}
/* print frequent patterns */
void BpTree::printFrequentPatterns(set<string> pFrequentPattern, string item) {
	*fout << "{";
	set<string> curPattern = pFrequentPattern;
	curPattern.erase(item);
	for (set<string>::iterator it = curPattern.begin(); it != curPattern.end();) {
		string temp = *it++;
		if (temp != item) *fout << temp;
		if (it == curPattern.end()) {
			*fout << "} ";
			break;
		}
		*fout << ", ";
	}
}
