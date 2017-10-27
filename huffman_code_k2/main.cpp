#define BUFFER_SIZE 102000
#include <iostream>
#include <cstdio>
#include <queue>
#include <cmath>
#include "Data.h"
#include <string.h>
#include <time.h>
//#include <math.h>

//#define DEBUGDEC
//#define DEBUG

struct comparatorLong{
	bool operator ()(Data* i, Data* j){
		return i->getFrequency() > j->getFrequency();
	}
};

unsigned long header_bytes = 0;


/************** FUNCTIONS *******************/
void readFile(const char*  filename, unsigned long freq_num[256][256], unsigned long * qtde);
Data* AddNode(const unsigned char byte, unsigned long freq, Data* left, Data* right, Data* father);
void printTree(Data *root);
void printCode(Data* root, std::string code, char c);
Data* createHuffmanTree(std::priority_queue<Data* , std::vector<Data*>, comparatorLong>* heap);
/************* MAIN *************************/ 

inline Data* AddNode(const unsigned char byte, unsigned long freq, Data* left, Data* right, Data* father){
	Data* new_node = new Data(byte, freq);
	new_node->setLeft(left);
	new_node->setRight(right);
	new_node->setFather(father);
	return new_node;
}

void printHuffmanTree(Data * treeHuffman, int qtdEspacos, bool simpleExibition){
	if(treeHuffman != nullptr){
		std::string deslocamento = "";
		if(qtdEspacos != 0)
			deslocamento += "   ";			
		for(int i = 0; i < qtdEspacos; i++){
			if(i == qtdEspacos-1){
				deslocamento += "|__";
			}
			else
				deslocamento += "      ";
		}
		std::string byte = "";
		byte += (char)treeHuffman->getCharacter();
		if(byte == "\n"){
			byte = "\\n";
		}
		if(simpleExibition)
			std::cout << deslocamento << "(" << byte << ", " << treeHuffman->getFrequency() << ")" << std::endl;
		else
			std::cout  << deslocamento << "Nivel = " << qtdEspacos << "\n" << deslocamento << "parent = " << treeHuffman->getFather() <<"\n" << deslocamento << "(" << byte << ", " << treeHuffman->getFrequency() << ") = "<< treeHuffman << "\n"  << deslocamento << "left = " << treeHuffman->getLeft() << "right = " << treeHuffman->getRight() << std::endl;
		printHuffmanTree(treeHuffman->getLeft(), qtdEspacos+1, simpleExibition);
		//getchar();
		printHuffmanTree(treeHuffman->getRight(),qtdEspacos+1, simpleExibition);
	}
}

void printTree(Data *root){
	if(root == nullptr){
		return;
	}
	else{
		printTree(root->getLeft());
		if(root->getLeft() == nullptr && root->getRight() == nullptr)
			printf("\tP:%p \n\t ME:%p %lu %c\nL:%p \tR:%p\n\n", (void*)root->getFather(), (void*)root , root->getFrequency(), root->getCharacter(),(void*)root->getLeft(), (void*)root->getRight());
		printTree(root->getRight());
	}
}

void printCode(Data* root, std::string code, char c){
	if (c != ' ')
		code.push_back(c);
	if(root == nullptr){
		return;
	}
	else{
		printCode(root->getLeft(), code, '0');
		printCode(root->getRight(), code, '1');
		if(root->getLeft() == nullptr && root->getRight() == nullptr){
			std::cout << root->getCharacter() << " : " << code << std::endl;
		}
	}
}
inline void numBin(int numero){
    int i;
    int result=numero;
    int bin[8];
    for(i=0;i<8;i++)
        bin[i]=0;
    for(i=0;result!=0;i++){
        bin[i]=result%2;
        result=(result - bin[i])/2;
    }
    #ifdef DEBUG
    printf("Binario: ");
    for(i=7;i>=0;i--)
        printf("%d", bin[i]);
    #endif
}

Data* createHuffmanTree(std::priority_queue<Data* , std::vector<Data*>, comparatorLong>* heap){
	Data* left_node = nullptr;
	Data* right_node = nullptr;
	Data* parent_node = nullptr;
	
	while(!heap->empty()){
		left_node = heap->top();
		heap->pop();
		
		if(heap->empty()){
			return nullptr;
		}
		right_node = heap->top();
		heap->pop();
		
		parent_node = AddNode('*', left_node->getFrequency() + right_node->getFrequency(), left_node, right_node, nullptr);
	
		left_node->setFather(parent_node);
		right_node->setFather(parent_node);
		if(heap->empty()){
			break;
		}
		heap->push(parent_node);
	}
	return parent_node;
}

