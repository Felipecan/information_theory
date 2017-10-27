
#include "Data.h"

Data::Data(unsigned char byte, unsigned long freq) : byte(byte), freq(freq), left(nullptr), right(nullptr), father(nullptr) {}

unsigned char Data::getCharacter() const{
	return byte;
}

unsigned long Data::getFrequency() const{
	return freq;
}

void Data::setLeft(Data* left){
	this->left = left;
}

void Data::setRight(Data* right){
	this->right = right;
}

void Data::setFather(Data* father){
	this->father = father;
}

void Data::setFrequency(unsigned long freq){	
	this->freq = freq;
}

void Data::setCharacter(unsigned char byte){	
	this->byte = byte;
}

bool Data::operator >(Data* j){
	return this->getFrequency() > j->getFrequency();
	}

Data* Data::getLeft() const{
	return left;
}

Data* Data::getRight() const{
	return right;
}

Data* Data::getFather() const{
	return father;
}

