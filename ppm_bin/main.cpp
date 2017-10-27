#include <iostream>
#include <cstdio>
#include <string.h>
#include <fstream> 	
#include <cmath>
#include <time.h>
#include "ArithmeticCoderC.h"


const int g_Signature = 0x434D4341;

#define SIZE 5
unsigned int C, N;
unsigned int* contexts_index = NULL;
unsigned int* occurence = NULL;
unsigned int size_occurence = 0;
unsigned int K = 25;
unsigned int mTotal; 
ArithmeticCoderC aritmetico;

double entropia = 0.0;


inline unsigned int min(unsigned int x, unsigned int y)
{
	if(x < y)
		return x;
	else
		return y;
}

void criar_Modelo()
{
	contexts_index = (unsigned int*) malloc((K+1) * sizeof(unsigned int));

	if((contexts_index == NULL))
	{
		std::cout << "Error\n";
	}

	for (unsigned int i = 0; i < K+1; i++)
	{
		contexts_index[i] = (unsigned int)((1 << (i+1)) - 2);	
	}

	size_occurence = ((1 << (K+2)) - 2);
	occurence = (unsigned int*) malloc(size_occurence * sizeof(unsigned int));

	for (unsigned int i = 0; i < size_occurence; i++)
	{
		occurence[i] = 1L;
	}
	mTotal = size_occurence + 1;
}

unsigned int calcular_indice()
{
	int context_current = min(K, N);

	unsigned int mask = (1 << context_current) - 1;
	unsigned int index = contexts_index[context_current] + ((C & mask) << 1);
	
	return index;
}


void consultar_modelo(unsigned int tab[3])
{
	unsigned int index = calcular_indice();
	tab[0] = 0;
	tab[1] = occurence[index];
	tab[2] = occurence[index] + occurence[index+1];
}

void atualizar_modelo(unsigned int simbolo)
{
	unsigned int indice = calcular_indice();
	occurence[indice+simbolo]++;
}

void atualizar_contexto(unsigned int simbolo)
{
	C = (C << 1) | simbolo;
	N = min(N+1, K);
}

void comprimir(unsigned int simbolo)
{
	unsigned int tab[3];
	unsigned int low, high, total;
	consultar_modelo(tab);
	
	low = tab[simbolo];
	high = tab[simbolo+1];
	total = tab[2];
	
	//entropia
		entropia += (-log10((double)(high-low)/total));
	//entropia

	aritmetico.Encode(low, high, total);
	atualizar_modelo(simbolo);
	atualizar_contexto(simbolo);
}

void descomprimir(fstream *dTarget, unsigned int q_simbolos)
{
	unsigned int tab[3];
	unsigned int low, high, total;
	unsigned int simbolo = 0;
	unsigned int var = 0;
	unsigned int var_count = 0;

	do
	{
		consultar_modelo(tab);

		unsigned int valor = aritmetico.DecodeTarget(tab[2]);//aritmetico.contador_atual(tab[2]);
		if(valor >= tab[1])
		{
			simbolo = 1;
		}
		else
		{
			simbolo = 0;
		}
	
		var = var << 1;
		var += simbolo;
		var_count++;
		//std::cout << "var: " << var << "\tvar_count: " << var_count << std::endl;

		if(var_count >= 8)
		{
			var_count = 0;
			//std::cout << "" << (char)var;
			dTarget->write(reinterpret_cast<char*>(&var), sizeof(unsigned char));
			var = 0;
			q_simbolos--;
		}

		low = tab[simbolo];
		high = tab[simbolo+1];
		total = tab[2];

		aritmetico.Decode(low, high);
		atualizar_modelo(simbolo);
		atualizar_contexto(simbolo);
		mTotal++;
	}
	while(q_simbolos > 0);

}

unsigned int quantidade_de_simbolos(fstream *source)
{
	source->seekg(0, ios::end);
	return source->tellg();
}

/////////calculos da entropia e comprimento médio e RC
double RC(unsigned int source, fstream *target)
{		
	target->seekg(0, ios::end);

	unsigned int size_source = source;
	unsigned int size_target = target->tellg();

	double rc = ((double)size_source/size_target);
	return rc;
	//std::cout << "ss " << size_source << "\tst " << size_target << std::endl;
	//std::cout << "Razão de compressão: " << ((double)size_source/size_target) << std::endl;
}