void generateCode(Data* root,std::string codes[256], std::string code, char c){
	
	if(c != ' '){
		code.push_back(c);
	}
	if(root == nullptr){
		return;
	}
	else{
		if(root->getLeft() == nullptr && root->getRight() == nullptr){
			unsigned char temp_r = root->getCharacter();
			codes[temp_r] = code;
			
			return;
		}
		else{
			generateCode(root->getLeft(), codes, code,  '0');
			generateCode(root->getRight(), codes, code, '1');
			return;
		}
	}
}

inline void printBuffer(unsigned char buffer[BUFFER_SIZE], unsigned int size){
	printf("Buffer: ");
	for(unsigned int i = 0; i < size; i++){
		printf("([%d] %x), ", i, (unsigned char)(buffer)[i]);
	}
	printf("\n");
}

unsigned int readHeader(unsigned long freq_num[256][256], const char* filename_out, unsigned long* qtde){
	
	unsigned char context = 0;
	unsigned char word = 0;
	FILE* to_read = fopen(filename_out, "rb");
	unsigned int count = 0;
	unsigned int bytes_tojump = 4;
	
	for(unsigned int i = 0; i < 256; i++){
		for (unsigned int j = 0; j< 256; j++){
			freq_num[i][j] = 0;
		}
	}
	
	#ifdef DEBUG
	printf("Reading header!\n");
	#endif
	if(!fread(&count, sizeof(unsigned int), 1, to_read)){ // jump 4 bytes
		printf("Error qtde\n");
		return 0;
	}
	#ifdef DEBUG
	printf("Read size: %d\n", count);
	#endif
	
	(*qtde) = 1;
	for(unsigned int i = 0; i < count; i++){
		
		if(!fread(&context, sizeof(unsigned char), 1, to_read)){ // jump 1 byte
			printf("Error (1)!\n");
			return 0;	
		}
		if(!fread(&word, sizeof(unsigned char), 1, to_read)){ // jump 1 byte
			printf("Error (2)!\n");
			return 0;	
		}
		if(!fread(&freq_num[context][word], sizeof(unsigned long), 1, to_read)){ // jump 8 bytes
			printf("Error (3)!\n");
			return 0;	
		}
		(*qtde) += freq_num[context][word];
		bytes_tojump += 10;
		#ifdef DEBUG
		printf("Read in file, Context{%c}||Word{%c}:%lu\n", context, word, freq_num[context][word]);
		#endif
		
	}
	fclose(to_read);
	return bytes_tojump;
}

void writeHeader(unsigned long freq_num[256][256], const char* filename_out, const unsigned long qtde){
	#ifdef DEBUG
	printf("Writing header!\n");
	#endif

	FILE* to_write = fopen(filename_out, "wb");
	unsigned int count = 0; // not bigger than 65365
	
	
	for(unsigned int i = 0; i< 256; i++){
		for (unsigned int j = 0; j < 256; j++){
			if(freq_num[i][j] > 0)
				count++;
		}
	}
	fwrite(&count, sizeof(unsigned int), 1, to_write);

	// global variable this shouldn't be right... :/
	header_bytes = count;
	
	#ifdef DEBUG
	printf("Size: %d\n", count);
	#endif
	
	for(unsigned int i = 0; i < 256; i++){
		for (unsigned int j = 0; j< 256; j++){
			if(freq_num[i][j] > 0){
				#ifdef DEBUG
				printf("Writing in file, Context{%c}||Word{%c}:%lu\n", i, j, freq_num[i][j]);
				#endif
				fwrite(&i, sizeof(unsigned char), 1, to_write);
				fwrite(&j, sizeof(unsigned char), 1, to_write);
				fwrite(&freq_num[i][j], sizeof(unsigned long), 1, to_write);
			}
		} 
	}
	fclose(to_write);
}



inline void ASCIItoBin(unsigned char c, char strOutput[8]){
	int bit, byte;

	for (bit = 0, byte = 7; bit < 8; bit++, byte--) {
		if ((c >> bit) & 0x01){
			strOutput[byte] = '1';
		}
		else{
			strOutput[byte] = '0';
		}
	}
}

