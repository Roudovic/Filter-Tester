#include "Test2.h"
#include <cstdint>
#include <iostream>

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

int main(){



const uint16_t size_input = 40;
const uint16_t max_size_input = 100;

    const uint32_t n = 10;
const uint32_t un_sur_tx_pos = 6000;
const int nb_tests = 100;


uint64_t m = floor(-1.44*n*log2(FP));
uint8_t k = floor(log(2)*m/n);
//printf("k=%d\n",k);
//std::cout<< "filter size is : " <<m<<std::endl;
double temps_moyens[5];
double fp_means[5];
int tours = 0;
    
    
while(tours < nb_tests){

char s[max_size_input] = " ";
char testingvalues[(un_sur_tx_pos)*n][max_size_input];
int integer_testingvalues[(un_sur_tx_pos)*n];


std::size_t l = size_input;

//BloomFilter_1 bloom = BloomFilter_1(m,k);
BloomFilter Sbloom = BloomFilter(m, k); 

//Cuckoo filter
const uint8_t fingerprint_size = floor(log2(8/FP));
cuckoofilter::CuckooFilter<int,12> filter(n);


//Other Bloom Filter implementation with optimally chosen parameters
//And ability to insert integers

bloom_parameters parameters;

parameters.projected_element_count = n;
parameters.false_positive_probability = FP; 

parameters.compute_optimal_parameters();
bloom_filter newbloom(parameters);

// SIMD Based Blocked Bloom Filter -- Close to the Ultra-fast Bloom filter implementation

SimdBlockFilter<> simdbloom(ceil(log2( 2*n * 8.0 / CHAR_BIT)));



for(unsigned int i = 0; i<n; i++){
    
	gen_random(s,size_input);
	strcpy(testingvalues[i],s);
    
    srand(i);
    integer_testingvalues[i]=rand();

    //bloom.add((const uint8_t*)s, l);
	Sbloom.add((const uint8_t*)s, l);
    
    filter.Add(integer_testingvalues[i]);
    newbloom.insert(integer_testingvalues[i]);
    simdbloom.Add(integer_testingvalues[i]);


}
// std::cout<< "Filters are done" << std::endl;
for (unsigned int i=n; i<(un_sur_tx_pos)*n; i++){
    gen_random(s,size_input);
    strcpy(testingvalues[i],s );
    srand(i);
    integer_testingvalues[i]=rand();
}

std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
int fp = 0;
    /*
for(unsigned int i = 0; i<(un_sur_tx_pos)*n; i++){
   // std::cout<<
	bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]));
	if (i >n && bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]))==1) {
		fp++;}
		//std::cout << testingvalues[i] << std::endl;
	
    //<<std::endl ;
}
     */
 std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
 std::chrono::duration<double> diff = t2-t1;
// std::cout<<"it took "<<diff.count()<<" s "<<"for the Bloom-1 "<<std::endl;
// std::cout << "Its false positive rate is " << (double)fp / (un_sur_tx_pos* n) << std::endl;
temps_moyens[0]+=diff.count();
    fp_means[0]+=(double)fp / (un_sur_tx_pos* n);

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
// std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
// std::cout << "Its false positive rate is " << (double)fp /(un_sur_tx_pos* n) << std::endl;
temps_moyens[1]+=diff.count();
fp_means[1]+=(double)fp / (un_sur_tx_pos* n);

// std::cout<<std::endl<<"Entries are integers"<<std::endl;

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
// std::cout << "it took " << diff.count() << " s " << "for the Cuckoo Filter " <<std::endl;
    if(tours ==1) {std::cout << "Cuckoo false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
        std::cout<<"Cuckoo Filter Size : " << filter.SizeInBytes()*8<<std::endl;}
// std::cout << "Fingerprint size : "<< floor(log2(8/FP)) << std::endl;
if(tours ==1) std::cout<<filter.Info()<<std::endl;
temps_moyens[2]+=diff.count();
fp_means[2]+=(double)fp / (un_sur_tx_pos* n);

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
// std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
    if(tours ==1){std::cout << "Standard Bloom false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
 std::cout <<"It uses "<< parameters.optimal_parameters.number_of_hashes << " hash functions "<< std::endl;
        std::cout << "Bloom filter size : " << parameters.optimal_parameters.table_size << std::endl;}
temps_moyens[3]+=diff.count();
fp_means[3]+=(double)fp / (un_sur_tx_pos* n);
//              SIMD BLOCK BLOOM FILTER 

t1 = std::chrono::system_clock::now();
fp = 0;
for (unsigned int i = 0; i<(un_sur_tx_pos) * n; i++) {
	srand(i);
    //std::cout<<
	simdbloom.Find(integer_testingvalues[i]);//<<std::endl;
	if  (i>n && simdbloom.Find(integer_testingvalues[i]))  { fp++;  }
		//<<std::endl ;
}
 t2 = std::chrono::system_clock::now();
 diff = t2 - t1;
// std::cout<<std::endl << "it took " << diff.count() << " s " << "for the SIMD Bloom Filter " <<std::endl;
    if(tours ==1){std::cout << "SIMD false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
        std::cout <<"SIMD Block Bloom filter size : "<<simdbloom.SizeInBytes()*8<<std::endl;}
temps_moyens[4]+=diff.count();
fp_means[4]+=(double)fp / (un_sur_tx_pos* n);
tours++;
}
for(int i =2; i<5; i++){
     std::cout << std::endl;
    temps_moyens[i]=temps_moyens[i]/nb_tests;
    fp_means[i]=fp_means[i]/nb_tests;
    std::cout <<"temps : " <<temps_moyens[i]<<std::endl;
    std::cout <<" FP Rate : " <<  fp_means[i] << std::endl;
}



}
