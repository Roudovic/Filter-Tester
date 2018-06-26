all : Test

CC = g++

CFLAGS = -W -Wall -ansi -pedantic
LDFLAGS = 
EXEC = TestFilters
SRC = BloomFilter.cpp BloomFilter_1.cpp Hasher.cpp MurmurHash3.cpp Test.cpp
OBJ = $(SRC:.c=.o)

Test : $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 
	
clean : 
	rm -rf *.o
	