inline void addBuffer(unsigned char buffer[BUFFER_SIZE], unsigned int* pos, unsigned char c, FILE* f_pointer, const char * filename, bool force_write){
	if((*pos) >= BUFFER_SIZE){
		f_pointer = fopen(filename, "a+b");
		if(fwrite((buffer), sizeof(char), (*pos), f_pointer) != (*pos)){
			printf("Error!");
		}
		else{
			(*pos) = 0;
			(buffer)[(*pos)] = c;
			(*pos)++;
		}
		fclose(f_pointer);
	}
	else{
		(buffer)[(*pos)] = c;
		(*pos)++;
		#ifdef DEBUG
		printBuffer(buffer, (*pos));
		#endif
		if(force_write){
			f_pointer = fopen(filename, "a+b");
			if(fwrite((buffer), sizeof(char), (*pos), f_pointer) != (*pos)){
				printf("Error!");
			}
			else{
				(*pos) = 0;
			}
			fclose(f_pointer);
		}
	}	
}

void codifyFile(std::string code_num[256][256], unsigned long qtde, const char * filename_in, const char * filename_out){
	unsigned char buffer_toread[BUFFER_SIZE];
	
	unsigned char buffer_towrite[BUFFER_SIZE];
	unsigned int buffer_towritepos = 0;
	FILE* to_write;
	
	unsigned int bit_count = 0;
	unsigned char x = 0;

	unsigned int fread_return;
	unsigned int contexto = 300;
	
	std::FILE* file_pointer = fopen(filename_in, "rb");
	
	for(unsigned long i = qtde; i > 0; i-=fread_return){
		fread_return = std::fread(buffer_toread, sizeof(char), BUFFER_SIZE, file_pointer);
		
		for(unsigned int j = 0; j < fread_return; j++) {
			if(contexto == 300){
				#ifdef DEBUG
				printf("First char(not converting): %c\n", buffer_toread[j]);
				#endif
				contexto = buffer_toread[j];				
				addBuffer(buffer_towrite, &buffer_towritepos, buffer_toread[j], to_write, filename_out, false);
								
				#ifdef DEBUG
				printBuffer(buffer_towrite, buffer_towritepos);
				#endif
			}
			else{
				std::string codigo = code_num[(unsigned char)contexto][buffer_toread[j]];
				#ifdef DEBUG
				printf("\nConverting...Code:'");
				std::cout << codigo;
				printf("'...Context:'%c", contexto);
				if(buffer_toread[j] != '\n')
					printf("'...Word:'%c'\n", buffer_toread[j]);
				else
					printf("'...Word:'\\n'\n");
				#endif
				contexto = buffer_toread[j];
				
				
				for(unsigned int u = 0; u < codigo.size(); u++){
					unsigned char y = codigo[u];
					x = x << 1;
					y = y - 48;
					x = x | y;
					bit_count++;
					#ifdef DEBUG
					printf("\tBC: %d >> ", bit_count);
					numBin((int)x);
					printf("\n");
					#endif
					
					if(bit_count == 8){
						#ifdef DEBUG
						printf("\t\tConverting: %d->", bit_count);
						numBin((int)x);
						printf("\n");
						#endif

						addBuffer(buffer_towrite, &buffer_towritepos, x, to_write, filename_out, false);
						bit_count = 0;
						x = 0;
					}
				}
				#ifdef DEBUG
				printf("End convertion\n");
				#endif
			}
		}
	}
	
	if(bit_count != 0)
	{
		#ifdef DEBUG
		printf("Padding: ");
		#endif
		for(unsigned int i = bit_count; i < 8; i++){
			x = x << 1;
		}
		addBuffer(buffer_towrite, &buffer_towritepos, x, to_write, filename_out, false);
		x = 0;
		#ifdef DEBUG
		printf("%d times.\n", (8-bit_count));
		#endif
		
		
	}
	to_write = fopen(filename_out, "a+b");	
	if(fwrite(buffer_towrite, sizeof(char), buffer_towritepos, to_write) != buffer_towritepos){
		// error
		printf("Error! In write file.\n");
		fclose(to_write);
	}
	else{
		fclose(to_write);
	}	
	fclose(file_pointer);

}

