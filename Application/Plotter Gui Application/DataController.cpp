#include "DataController.h"
DataController dataController;
void DataController::addData(std::string label, float x, float y) {
	if (this->indexMap[label] == 0) {
		this->datas.push_back(Data());
		this->datas[this->datas.size() - 1].label = label;
		this->indexMap[label] = this->datas.size();
		this->datas[this->indexMap[label] - 1].scrVec.addData(x, y, this->maxSize);
	}
	else {
		this->datas[this->indexMap[label] - 1].scrVec.addData(x, y, this->maxSize);
	}
}

void DataController::deleteData(int index){
	this->indexMap[this->datas[index].label] = 0;
	this->datas[index].scrVec.data.clear();
	this->datas.erase(this->datas.begin() + index);
}

void DataController::clear() {
	this->indexMap.clear();
	this->datas.clear();
}
