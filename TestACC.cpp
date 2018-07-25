//
//  TestACC.cpp
//  
//
//  Created by Ludovic Theobald on 23/7/2561 BE.
//

#include "TestACC.hpp"
#include "bloom_filter.hpp"
#include <cstdint>
#include <iostream>
#include <chrono>
#include <ctime>
#include <stdlib.h>

#define FP 0.0001



const uint32_t n =250;
const uint32_t un_sur_tx_pos = 200;


int integer_testingvalues[(un_sur_tx_pos)*n];


int main(){

bloom_parameters parameters;

parameters.projected_element_count = n;
parameters.false_positive_probability = FP;

parameters.compute_optimal_parameters();
bloom_filter newbloom(parameters);
    
    
    
    for (unsigned int i = 0; i<n ; i++){
        srand(i);
        integer_testingvalues[i]=rand();
        
        newbloom.insert(integer_testingvalues[i]);
    }
    for (uint64_t i=n; i<(un_sur_tx_pos)*n; i++){
        
        srand(i);
        integer_testingvalues[i]=rand();
    
    }
    
    auto t1 = std::chrono::system_clock::now();
    int fp = 0;
    for (uint64_t i = 0
         ; i<(un_sur_tx_pos) * n; i++) {
        
                newbloom.contains(integer_testingvalues[i]);
        if  (i>=n && newbloom.contains(integer_testingvalues[i]))  { fp++;  }
            }
    auto t2 = std::chrono::system_clock::now();
    auto diff = t2 - t1;
     std::cout << "it took " << diff.count() << " s " << "for the Bloom Filter " <<std::endl;
    std::cout << "Standard Bloom false positive rate is " << (double)fp / (un_sur_tx_pos*n) << std::endl;
        std::cout <<"It uses "<< parameters.optimal_parameters.number_of_hashes << " hash functions "<< std::endl;
        std::cout << "Bloom filter size : " << parameters.optimal_parameters.table_size << std::endl;
    

    
    
    
}