void decodifyFile(Data* const root[256], unsigned long freq_num[256][256], unsigned long qtde, unsigned int bytes_tojump, const char* filename_in, const char* filename_out){
	FILE* to_read = nullptr;
	FILE* to_write = nullptr;
	unsigned int fread_return = 0, pos_towrite = 0, context = 300;
	unsigned char buffer_toread[BUFFER_SIZE];
	unsigned char buffer_towrite[BUFFER_SIZE];
	Data* p_root = nullptr;
	to_read = fopen(filename_in, "rb");
	fseek (to_read, bytes_tojump , SEEK_SET);

	to_write = fopen(filename_out, "wb");
	fclose(to_write);
		
	for(unsigned long i = 0; i < qtde;){
		fread_return = std::fread(buffer_toread, sizeof(char), BUFFER_SIZE, to_read);
		for (unsigned int r = 0; r < fread_return; r++){
			if(context == 300){
				context = buffer_toread[r];
				p_root = root[context];
				addBuffer(buffer_towrite, &pos_towrite, context , to_write, filename_out, false);
				#ifdef DEBUGDEC
				printf("First char(not converting): %c\n", buffer_toread[r]);
				#endif
				i++;
			}
			else{
				char bin[8];
				ASCIItoBin(buffer_toread[r], bin);
				
				for(int k = 0; k < 8 && i < qtde;){
					if(p_root == nullptr){
						// sem arvore prob. 1
						bool problem = true;
						for(unsigned int find = 0; find < 256; find++){
							if(freq_num[context][find] > 0){
								problem = false;
								addBuffer(buffer_towrite, &pos_towrite, (unsigned char)find , to_write, filename_out, false);
								#ifdef DEBUG
								printf("\n\tSave(Notree)::Context: %c Char: %c --- %d'", (unsigned char) context, (unsigned char)find, k);
								for(int loop = 0; loop < 8; loop++){
									printf("%c", (unsigned char)bin[loop]);
								}
								printf("'\n");
								#endif
								i++;
								context = find;
								p_root = root[context];
								break;
							}
						}
						if(problem){
							// error cant find symbol
							printf("ERROR!Finding value without tree:: Context: %c\n",context);
							return;
						}
					}
					else{
						if(p_root != nullptr && bin[k] == '0'){
							#ifdef DEBUG
							printf("0");
							#endif
							p_root = p_root->getLeft();
							k++;
						}
						else{
							if(p_root != nullptr){
								#ifdef DEBUG
								printf("1");
								#endif
								p_root = p_root->getRight();
								k++;
							}
						}
						if(p_root->getLeft() == nullptr && p_root->getRight() == nullptr){
							addBuffer(buffer_towrite, &pos_towrite, (unsigned char)p_root->getCharacter() , to_write, filename_out, false);
							i++;
							#ifdef DEBUG
							printf("\nSave(Tree)::(%lu)Context: %c Char: %c --- %d'", i,(unsigned char) context, p_root->getCharacter(), k);
							for(int loop = 0; loop < 8; loop++){
								if(loop == k)
									printf("-");
								printf("%c", (unsigned char)bin[loop]);
								
							}
							printf("'\n");
							#endif
							context = p_root->getCharacter();
							p_root = root[context];
							
							
							
						}
					}
				}
			}
		}
	}	
	fclose(to_read);
	if(pos_towrite != 0){
		to_write = fopen(filename_out, "a+b");	
		if(fwrite(buffer_towrite, sizeof(char), pos_towrite, to_write) != pos_towrite){
			// error
			printf("Error! In write file.\n");
			fclose(to_write);
		}
		else{
			fclose(to_write);
		}
	}
}

double comprimento(const char* filename, unsigned long qtde){
	FILE* file_pointer = fopen(filename, "rb");
	header_bytes = header_bytes * 10 + 4;
	fseek(file_pointer, 0, SEEK_END);
	
	unsigned long b_compressed = ftell(file_pointer) - header_bytes;
	fclose(file_pointer);
	
	return (8 * ((double)b_compressed/(double)qtde));
}

double entropy(unsigned long freq[256][256], unsigned long qtde)
{   
	/*
        a b c d e f
	a   1 2 5 8 0 1	
	b   2 3 5 4 1 2
	c   8 9 7 8 4 1
	*/
	unsigned long total[256];
	double entropy = 0.0;
	
	for(unsigned int i = 0; i < 256; i++){
		total[i] = 0;
	}
	
	for(unsigned int context = 0; context< 256; context++){
		for(unsigned int character = 0; character < 256; character++){
			total[context] += freq[context][character];
		}
	}
	
	for(int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			if(freq[i][j] > 0 && total[i] > 0)
			{
				double l_local = ((double)freq[i][j]/(double)qtde) * (log(1.0/((double)freq[i][j]/(double)total[i]))/log(2.0));
				entropy += l_local;
			}
		}
	}
	return entropy;
}

