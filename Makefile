all : Test2

CC = g++

CFLAGS = -Ofast -W -Wall -ansi -pedantic -fno-strict-aliasing -fpermissive -std=gnu++14 -mpclmul -msse4.2

LDFLAGS =  -Ofast -fno-strict-aliasing -Wall -lpthread -lssl -lcrypto -march=core-avx2 -g -std=gnu++14  -mpclmul -msse4.2

EXEC = Test2
SRC = BloomFilter.cpp Hasher.cpp MurmurHash3.cpp Test2.cpp clhash.c
OBJ = $(SRC:.c=.o) 

$(EXEC) : $(OBJ) 
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src -IMortonFilter/MortonFilter
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 


	
clean : 
	rm -rf *.o $(EXEC)
mrproper: clean
	rm -rf $(EXEC)
	
