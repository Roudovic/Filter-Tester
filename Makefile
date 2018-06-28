all : Test2

CC = g++

CFLAGS = -W -Wall -ansi -pedantic -fno-strict-aliasing -fpermissive
LDFLAGS = 
EXEC = Test2
SRC = BloomFilter.cpp BloomFilter_1.cpp Hasher.cpp MurmurHash3.cpp Test2.cpp 
OBJ = $(SRC:.c=.o) 

$(EXEC) : $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 

cuckoofilter.o : cuckoofilter.h
	$(CC) -o $@ -c cuckoofilter.h $(CFLAGS)
	
clean : 
	rm -rf *.o
mrproper: clean
	rm -rf $(EXEC)
	