int main(int argc, char * argv[]){

	unsigned long freq_num[256][256], qtde = 0L;
	std::priority_queue <Data* , std::vector<Data*>, comparatorLong> minHeap[256];
	Data *root[256];

	clock_t time;
	double diff_time;
	
	if(argc == 4 && !strcmp(argv[2], "-c"))
	{	
		time = clock();
		// Coding
		printf("Reading file...");
		readFile(argv[1], freq_num, &qtde); 
		printf("Done.\n");
		
		#ifdef DEBUG
		for(unsigned int i = 0; i < 256; i++){
			for(unsigned int j=0; j < 256; j++){
				if(freq_num[i][j] > 0){
					printf("%c %c %lu\n", i, j, freq_num[i][j]);
				}
			}
		}
		#endif
		
		for(unsigned int i = 0; i < 256; i++){
			for(unsigned int j=0; j < 256; j++){
				if(freq_num[i][j] > 0){
					minHeap[i].push(new Data((unsigned char) j, freq_num[i][j]));
				}
			}
		}
		printf("Creating huffmantree...");
		for(unsigned int i = 0; i < 256; i++){
			
			if(!minHeap[i].empty()){
				root[i] = createHuffmanTree(&(minHeap[i]));	
			}
			
			else{
				root[i] = nullptr;
			}
		}
		
		printf("Done.\n");
		printf("Generating code...");
		std::string code_num[256][256];
		for(unsigned int i = 0; i < 256; i++){
			generateCode(root[i], code_num[i], "", ' ');
		}
		printf("Done.\n");
		
		printf("Writing header...");
		writeHeader(freq_num, argv[3], qtde);
		printf("Done.\n");
		
		printf("Writing compressed file...");
		codifyFile(code_num, qtde, argv[1], argv[3]); 
		printf("Done.\n");
		diff_time = clock() - time;
		printf("Time compression: %f seconds\n", ((double)diff_time)/CLOCKS_PER_SEC);
		
		double length_ = comprimento(argv[3], qtde);
		double entropy_ = entropy(freq_num, qtde);
		
		printf("Entropia: %f\n", entropy_);
		printf("Comprimento Medio: %f\n", length_);
		printf("RC: %f\n", length_/entropy_);
		
	}
	else if(argc == 4 && !strcmp(argv[2], "-x"))
	{
		// Decoding
		time = clock();
		printf("Reading header....");
		unsigned int bytes_tojump = readHeader(freq_num, argv[1], &qtde);
		printf("Done.\n");

		#ifdef DEBUG
		printf("Qtde: %lu\n", qtde);
		printf("Bytestojump: %d\n", bytes_tojump);
		#endif

		for(unsigned int i = 0; i < 256; i++){
			for(unsigned int j=0; j < 256; j++){
				if(freq_num[i][j] > 0){
					minHeap[i].push(new Data((unsigned char) j, freq_num[i][j]));
				}
			}
		}
		printf("Creating HuffmanTree...");	
		for(unsigned int i = 0; i < 256; i++){
			
			if(!minHeap[i].empty()){
				root[i] = createHuffmanTree(&minHeap[i]);	
			}
			else{
				root[i] = nullptr;
			}
		}
		printf("Done.\n");
		printf("Decoding file...");
		decodifyFile(root, freq_num, qtde, bytes_tojump, argv[1], argv[3]);
		printf("Done.\n");
		diff_time = clock() - time;
		printf("Time descompression: %f seconds\n", ((double)diff_time)/CLOCKS_PER_SEC);
	}
	else{
		printf("Is not possible to use this options.\n");
	}
	return 0;
}

void readFile(const char*  filename, unsigned long freq_num[256][256], unsigned long * qtde){
	unsigned int read_bytes = 0;
	*qtde = 1;
	for(unsigned int i = 0; i < 256; i++){
		for(unsigned int j = 0; j < 256; j++){
			freq_num[i][j] = 0;
		}
	}
	bool jump_first = true;
	unsigned char last_char = 0;
	unsigned char buffer[BUFFER_SIZE];
	
	std::FILE *file_pointer = std::fopen(&filename[0], "rb");
	
	if(file_pointer == nullptr){
		std::cout << "File not find! getArrayFrequency()" << std::endl;
		return;
	}

	while(!feof(file_pointer)){
		if(file_pointer){
			read_bytes = std::fread(buffer, sizeof(char), BUFFER_SIZE, file_pointer);
		}	
		else{
			// error
			printf("File not found!");
		}
		for(unsigned int i = 0; i < read_bytes; i++){
			if(jump_first == true){
				jump_first = false;
				last_char = buffer[i];	
				
			}
			else{	
				freq_num[last_char] [(unsigned int )buffer[i]] += 1; 
				last_char = buffer[i];
				(*qtde) += 1;
			}
		}
	}
	std::fclose(file_pointer);
	return;
}

