//
//  MortonFilter.hpp
//  MortonFilter
//
//  Created by Ludovic Theobald on 6/7/2561 BE.
//  Copyright © 2561 Ludovic Theobald. All rights reserved.
//

#ifndef MortonFilter_hpp
#define MortonFilter_hpp

#include <algorithm>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include "MurmurHash3.h"
#include "hashutil.h"

#endif /* MortonFilter_hpp */
#define LEN 10
#define OFF_RANGE 128

class MortonFilter{
    // Public here for tests purposes, TO CHANGE
    public :
    
    uint32_t nb_blocks;
    typedef struct Block {
        uint16_t OTA;
        uint64_t FCA[2];
        uint8_t FSA[46];
        
    }__attribute__((packed))Block;
    
    //    /!\ changed *filter to filter[]
    
    Block *filter;
    
    cuckoofilter::TwoIndependentMultiplyShift hasher;
    
    int Add(void* __restrict data, int len);
    
    bool table_simple_store(Block *block, uint8_t lbi, uint8_t Fx);
    
    void set_OTA(Block *block, uint8_t lbi, uint64_t murhash);
    
    bool Contains(void* __restrict data, int len);
    
    bool Contains1(void* __restrict data, int len, int* second_bucket_items, int *size_list);
    
    bool ContainsElse(void* __restrict data, int len);
    
    bool table_read_and_cmp(Block *block, uint8_t lbi, uint8_t Fx);
    
    bool OTA_bit_is_unset(Block *block, uint8_t lbi, uint64_t murhash);
    
    int res_conflict(Block* filter, Block* block1, Block* block2, uint32_t glbi1, uint32_t glbi2, uint8_t hf, uint64_t murhash );
    
    
    uint16_t find_fsa_slot(Block* block, uint8_t lbi );
    
    uint16_t find_fsa_slot_noffset(Block *block, uint8_t lbi);
    
    int SizeInBytes();
    
    // TODO : Create a method to get the real FCA[i] without using bits arithmetic everytime
    // like block.getFCAindex(i)
    
    explicit MortonFilter(int nb_items_expected) {
        nb_blocks = (1.3 * nb_items_expected / 46) + 2 ;
        size_t size_block = sizeof(struct Block);
        //filter = (Block*)malloc(size_block * nb_blocks);
        filter = new Block[nb_blocks];
        memset(filter, 0, size_block*nb_blocks);
    }
};

int MortonFilter::SizeInBytes(){
    return nb_blocks*sizeof(struct Block)*8;
    
}

inline uint32_t map(uint64_t x, uint32_t n){
    return ((__uint128_t)(x) * (__uint128_t)n)>>64;
}

inline uint32_t map2(int x, uint32_t n){
    if(x>=0 && x<=n-1) return x;
    else if (x<0) return x+n;
    else return x-n;
}

inline uint8_t offset(uint8_t Fx){
    return  (64 + (Fx & (OFF_RANGE-1))) | 1;
}

inline uint16_t MortonFilter::find_fsa_slot(MortonFilter::Block *block, uint8_t lbi){
    __uint128_t one_128 = 1;
    __uint128_t fullnessCounterArrayMask = (one_128 << (2 * lbi)) - 1;
    //    __uint128_t FCA128 = (__uint128_t)block->FCA;
    uint64_t mFCA[2];
    mFCA[0]=block->FCA[0]&(fullnessCounterArrayMask>>64);
    mFCA[1]=block->FCA[1]&fullnessCounterArrayMask;
    //    __uint128_t mFCA = (__uint128_t)block->FCA & fullnessCounterArrayMask;
    uint64_t pcMask = 0x5555555555555555;
    //    pcMask=(pcMask<<64) + 0x5555555555555555;
    uint8_t sum = 0;
    for ( int bitPos = 0; bitPos < 2; bitPos++){
        uint64_t partialAnd[2];
        partialAnd[0] = mFCA[0] & pcMask;
        partialAnd[1]=mFCA[1] & pcMask;
        sum+=__builtin_popcountll((uint64_t)(partialAnd[0]))<<bitPos;
        sum+=__builtin_popcountll((uint64_t)(partialAnd[1]))<<bitPos;
        pcMask<<=1; //Verify if the shift actually occurs
    }
    uint64_t off;
    if (lbi < 32){
        off = block->FCA[1]&((1ull<<(2*lbi)) | (1ull<<(2 * lbi +1)));
        off >>=(2*lbi) ;
    }
    else{
        off = block->FCA[0] & ((1ull<<(2*(lbi-32))) | (1ull<<(2 * (lbi-32) + 1)) );
        off>>=(2 * (lbi-32));
    }
    return 45 - (sum + off);
    
}


