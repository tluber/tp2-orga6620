#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define waysAmount 8      
#define setsAmount 32 
#define blockSize 32    
#define MemoryBlocks 1024    

typedef struct
{
	int addresses[blockSize];
} MemoryBlock;

typedef struct
{
	int addresses[blockSize];
	int tag;
	int valid;
	int order;

} CacheBlock;

CacheBlock cache[waysAmount][setsAmount];

MemoryBlock memory[MemoryBlocks];

int hits = 0;

int misses = 0;

void init()
{
	int max = 65536;
	
	for (int a = 0; a<waysAmount; a++)
	{
		for (int b =0; b < setsAmount; b++)
		{
			CacheBlock cb;
			
			for (int c=0; c < blockSize; c++)
			{
				cb.addresses[c] = 0;
			}
			cb.tag = 0;
			cb.valid=0;
			cb.order=0;
			cache[a][b] = cb;
		}
	}
	for (int d = 0; d<MemoryBlocks; d++)
		{
			MemoryBlock nb;
			int d;
			for (int e =0; e < blockSize; e++)
			{
				nb.addresses[e] = rand()% max;
			}
			memory[d] = nb;
		}
	}


unsigned int find_set(unsigned int address)
{
	return address/ blockSize % setsAmount;
}

unsigned int get_offset(unsigned int address)
{
	return address % blockSize;
}

unsigned int select_oldest(unsigned int set)
{
	int max = 0;
	int way = 0;
	int y;
	for (int x = 0; x < waysAmount; x++)
	{
		if (cache[x][set].valid == 0)
			{
				return x;
			}
		else
		{
			y = cache[x][set].order;
			if (max < y)
			{
				max = y;
				way = x;
			}
		}

	}
	return way;
}

void read_tocache(unsigned int block, unsigned int way, unsigned int set)
{
	for (int i = 0; i <blockSize; i++)
	{
		cache[way][set].addresses[i] = memory[block].addresses[i];
	}
	cache[way][set].tag = block;
	cache[way][set].valid = 1;
	cache[way][set].order = 0;
}

signed int findTag(unsigned int tag, unsigned int set)
{
	int aux = -1;
	for (int i = 0; i< waysAmount; i++)
	{
		if (cache[i][set].valid == 1 && cache[i][set].tag == tag)
		{
			aux = i;
			break;
		}
	}
	return aux;
}

unsigned char read_byte(unsigned int address)
{
	int offset = get_offset(address);
	int set = find_set(address);
	int tag = address / (blockSize*setsAmount);
	
	if (findTag(tag, set) == -1)
	{
		for (int i=0; i < waysAmount; i++)
		{
			if (cache[i][set].valid == 1)
			{
				cache[i][set].order++;
			}
		}
		read_tocache(tag, select_oldest(set), set);
		misses++;
	}
	else
	{
		hits++;
	}
	int way = findTag(tag, set);
	return cache[way][set].addresses[offset];
}

void write_byte(unsigned int address, unsigned char data)
{
	int offset = get_offset(address);
	int set = find_set(address);
	int tag = address / (blockSize*setsAmount);
	
	memory[tag].addresses[offset] = data;
	if (findTag(tag, set) == -1)
	{
		misses++;
	}
	else
	{
		hits++;
		int way = findTag(tag, set);
		cache[way][set].addresses[offset] = data;
	}
}

float get_miss_rate()
{
	float MR = 0;
	int cant = hits + misses;
	if (cant > 0)
	{
		MR =(float)misses/(float)cant;
	}
	return MR;
}

void borrar(char *palabra,char caracter)
{
	int counter=0;
	for(int i=0; palabra[i];i++)
	{
		palabra[i]=palabra[i+counter];
		if(palabra[i]==caracter)
		{
			i--;
			counter++;
		}
	}
}

void parser(char linea[256])
{
	if(strstr(linea, "FLUSH ") != NULL)
	{
		init();
	}
	if(strstr(linea, "MR") != NULL)
	{
		printf("El Miss rate fue del: %f \n", get_miss_rate());
	}
	if(strstr(linea, "W ") != NULL)
	{
		
		char *cadena = strtok(linea, " " );
		cadena = strtok(NULL, " " );
		char *cadena2 = strtok(NULL, "," );
		borrar(cadena, ','); 
        borrar(cadena, ' '); 
        borrar(cadena, '\n'); 
        int address = strtol(cadena, (char **)NULL, 10);
        int data = strtol(cadena2, (char **)NULL, 10);
		write_byte(address, data);
		printf("se escribio en la direccion: %d el dato %d \n", address, data);
	}
	if(strstr(linea, "R ") != NULL)
	{
		char * cadena = strtok(linea, " " );
		cadena = strtok(NULL, " " );
        borrar(cadena, '\n');
        int address = strtol(cadena, (char **)NULL, 10);
        read_byte(address);
        printf("se leyo la direccion: %d \n", address); 
	}
}

int main(int argc, char* argv[])
{
	init();
	char const* const archivo = argv[1];
    FILE* prueba = fopen(archivo, "r");
    char linea[256];

    while (fgets(linea, sizeof(linea), prueba)) {
        parser(linea);
    }

    fclose(prueba);
    return 0;
}