#include "Test.h"
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


#include "cuckoofilter.h"





#define __STDC_WANT_LIB_EXT1__ 1

#define FP 0.005




Test::Test()
{
    //ctor
}

Test::~Test()
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

const uint16_t n = 1000;



uint64_t m = floor(-1.44*n*log2(FP));
uint8_t k = floor(log(2)*m/n);
printf("k=%d\n",k);
std::cout<<m<<std::endl;





char s[max_size_input] = " ";
char testingvalues[2*n][max_size_input];
std::size_t l = size_input;

BloomFilter_1 bloom = BloomFilter_1(m,k);
BloomFilter Sbloom = BloomFilter(m, k);
cuckoofilter::CuckooFilter<size_t,12> filter(m);


for(unsigned int i = 0; i<n; i++){
   // memset(data1,0,max_size_input);
	gen_random(s,size_input);
	strcpy(testingvalues[i],s);

    //std::cout << s << std::endl;
  //  bloom1.add((const uint8_t*)s, l);
    bloom.add((const uint8_t*)s, l);
	Sbloom.add((const uint8_t*)s, l);
    filter.Add(i);
   std::cout<< filter.Contain(i)<<std::endl;
   std::cout<< filter.Contain(i+1)<<std::endl;

}
std::cout<< "done" << std::endl;
for (unsigned int i=n; i<2*n; i++){
    gen_random(s,size_input);
    strcpy(testingvalues[i],s );
}
for(unsigned int i = 0; i<2*n; i++){
   // std::cout<<testingvalues[i]<<std::endl ;
}
std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
int fp = 0;
for(unsigned int i = 0; i<2*n; i++){
   // std::cout<<
	bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]));
	if (i > n && bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]))==1) {
		fp++;
		//std::cout << testingvalues[i] << std::endl;
	}
    //<<std::endl ;
}
std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
std::chrono::duration<double> diff = t2-t1;
std::cout<<"it took "<<diff.count()<<" s "<<"for the Bloom-1 "<<std::endl;
std::cout << "Its false positive rate is " << (double)fp / n << std::endl;

t1 = std::chrono::system_clock::now();
fp = 0;
for (unsigned int i = 0; i<2 * n; i++) {
	//std::cout<<
	Sbloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]));
	if (i > n && Sbloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i])) == 1) { fp++; }
		//<<std::endl ;
}
 t2 = std::chrono::system_clock::now();
 diff = t2 - t1;
std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
std::cout << "Its false positive rate is " << (double)fp / n << std::endl;


t1 = std::chrono::system_clock::now();
fp = 0;
for (unsigned int i = 0; i<2 * n; i++) {
	std::cout<<
	filter.Contain(i)<<std::endl;
	if (i > n && filter.Contain(i)==0)  { fp++; }
		//<<std::endl ;
}
 t2 = std::chrono::system_clock::now();
 diff = t2 - t1;
std::cout << "it took " << diff.count() << " s " << "for the Cuckoo Filter " <<std::endl;
std::cout << "Its false positive rate is " << (double)fp / n << std::endl;

//std::cout<<bloom.possiblyContains((const uint8_t*)data1, l)<<std::endl ;




}