inline uint16_t MortonFilter::find_fsa_slot_noffset(MortonFilter::Block *block, uint8_t lbi){
    __uint128_t one_128 = 1;
    __uint128_t fullnessCounterArrayMask = (one_128 << (2 * lbi)) - 1;
    //    __uint128_t FCA128 = (__uint128_t)block->FCA;
    uint64_t mFCA[2];
    mFCA[0]=block->FCA[0]&(fullnessCounterArrayMask>>64);
    mFCA[1]=block->FCA[1]&fullnessCounterArrayMask;
    //    __uint128_t mFCA = (__uint128_t)block->FCA & fullnessCounterArrayMask;
    uint64_t pcMask = 0x5555555555555555;
    //    pcMask=(pcMask<<64) + 0x5555555555555555;
    uint8_t sum = 0;
    for ( int bitPos = 0; bitPos < 2; bitPos++){
        uint64_t partialAnd[2];
        partialAnd[0] = mFCA[0] & pcMask;
        partialAnd[1]=mFCA[1] & pcMask;
        sum+=__builtin_popcountll((uint64_t)(partialAnd[0]))<<bitPos;
        sum+=__builtin_popcountll((uint64_t)(partialAnd[1]))<<bitPos;
        pcMask<<=1; //Verify if the shift actually occurs
    }
    
    return 45 - (sum);
    
}

inline bool MortonFilter::table_simple_store(Block *block, uint8_t lbi, uint8_t Fx){
    // Checking FSA to be non-full
    // TODO : Take the case where Fx == 0 at the first position, with the FCA
    if(block->FSA[0] == 0){
        uint8_t fsa_slot = find_fsa_slot(block, lbi);
        // FCA Updating
        uint64_t mask0 = (1ull<<(2*lbi)) | (1ull<<(2 * lbi +1));
        uint64_t mask1 = (1ull<<(2*(lbi-32))) | (1ull<<(2 * (lbi-32) + 1)); //makes sense only if lbi > 32
        if ( lbi < 32 && ((block->FCA[1] & mask0) >> (2*lbi)) < 3 ){
            block->FCA[1] += 1ull<<(2 * lbi);
            //            uint8_t fsa_slot = find_fsa_slot(block, lbi);
            for(int i = 1 ; i<=fsa_slot; i++){
                block->FSA[i-1]=block->FSA[i];
            }
            block->FSA[fsa_slot]=Fx;
            return true;
        }
        if ( lbi > 31 && ((block->FCA[0] & mask1)>>(2*(lbi-32))) < 3){
            block->FCA[0] += 1ull<<(2 * (lbi-32));
            //            uint8_t fsa_slot = find_fsa_slot(block,lbi);
            for(int i = 1 ; i <= fsa_slot; i++){
                block->FSA[i-1]=block->FSA[i];
            }
            block->FSA[fsa_slot]=Fx;
            return true;
        }
        
        return false;
    }
    // Little endian implementation...
    
    
    return false;
}



inline void MortonFilter::set_OTA(Block *block, uint8_t lbi, uint64_t murhash){
    
    block->OTA |= block->OTA ^(1<< (map(murhash,16)));
    
}

inline bool MortonFilter::OTA_bit_is_unset(Block *block, uint8_t lbi, uint64_t murhash){
    return (block->OTA ^(1<< (map(murhash,16)))) == 0;
}


inline int MortonFilter::Add(void* __restrict data, int len){
    uint64_t murhash0;
    MurmurHash3_x64_128( data, len, 0, &murhash0);
    int nb_buckets = 46 * nb_blocks;
    uint32_t glbi1 = map(murhash0, nb_buckets);
    uint32_t glbi2;
    Block *block2;
    //    printf("MurmurHash Value : %llx \n", murhash0);
    //    printf("Global block index : %x \n", glbi1 );
    
    uint8_t hf = ((&murhash0)[1]>>56);
    if(hf==0) hf++;
    //    printf("Hf : %x\n", hf);
    Block *block1 = &filter[glbi1/64];
    uint8_t lbi1 = glbi1 % 64;
    if(table_simple_store(block1, lbi1, hf)){
        //        std::cout << "Hash 1 is used "<< std::endl;
        return 1;
    }
    else{
        //        std::cout << "Hash 2 is used "<< std::endl;
        set_OTA(block1, lbi1, murhash0);
        glbi2 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(hf), nb_buckets);
        block2 = &filter[glbi2/64];
        uint8_t lbi2 = glbi2 % 64;
        if (table_simple_store(block2, lbi2, hf)){
            return 2;
        }
    }
    
    return 0;
    
    return res_conflict(filter, block1, block2, glbi1, glbi2, hf, murhash0);
    
    
    
    
    
}

