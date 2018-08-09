all : Test2

CC = g++

CFLAGS = -Ofast -W -Wall -ansi -pedantic -fno-strict-aliasing -fpermissive -std=c++17 
LDFLAGS =  -Ofast -fno-strict-aliasing -Wall -lpthread -lssl -lcrypto  -g -std=c++17  -march=core-avx2 -lcityhash


EXEC = Test2
SRC = BloomFilter.cpp Hasher.cpp MurmurHash3.cpp Test2.cpp 
OBJ = $(SRC:.c=.o) 

$(EXEC) : $(OBJ) 
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src -IMortonFilter/MortonFilter
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 


	
clean : 
	rm -rf *.o $(EXEC)
mrproper: clean
	rm -rf $(EXEC)
	
