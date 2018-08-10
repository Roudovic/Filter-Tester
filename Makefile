all :  Test3

CC = g++

CFLAGS =  -W -Wall -ansi -pedantic -fno-strict-aliasing -fpermissive -std=c++17 
LDFLAGS =  -Ofast -fno-strict-aliasing -Wall -lpthread -lssl -lcrypto  -g -std=c++17  -march=core-avx2 -lcityhash -lfarmhash


EXEC = Test3
SRC = BloomFilter.cpp Hasher.cpp MurmurHash3.cpp  Test3.cpp BloomFilter_1.cpp
OBJ = $(SRC:.c=.o) 

$(EXEC) : $(OBJ) 
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src -IMortonFilter/MortonFilter -Ifarmhash-master/src
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 


	
clean : 
	rm -rf *.o $(EXEC)
mrproper: clean
	rm -rf $(EXEC)
	
