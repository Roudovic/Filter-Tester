
//
//  main.cpp
//  MortonFilter
//
//  Created by Ludovic Theobald on 6/7/2561 BE.
//  Copyright © 2561 Ludovic Theobald. All rights reserved.
//

#define NB_ENTRIES 1000


#include <iostream>
#include "MortonFilter.hpp"

MortonFilter mrtn = MortonFilter(NB_ENTRIES);


int testingvalues[300*NB_ENTRIES];
int second_bucket_items[NB_ENTRIES];




int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    char test1[10] = "Hellworld";
//    char test2[10] = "HelloYou";
//    char test3[10] = "CoucouToi";
    

    
    for (int i =0; i<NB_ENTRIES ; i++){
        testingvalues[i]=rand();
        mrtn.Add(testingvalues+i,sizeof(int));
        std::cout <<
        mrtn.Contains(testingvalues+i,sizeof(int))
        << std::endl;
        if(i%50==0)std::cout<<i<<std::endl;
        
    }
    for (int i =NB_ENTRIES; i<300*NB_ENTRIES ; i++){
        testingvalues[i]=rand();
       
    }
    mrtn.Add(test1, 10);
    
    int size =0;
    int* size_ptr = &size;
    for (int i =0; i<300*NB_ENTRIES ; i++){
        if(i%100 == 0) std::cout << i << std::endl;
        std::cout <<
        mrtn.Contains1(testingvalues + i, sizeof(int), second_bucket_items )
        << std::endl;
        
        

    }
    int i = 0;
    while(second_bucket_items[i++] > 0){
        bool containMorton = mrtn.ContainsElse(&testingvalues[i],sizeof(int));
        std::cout << i << std::endl;//
        
    }
    
//    for (int i =0; i<NB_ENTRIES ; i++){
//
//        std::cout << mrtn.Contains(testingvalues+i,sizeof(int))  << std::endl;
//        if(i%50==0)std::cout<<i<<std::endl;
////        if(testingvalues[i]==0) break;
//    }

    
    

    std::cout << mrtn.SizeInBits() <<std::endl;
    std::cout << sizeof(struct MortonFilter::Block)*8 << std::endl;
    
//    uint64_t murtest1;
//    uint64_t murtest2;
//    MurmurHash3_x64_128(test1, 10, 0, &murtest1);
//    MurmurHash3_x64_128(test2, 10, 0, &murtest2);
//    std::cout << (&murtest1)[0]<<(&murtest1)[1]<< std::endl;
//    std::cout << (&murtest2)[0]<<(&murtest2)[1]<< std::endl;
    
    return 0;
}
