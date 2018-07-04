all : Test2

CC = g++

CFLAGS = -W -Wall -ansi -pedantic -fno-strict-aliasing -fpermissive -std=c++11
LDFLAGS =  -Wall -lpthread -lssl -lcrypto -march=core-avx2 -g -std=c++11
EXEC = Test2
SRC = BloomFilter.cpp Hasher.cpp MurmurHash3.cpp Test2.cpp 
OBJ = $(SRC:.c=.o) 

$(EXEC) : $(OBJ) 
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 
	
	
clean : 
	rm -rf *.o
mrproper: clean
	rm -rf $(EXEC)
	
