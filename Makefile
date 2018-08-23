all :  Test3

CC = g++

CFLAGS =  -W -Wall -ansi -pedantic -fno-strict-aliasing -fpermissive -std=c++14 -DFARMHASH_DEBUG=0
LDFLAGS =  -O3 -fno-strict-aliasing -Wall -lpthread -lssl -lcrypto  -g -std=c++17  -march=native -lcityhash -lfarmhash -DFARMHASH_DEBUG=0

EXEC = Test3
SRC = BloomFilter.cpp Hasher.cpp MurmurHash3.cpp  Test3.cpp BloomFilter_1.cpp gcs/gcs.cpp gcs/md5.c BloomFilter_Dash.cpp
OBJ = $(SRC:.c=.o) 

$(EXEC) : $(OBJ) 
	$(CC) -o $@ $^ $(LDFLAGS) -Icuckoofilter-master/src -IMortonFilter/MortonFilter -Ifarmhash-master/src -Igcs/
	
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 


	
clean : 
	rm -rf *.o $(EXEC)
mrproper: clean
	rm -rf $(EXEC)
	
