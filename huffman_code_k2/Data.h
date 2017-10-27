class Data{

private:
	unsigned char byte;
	unsigned long freq;
	Data* left;
	Data* right;
	Data* father;

public:
	Data(unsigned char byte, unsigned long freq);
	~Data(){
		
	}
	unsigned char getCharacter() const;
	unsigned long getFrequency() const;
	void setCharacter(unsigned char byte);
	void setFrequency(unsigned long freq);
	Data* getLeft() const;
	Data* getRight() const;
	Data* getFather() const;
	void setLeft(Data* left);
	void setRight(Data* right);
	void setFather(Data* father);
	
	bool operator >(Data* j);
};
