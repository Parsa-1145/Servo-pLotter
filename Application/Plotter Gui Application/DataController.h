#pragma once
#include "Common.h"
#include "Data.h"

struct DataController {
	std::map<std::string, int> indexMap;
	std::vector<Data> datas;
	int maxSize = 2000;

	void addData(std::string label, float x, float y);
	void deleteData(int index);
	void clear();
};
extern DataController dataController;