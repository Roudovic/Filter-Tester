#include "Test3.hpp"
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
#include <fstream>
#include "MortonFilter.hpp"

#include "cuckoofilter.h"
#include "cuckoofilter-master/src/simd-block.h"





#define __STDC_WANT_LIB_EXT1__ 1

#define FP 0.0001






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

const uint32_t size_input = 20 ;
const uint32_t max_size_input = 1000;

const uint32_t n = 200;
const uint32_t un_sur_tx_pos = 1000;
const int nb_tests = 10;

int second_buckets_items[n];

uint64_t m = floor(-1.44*n*log2(FP));
uint8_t k = floor(log(2)*m/n);
//printf("k=%d\n",k);
//std::cout<< "filter size is : " <<m<<std::endl;
double temps_moyens[6];
double fp_means[6];
int filter_sizes[6];
int tours = 0;

char s[max_size_input] = " ";
char testingvalues[(un_sur_tx_pos)*n][max_size_input];
int integer_testingvalues[(un_sur_tx_pos)*n];

int main(){
    
    
    
    
    
    
    while(tours < nb_tests){
        
        
        
        
        std::size_t l = size_input;
        
        BloomFilter_1 bloom = BloomFilter_1(m,k);
        BloomFilter Sbloom = BloomFilter(m, k);
        
        //Cuckoo filter
        const uint8_t fingerprint_size = floor(log2(8/FP));
        cuckoofilter::CuckooFilter<char[max_size_input],16> filter(n);
        
        
        //Other Bloom Filter implementation with optimally chosen parameters
        //And ability to insert integers
        
        bloom_parameters parameters;
        
        parameters.projected_element_count = n;
        parameters.false_positive_probability = FP;
        
        parameters.compute_optimal_parameters();
        bloom_filter newbloom(parameters);
        
        // SIMD Based Blocked Bloom Filter -- Close to the Ultra-fast Bloom filter implementation
        
        SimdBlockFilter<> simdbloom(ceil(log2( 2 * n * 8.0 / CHAR_BIT)));
        
        //SimdBlockFilter<> simdbloom(100);
        
        //    Morton Filter, not optimized
        
        MortonFilter mrtn = MortonFilter(n);
        
        
        for(uint64_t i = 0; i<n; i++){
            
            gen_random(s,size_input);
            strcpy(testingvalues[i],s);
            
            srand(i);
            integer_testingvalues[i]=rand();
            
            bloom.add((const uint8_t*)s, l);
            Sbloom.add((const uint8_t*)s, l);
            
            mrtn.Add(testingvalues[i],l);
            
            
            newbloom.insert(integer_testingvalues[i]);
            simdbloom.Add(testingvalues[i], l);
            filter.Add(testingvalues[i],l);
        }
        // std::cout<< "Filters are done" << std::endl;
        for (uint64_t i=n; i<(un_sur_tx_pos)*n; i++){
            gen_random(s,size_input);
            strcpy(testingvalues[i],s );
            srand(i);
            integer_testingvalues[i]=rand();
            
            
        }
        bool contains = false;
        
        std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
        int fp = 0;
        contains = false;
         for(uint64_t i = 0; i<(un_sur_tx_pos)*n; i++){
         // std::cout<<
         contains = bloom.possiblyContains((const uint8_t*)testingvalues[i], strlen(testingvalues[i]));
         if (i >n && contains) {
         fp++;}
         //std::cout << testingvalues[i] << std::endl;
         
         //<<std::endl ;
         }
        
        std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = t2-t1;
        // std::cout<<"it took "<<diff.count()<<" s "<<"for the Bloom-1 "<<std::endl;
        // std::cout << "Its false positive rate is " << (double)fp / (un_sur_tx_pos* n) << std::endl;
        temps_moyens[0]+=diff.count();
        fp_means[0]+=(double)fp / (un_sur_tx_pos* n);
        
        t1 = std::chrono::system_clock::now();
        fp = 0;
                for (uint64_t i = 0; i<(un_sur_tx_pos) * n; i++) {
                    contains = false;
                    //std::cout<<
                    contains = Sbloom.possiblyContains((const uint8_t*)testingvalues[i], l);
                    if (i >=n && contains) { fp++; }
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
        
        for (uint64_t i = 0
             ; i<(un_sur_tx_pos) * n; i++) {
            contains = false;
            //std::cout<<
            contains = newbloom.contains(testingvalues[i], l);//<<std::endl;
            if  (i>=n && contains)  { fp++;  }
            //<<std::endl ;
        }
        t2 = std::chrono::system_clock::now();
        diff = t2 - t1;
        // std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
        if(tours ==1){std::cout << "Standard Bloom false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
            std::cout <<"It uses "<< parameters.optimal_parameters.number_of_hashes << " hash functions "<< std::endl;
            std::cout << "Bloom filter size : " << parameters.optimal_parameters.table_size << std::endl;
            filter_sizes[3]=parameters.optimal_parameters.table_size;
            
        }
        temps_moyens[3]+=diff.count();
        fp_means[3]+=(double)fp / (un_sur_tx_pos* n);
        
        
        //              SIMD BLOCK BLOOM FILTER
        
        t1 = std::chrono::system_clock::now();
        fp = 0;
        for (uint64_t i = 0; i<(un_sur_tx_pos) * n; i++) {
            contains = false;
            //std::cout<<
            contains = simdbloom.Find(testingvalues[i], l);//<<std::endl;
            if(i<n && !contains){std::cout << "faux negatif SIMD" << std::endl;}
            if  (i>=n && contains)  { fp++;  }
            //<<std::endl ;
        }
        t2 = std::chrono::system_clock::now();
        diff = t2 - t1;
        // std::cout<<std::endl << "it took " << diff.count() << " s " << "for the SIMD Bloom Filter " <<std::endl;
        if(tours ==1){std::cout << "SIMD false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
            std::cout <<"SIMD Block Bloom filter size : "<<simdbloom.SizeInBytes()*8<<std::endl;
            filter_sizes[4]=simdbloom.SizeInBytes()*8;
        }
        temps_moyens[4]+=diff.count();
        fp_means[4]+=(double)fp / (un_sur_tx_pos* n);
        
        t1 = std::chrono::system_clock::now();
        fp = 0;
        
        for (uint64_t i = 0; i<(un_sur_tx_pos) * n; i++) {
            
            //std::cout<<
            
            contains = filter.Contain(testingvalues[i], l);//<<std::endl;
            if(i<n && contains != 0){std::cout << "faux negatif cuckoo" << std::endl;}
            if (i>=n && contains==0)  { fp++;  }
            //<<std::endl ;
        }
        t2 = std::chrono::system_clock::now();
        diff = t2 - t1;
        // std::cout << "it took " << diff.count() << " s " << "for the Cuckoo Filter " <<std::endl;
        if(tours ==1) {std::cout << "Cuckoo false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
            std::cout<<"Cuckoo Filter Size : " << filter.SizeInBytes()*8<<std::endl;
            filter_sizes[2]=filter.SizeInBytes()*8;
        }
        // std::cout << "Fingerprint size : "<< floor(log2(8/FP)) << std::endl;
        if(tours ==1) std::cout<<filter.Info()<<std::endl;
        temps_moyens[2]+=diff.count();
        fp_means[2]+=(double)fp / (un_sur_tx_pos* n);
        
        
        //    MORTON FILTER
        
        
        bool containMorton;
        
        t1 = std::chrono::system_clock::now();
        fp = 0;
        
        
        for (uint64_t i = 0; i<(un_sur_tx_pos) * n; i++) {
            containMorton = mrtn.Contains1(testingvalues[i],l, second_buckets_items);//
//            if(i<n && !containMorton){std::cout << "faux negatif Morton" << std::endl;}
            if(i>=n && containMorton ) { fp++;  }
        }
        int i = 0;
//        while(second_buckets_items[i++] > 0){
//            containMorton = mrtn.ContainsElse(testingvalues[i],l);
//            std::cout << i << std::endl;//
//            if(i>=n && containMorton ) { fp++;  }
//        }
        //        std::cout<<size_list<<std::endl;
        
        
        
        t2 = std::chrono::system_clock::now();
        diff = t2 - t1;
        
        if(tours ==1){std::cout << "Morton Filter false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
            std::cout << "Morton filter size : " << mrtn.SizeInBytes() << std::endl;
            //filter_sizes[3]=parameters.optimal_parameters.table_size;
            
        }
        temps_moyens[5]+=diff.count();
        fp_means[5]+=(double)fp / (un_sur_tx_pos* n);
        
        //
        
        tours++;
        
    }
    
    std::array<std::string,6> names = {{ "Bloom-1 :", " Standard Bloom", "Cuckoo", "Another Bloom", "SIMD", "Morton" }};
    for(int i =0; i<6; i++){
        std::cout << std::endl;
        temps_moyens[i]=temps_moyens[i]/nb_tests;
        fp_means[i]=fp_means[i]/nb_tests;
        std::cout << names[i] << std::endl;
        std::cout <<"temps : " <<temps_moyens[i]<<std::endl;
        std::cout <<" FP Rate : " <<  fp_means[i] << std::endl;
    }
    
    
    //
    //    // LOGGING VALUES INTO TEXT FILE
    //    std::ofstream bloom_log("bloom_log.txt", std::ios::app);
    //
    //    bloom_log << n <<std::endl;
    //
    //    bloom_log << "Cuckoo \n";
    //    bloom_log << filter_sizes[2] << std::endl;
    //    bloom_log << temps_moyens[2] << std::endl;
    //    bloom_log << fp_means[2] << std::endl;
    //
    //    bloom_log << "Bloom \n";
    //    bloom_log << filter_sizes[3] << std::endl;
    //    bloom_log << temps_moyens[3] << std::endl;
    //    bloom_log << fp_means[3] << std::endl;
    //
    //    bloom_log << "SIMD Bloom \n";
    //    bloom_log << filter_sizes[4] << std::endl;
    //    bloom_log << temps_moyens[4] << std::endl;
    //    bloom_log << fp_means[4] << "\n"<< std::endl;
    //
    //
}