double comprimento_medio(unsigned int source, fstream *target)
{	
	target->seekg(0, ios::end);

	unsigned int size_source = source;
	unsigned int size_target = target->tellg();

	double l = ((double)(size_target*8)/size_source);
	return l;
	//std::cout << "ss " << size_source << "\tst " << size_target << std::endl;
	//std::cout << "Comprimento médio: " << ((double)(size_target*8)/size_source) << " Bits/simbolo" << std::endl;
}

double entropy(unsigned int source)
{
	unsigned int size_source = source;

	double h = (entropia/log10(2.0))/size_source;
	return h;
	//std::cout << "Entropia: " << (entropia/log10(2.0))/size_source << " Bits/simbolo" << std::endl;
}
/////////calculos da entropia e comprimento médio e RC

int main (int argc, char** argv)
{
	/*unsigned int C, N;
	unsigned int* contexts_index = NULL;
	unsigned int* occurence = NULL;
	unsigned int size_occurence = 0L;
	unsigned int K = 0; 
	*/

	unsigned int signature = 0;

	clock_t time;
	double diff_time;

	unsigned int bits[8];

	if(argc == 3)
	{
		criar_Modelo();

		fstream source(argv[1], ios::in | ios::binary);
		fstream target(argv[2], ios::out | ios::binary);
		if(!source.is_open())
		{
			cout << "erro no arquivo de entrada";
			return 2;
		}
		if(!target.is_open())
		{
			cout << "erro no arquivo de saida";
			return 3;
		}

		signature = 0;
    	source.read(reinterpret_cast<char*>(&signature),sizeof(signature));
    	if( signature == g_Signature )
	    {
	    	unsigned int q_simbolos = 0;
	        cout << "Decodificar " << argv[1] << " para " << argv[2] << endl;
	        source.read(reinterpret_cast<char*>(&q_simbolos),sizeof(q_simbolos)); // quantidade de simbolos a decoficar
	        aritmetico.SetFile(&source);
	        aritmetico.DecodeStart();
	        time = clock(); // INICIO DO TEMPO
	       	descomprimir(&target, q_simbolos);
	       	diff_time = clock() - time; //FIM DO TEMPO
	       	std::cout << "Tempo de descompressão: " << ((double)diff_time)/CLOCKS_PER_SEC << " segundos" <<std::endl;
	    }
	    else
	    {
	        cout << "Codificar " << argv[1] << " para " << argv[2] << endl;
	        aritmetico.SetFile(&target);
	        source.seekg(0, ios::beg);
	        target.write(reinterpret_cast<const char*>(&g_Signature), sizeof(g_Signature));
	        unsigned int q_simbolos = quantidade_de_simbolos(&source);//quantidade de simbolos a decodificar para o cabeçalho...
	        target.write(reinterpret_cast<const char*>(&q_simbolos), sizeof(q_simbolos));

	        source.seekg(0, ios::beg);
	        time = clock(); //INICIO DO TEMPO
	        while(!source.eof())
	        {
				unsigned char data;
				source.read(reinterpret_cast<char*>(&data), sizeof(data));
				
				if(!source.eof())
				{
					for(int i = 0; i < 8; i++)
					{
						bits[i] = ((data & (128 >> i))? 1 : 0);
					
						//std::cout << "Bit: "<< bits[i] << "\n";
						comprimir(bits[i]);
						mTotal++;
					}	
				}	 
	    	}

	    	unsigned int tab[3];
	    	consultar_modelo(tab);
	    	aritmetico.Encode(tab[2]-1, tab[2], tab[2]);
	    	//aritmetico.Encode(mTotal-1, mTotal, mTotal);
	    	aritmetico.EncodeFinish();
	    	diff_time = clock() - time; //FIM DO TEMPO

	    	std::cout << "Comprimento médio: " << comprimento_medio(q_simbolos, &target) << " Bits/simbolo" << std::endl;
	    	std::cout << "Entropia: " <<  entropy(q_simbolos) << " Bits/simbolo" << std::endl;
			std::cout << "Razão de compressão: " << RC(q_simbolos, &target) << std::endl;
			std::cout << "Tempo de compressão: " << ((double)diff_time)/CLOCKS_PER_SEC << " segundos" << std::endl;
		}

		source.seekg(0, ios::end);
		target.seekg(0, ios::end);
		source.close();
		target.close();
	}
	else
	{
		std::cout << "argumentos invalidos" << std::endl;
		std::cout << "Para comprimir e descomprimir: ./main arquivo.ext arquivo.ext" << std::endl;	
	}

	return 0;
}
