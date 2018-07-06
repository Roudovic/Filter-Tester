//
//  MortonFilter.hpp
//  MortonFilter
//
//  Created by Ludovic Theobald on 6/7/2561 BE.
//  Copyright © 2561 Ludovic Theobald. All rights reserved.
//

#ifndef MortonFilter_hpp
#define MortonFilter_hpp

#include <stdio.h>
#include <vector>

#endif /* MortonFilter_hpp */

class MortonFilter{
    size_t OTA_size, FCA_size, FSA_size;
    uint32_t nb_blocks;
    struct Block {
        unsigned OTA : 16;
        unsigned FCA : 128;
        unsigned FSA : 368;
    };typedef Block Block;
    
    
    int Add(char* data);
    
};