int MortonFilter::res_conflict(Block* filter, Block* block1, Block* block2, uint32_t glbi1, uint32_t glbi2, uint8_t hf, uint64_t murhash ){
    
    
    
    uint8_t lbi1 = glbi1%64;
    uint8_t lbi2 = glbi2%64;
    
    
    uint8_t cuckooed_hf = block1->FSA[find_fsa_slot_noffset(block1, lbi1)];
    uint8_t cuckooed_hf2 = block2->FSA[find_fsa_slot_noffset(block2, lbi2)];
    
    uint32_t glbi3 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(cuckooed_hf), 46*nb_blocks);
    uint8_t lbi3 = glbi3%64;
    Block *block3 = &filter[glbi3/64];
    
    uint32_t glbi4 = map2(glbi2 + (1-((glbi2 & 1)<<1))*offset(cuckooed_hf2), 46*nb_blocks);
    uint8_t lbi4 = glbi4%64;
    Block *block4 = &filter[glbi4/64];
    
    block1->FSA[find_fsa_slot_noffset(block1, lbi1)]=hf;
    set_OTA(block1, lbi1, murhash);
    
    if(table_simple_store(block3, lbi3, cuckooed_hf)){
        return 1;
    }else{
        return res_conflict(filter, block3, block2, glbi3, glbi2, cuckooed_hf, murhash);
    }
    
    return 0;
    
}


inline bool MortonFilter::table_read_and_cmp(MortonFilter::Block *block, uint8_t lbi, uint8_t Fx){
    uint64_t mask0 = (1ull<<(2*lbi)) | (1ull<<(2 * lbi +1));
    uint64_t mask1 = (1ull<<(2*(lbi-32))) | (1ull<<(2 * (lbi-32) + 1));
    uint64_t bkt_size = 0;
    if (lbi < 32 ){
        bkt_size = (block->FCA[1] & mask0) >> (2*lbi);
        uint16_t fsa_slot = find_fsa_slot_noffset(block, lbi);
        for (int i = 0; i<bkt_size; i++ ){
            if(Fx==block->FSA[fsa_slot-i] ) return true;
        }
        return false;
    }
    else{
        bkt_size = (block->FCA[0] & mask1) >> (2*(lbi-32));
        uint16_t fsa_slot = find_fsa_slot_noffset(block, lbi);
        for (int i = 0; i<bkt_size; i++ ){
            if(Fx==block->FSA[fsa_slot-i] ) return true;
        }
        return false;
    }
    
    
}


bool MortonFilter::Contains(void* __restrict data, int len){
    uint64_t murhash0;
    MurmurHash3_x64_128( data, len, 0, &murhash0);
    int nb_buckets = 46 * nb_blocks;
    uint32_t glbi1 = map(murhash0, nb_buckets);
    uint8_t hf = ((&murhash0)[1]>>56);
    if(hf==0)hf++;
    Block *block1 = &filter[glbi1/64];
    uint8_t lbi1 = glbi1 % 64;
    bool match = table_read_and_cmp(block1, lbi1, hf);
    if (match || OTA_bit_is_unset(block1, lbi1, murhash0)){
        return match;
    }
    else{
        uint32_t glbi2 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(hf), nb_buckets);  // nb_buckets HAS to be > 64+OFF_RANGE+1
        Block *block2 = &filter[glbi2/64];
        uint8_t lbi2 = glbi2 % 64;
        return table_read_and_cmp(block2, lbi2, hf);
    }
    return false;
}

inline bool MortonFilter::Contains1(void* __restrict data, int len, int* second_bucket_items, int *size_list){
    uint64_t murhash0;
    MurmurHash3_x64_128( data, len, 0, &murhash0);
    int nb_buckets = 46 * nb_blocks;
    uint32_t glbi1 = map(murhash0, nb_buckets);
    uint8_t hf = ((&murhash0)[1]>>56);
    if(hf==0)hf++;
    Block *block1 = &filter[glbi1/64];
    uint8_t lbi1 = glbi1 % 64;
    bool match = table_read_and_cmp(block1, lbi1, hf);
    if (match || OTA_bit_is_unset(block1, lbi1, murhash0)){
        return match;
    }
    else{
        second_bucket_items = (int*)data;
        second_bucket_items++;
        //        *size_list+=1;
    }
    return false;
}


inline bool MortonFilter::ContainsElse(void* __restrict data, int len){
    uint64_t murhash0;
    MurmurHash3_x64_128( data, len, 0, &murhash0);
    int nb_buckets = 46 * nb_blocks;
    uint32_t glbi1 = map(murhash0, nb_buckets);
    uint8_t hf = ((&murhash0)[1]>>56);
    uint32_t glbi2 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(hf), nb_buckets);  // nb_buckets HAS to be > 64+OFF_RANGE+1
    Block *block2 = &filter[glbi2/64];
    uint8_t lbi2 = glbi2 % 64;
    return table_read_and_cmp(block2, lbi2, hf);
    
    
}
