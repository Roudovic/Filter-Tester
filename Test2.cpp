#include "Test2.h"
#include <cstdint>
#include <iostream>
#include "BloomFilter_1.h"
#include "BloomFilter.h"
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <chrono>
#include <thread>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include "bloom_filter.hpp"

#include "cuckoofilter.h"
#include "simd-block.h"





#define __STDC_WANT_LIB_EXT1__ 1

#define FP 0.0001




Test2::Test2()
{
    //ctor
}

Test2::~Test2()
{
    //dtor
}


void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

int main(int argc, char* argv[]){



const uint16_t size_input = 40;
const uint16_t max_size_input = 100;

const uint32_t n = 128;
const uint32_t un_sur_tx_pos = 600;



uint64_t m = floor(-1.44*n*log2(FP));
uint8_t k = floor(log(2)*m/n);
printf("k=%d\n",k);
std::cout<<m<<std::endl;


const uint8_t fingerprint_size = floor(log2(8/FP));


char s[max_size_input] = " ";
char testingvalues[(un_sur_tx_pos)*n][max_size_input];
int integer_testingvalues[(un_sur_tx_pos)*n];

std::size_t l = size_input;

BloomFilter_1 bloom = BloomFilter_1(m,k);
BloomFilter Sbloom = BloomFilter(m, k);

cuckoofilter::CuckooFilter<int,fingerprint_size> filter(n);


bloom_parameters parameters;

parameters.projected_element_count = n;
parameters.false_positive_probability = FP; 

parameters.compute_optimal_parameters();
bloom_filter newbloom(parameters);



SimdBlockFilter<> simdbloom(ceil(log2(n * 8.0 / CHAR_BIT)));

for(unsigned int i = 0; i<n; i++){
   // memset(data1,0,max_size_input);
	gen_random(s,size_input);
	strcpy(testingvalues[i],s);
    srand(i);
    integer_testingvalues[i]=rand();

    //std::cout << s << std::endl;
  //  bloom1.add((const uint8_t*)s, l);
    bloom.add((const uint8_t*)s, l);
	Sbloom.add((const uint8_t*)s, l);
    srand(i);
    filter.Add(integer_testingvalues[i]);
    newbloom.insert(integer_testingvalues[i]);
    //simdbloom.Add((uint64_t)i);


}
std::cout<< "Filters are done" << std::endl;
for (unsigned int i=n; i<(un_sur_tx_pos)*n; i++){
    gen_random(s,size_input);
    strcpy(testingvalues[i],s );
    srand(i);
    integer_testingvalues[i]=rand();
}

std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
int fp = 0;
for(unsigned int i = 0; i<(un_sur_tx_pos)*n; i++){
   // std::cout<<
	bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]));
	if (i >n && bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]))==1) {
		fp++;}
		//std::cout << testingvalues[i] << std::endl;
	
    //<<std::endl ;
}
std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
std::chrono::duration<double> diff = t2-t1;
std::cout<<"it took "<<diff.count()<<" s "<<"for the Bloom-1 "<<std::endl;
std::cout << "Its false positive rate is " << (double)fp / (un_sur_tx_pos* n) << std::endl;

t1 = std::chrono::system_clock::now();
fp = 0;
for (unsigned int i = 0; i<(un_sur_tx_pos) * n; i++) {
	//std::cout<<
	Sbloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]));
	if (i > n && Sbloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i])) == 1) { fp++; }
		//<<std::endl ;
}
 t2 = std::chrono::system_clock::now();
 diff = t2 - t1;
std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
std::cout << "Its false positive rate is " << (double)fp /(un_sur_tx_pos* n) << std::endl;


std::cout<<std::endl<<"Entries are integers"<<std::endl;

t1 = std::chrono::system_clock::now();
fp = 0;
for (unsigned int i = 0; i<(un_sur_tx_pos) * n; i++) {
	srand(i);
    //std::cout<<
    
	filter.Contain(integer_testingvalues[i]);//<<std::endl;
	if (i>n && filter.Contain(integer_testingvalues[i])==0)  { fp++;  }
		//<<std::endl ;
}
 t2 = std::chrono::system_clock::now();
 diff = t2 - t1;
std::cout << "it took " << diff.count() << " s " << "for the Cuckoo Filter " <<std::endl;
std::cout << "Its false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
std::cout<<"Cuckoo Filter Size : " << filter.SizeInBytes()*8<<std::endl;
std::cout << "Fingerprint size : "<< floor(log2(8/FP)) << std::endl;
std::cout<<filter.Info()<<std::endl;

t1 = std::chrono::system_clock::now();
fp = 0;
for (unsigned int i = 0; i<(un_sur_tx_pos) * n; i++) {
	srand(i);
    //std::cout<<
	newbloom.contains(integer_testingvalues[i]);//<<std::endl;
	if  (i>n && newbloom.contains(integer_testingvalues[i]))  { fp++;  }
		//<<std::endl ;
}
 t2 = std::chrono::system_clock::now();
 diff = t2 - t1;
std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
std::cout << "Its false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
std::cout <<"It uses "<< parameters.optimal_parameters.number_of_hashes << " hash functions "<< std::endl;
std::cout << "Bloom filter size : " << parameters.optimal_parameters.table_size << std::endl;


}
