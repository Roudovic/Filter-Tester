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
//#include "../../xxhash.hpp"
#include "../../cityhash-master/src/city.h"
#include "../../cityhash-master/src/citycrc.h"
#include "../../farmhash-master/src/farmhash.h"


#endif /* MortonFilter_hpp */
#define LEN 10
#define OFF_RANGE 256
#define BLOCK_OVERFLOW -3
#define BUCKET_OVERFLOW -2

class MortonFilter{
    // Public here for tests purposes, TO CHANGE
    public :
    
    uint32_t nb_blocks;
    typedef struct Block {
        uint64_t FCA[2];
        uint8_t FSA[46];
        uint16_t OTA;
        
        
    } __attribute((aligned(64)))Block;
   
    
    Block *filter;
    
    cuckoofilter::TwoIndependentMultiplyShift hasher;
    
    int Add(void* __restrict data, int len);
    
    int table_simple_store(Block *block, uint8_t lbi, uint8_t Fx);
    
    void set_OTA(Block *block, uint8_t lbi);
    
    bool Contains(void* __restrict data, int len);
    
    bool Contains1(void* __restrict data, int len, int* second_bucket_items) const ;
    
    bool ContainsElse(void* __restrict data, int len);
    
    bool table_read_and_cmp(Block *block, uint8_t lbi, uint8_t Fx) const ;
    
    bool OTA_bit_is_unset(Block *block, uint8_t lbi) const;
    
    int res_conflict(Block* filter, Block* block1, Block* block2, uint32_t glbi1, uint32_t glbi2, uint8_t hf, int overflow_type1, int overflow_type2 );
    
    uint8_t cuckoo_eviction_block( Block* block, uint32_t *glbi) ;
    
    
    uint16_t find_fsa_slot(Block* block, uint8_t lbi ) const ;
    
    uint16_t find_fsa_slot_noffset(Block *block, uint8_t lbi) const;
    
    int SizeInBits();
    
    // TODO : Create a method to get the real FCA[i] without using bits arithmetic everytime
    // like block.getFCAindex(i)
    
    explicit MortonFilter(int nb_items_expected) {
        nb_blocks = (1.1 * nb_items_expected / 46) + 2 ;
        size_t size_block = sizeof(struct Block);
        //filter = (Block*)malloc(size_block * nb_blocks);
        filter = new Block[nb_blocks];
        memset(filter, 0, size_block*nb_blocks);
    }
};

int MortonFilter::SizeInBits(){
    return nb_blocks*sizeof(struct Block)*8;
    
}

inline uint32_t map(uint32_t x, uint32_t n){
    return ((__uint128_t)(x) * (__uint128_t)n)>>32;
}

inline uint32_t map2(int x, uint32_t n){
    if(x>=0 && x<=n-1) return x;
    else if (x<0) return x+n;
    else return x-n;
}

inline uint32_t mapOTA(uint8_t x, uint32_t n){
    return (x * n)>>8;
}




inline uint8_t offset(uint8_t Fx){
    return  (64 + (Fx & (OFF_RANGE-1))) | 1;
}

inline uint16_t MortonFilter::find_fsa_slot(MortonFilter::Block *block, uint8_t lbi) const {
    __uint128_t one_128 = 1;
    __uint128_t fullnessCounterArrayMask = (one_128 << (2 * lbi)) - 1;
    //    __uint128_t FCA128 = (__uint128_t)block->FCA;
    uint64_t mFCA[2];
    mFCA[1]=block->FCA[1]&fullnessCounterArrayMask;
    mFCA[0]=block->FCA[0]&(fullnessCounterArrayMask>>64);
    
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


inline uint16_t MortonFilter::find_fsa_slot_noffset(MortonFilter::Block *block, uint8_t lbi) const {
    __uint128_t one_128 = 1;
    __uint128_t fullnessCounterArrayMask = (one_128 << (2 * lbi)) - 1;
    
    //    __uint128_t FCA128 = (__uint128_t)block->FCA;
    uint64_t mFCA[2];
    
    mFCA[1]=block->FCA[1]&fullnessCounterArrayMask;
    mFCA[0]=block->FCA[0]&(fullnessCounterArrayMask>>64);
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

inline int MortonFilter::table_simple_store(Block *block, uint8_t lbi, uint8_t Fx){
    // Checking FSA to be non-full
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
            return 1;
        }
        if ( lbi > 31 && ((block->FCA[0] & mask1)>>(2*(lbi-32))) < 3){
            block->FCA[0] += 1ull<<(2 * (lbi-32));
            //            uint8_t fsa_slot = find_fsa_slot(block,lbi);
            for(int i = 1 ; i <= fsa_slot; i++){
                block->FSA[i-1]=block->FSA[i];
            }
            block->FSA[fsa_slot]=Fx;
            return 1;
        }
        
        return BUCKET_OVERFLOW;
    }
    // Little endian implementation...
    
    
    return BLOCK_OVERFLOW;
}


// Get the Murmurhash dependance out

inline void MortonFilter::set_OTA(Block *block, uint8_t lbi){
    
    block->OTA |= 1<< (mapOTA(lbi,16));
    
}

inline bool MortonFilter::OTA_bit_is_unset(Block *block, uint8_t lbi) const {
    return (block->OTA & (1<< (mapOTA(lbi,16)))) == 0;
//    return false;
}


inline int MortonFilter::Add(void* __restrict data, int len){
//    uint64_t murhash0;
//    MurmurHash3_x64_128( data, len, 0, &murhash0);
    int* key = (int*)data;
//    murhash0 = hasher(*key );
    
//    TEST FOR XXHASH
//    xxh::hash_t<64> murhash0 = xxh::xxhash<64>(key,4);
//    TEST FOR XXHASH
    
    //    TEST FOR CITYHASH
//   uint64_t murhash0 = CityHash64((char*)data, len);
    //    TEST FOR CITYHASH
    
    //    TEST FOR FARMHASH
    uint64_t murhash0 = util::Fingerprint64((char*)data, len);
    //    TEST FOR FARMHASH
    
    
    uint32_t glbi1 = (murhash0 & 0xffffffff00000000)>>32;
    int nb_buckets = 64 * nb_blocks;
    glbi1 = map(glbi1, nb_buckets);
    uint32_t glbi2;
    uint8_t lbi2 ;
    Block *block2;
    //    printf("MurmurHash Value : %llx \n", murhash0);
    //    printf("Global block index : %x \n", glbi1 );
    
    uint8_t hf = murhash0 & ((1ull << 8)-1);
    if(hf==0) {hf++;}
    //    printf("Hf : %x\n", hf);
    Block *block1 = &filter[glbi1/64];
    uint8_t lbi1 = glbi1 % 64;
    if(table_simple_store(block1, lbi1, hf)>0){
        //        std::cout << "Hash 1 is used "<< std::endl;
        return 1;
    }
    else{
//                std::cout << "Hash 2 is used "<< std::endl;
        set_OTA(block1, lbi1);
        glbi2 = map2((glbi1 + (1-((glbi1 & 1)<<1))*offset(hf)), nb_buckets);
        block2 = &(filter[glbi2/64]);
        lbi2 = glbi2 % 64;
        if (table_simple_store(block2, lbi2, hf)>0){
            return 2;
        }
    }
    
//    return 0;
    
    return res_conflict(filter, block1, block2, glbi1, glbi2, hf, table_simple_store(block1, lbi1, hf), table_simple_store(block2, lbi2, hf) );
    
    
    
    
    
}

int MortonFilter::res_conflict(Block* filter, Block* block1, Block* block2, uint32_t glbi1, uint32_t glbi2, uint8_t hf, int overflow_type1, int overflow_type2){
//    std::cout << " Resolution Conflict"  << std::endl;
    uint8_t lbi1 = glbi1%64;
    uint8_t lbi2 = glbi2%64;
    
   
    uint8_t fsa_slot_block2 = find_fsa_slot_noffset(block2, lbi2) - rand()%3;

    
    uint8_t cuckooed_hf2 = block2->FSA[fsa_slot_block2];
    
    
   
    
    uint32_t glbi4 = map2(glbi2 + (1-((glbi2 & 1)<<1))*offset(cuckooed_hf2), 64*nb_blocks);
    uint8_t lbi4 = glbi4%64;
    Block *block4 = &filter[glbi4/64];
    
//    int block = rand()%2;
    int block = 1;
//
//    std::cout << " Conflict Resolution" << std::endl;
    
    switch (block) {
        case 1:
            if(overflow_type1 == BUCKET_OVERFLOW){
                uint8_t fsa_slot_block1 = find_fsa_slot_noffset(block1, lbi1) - rand()%3;
                uint8_t cuckooed_hf = block1->FSA[fsa_slot_block1];
                uint32_t glbi3 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(cuckooed_hf), 64*nb_blocks);
                uint8_t lbi3 = glbi3%64;
                Block *block3 = &filter[glbi3/64];
                
                block1->FSA[fsa_slot_block1]=hf;
                set_OTA(block1, lbi1);
                int otype3 = table_simple_store(block3, lbi3, cuckooed_hf);
                if(otype3 > 0){
                    return 1;
                }else{
                    return res_conflict(filter, block3, block1, glbi3, glbi1, cuckooed_hf, otype3, overflow_type1 );
                }
            }
            else{
                uint32_t glbi5 = glbi1 ;
                uint8_t cuckooed_hf_block = cuckoo_eviction_block(block1, &glbi5);
                if(table_simple_store(block1, lbi1, hf)<0) {
                    res_conflict(filter, block1, block2, glbi1, glbi2, hf, BUCKET_OVERFLOW, overflow_type2);
                }
                
                uint32_t glbi5_b = map2((glbi5 + (1-((glbi5 & 1)<<1))*offset(cuckooed_hf_block)), 64 * nb_blocks);
                Block *block5_b = &filter[glbi5_b/64];
                uint8_t lbi5_b = glbi5_b%64;
                set_OTA(block1, glbi5%64);
                int otype5_b = table_simple_store(block5_b, lbi5_b, cuckooed_hf_block);
                if( otype5_b > 0){
                    return 1;
                }else{
                    return res_conflict(filter, block5_b, block1, glbi5_b, glbi5, cuckooed_hf_block, otype5_b, overflow_type1 );
                }
            }
                break;
                
                
            default:
            if(overflow_type1 == BUCKET_OVERFLOW){
                block2->FSA[find_fsa_slot_noffset(block2, lbi2)]=hf;
                set_OTA(block2, lbi2);
                int otype4 = table_simple_store(block4, lbi4, cuckooed_hf2);
                if(otype4 > 0 ){
                    return 1;
                }else{
                    return res_conflict(filter, block4, block2, glbi4, glbi2, cuckooed_hf2, otype4, overflow_type2);
                }
            }
            else{
                uint32_t glbi5 = glbi2;
                uint8_t cuckooed_hf_block = cuckoo_eviction_block(block2, &glbi5);
                if(table_simple_store(block2, lbi2, hf)>0){
                    res_conflict( filter, block1, block2, glbi1, glbi2, hf, overflow_type1, BUCKET_OVERFLOW);
                }
                set_OTA(block2, lbi2);
                uint32_t glbi5_b = map2((glbi5 + (1-((glbi5 & 1)<<1))*offset(cuckooed_hf_block)), 64 * nb_blocks);
                Block *block5_b = &filter[glbi5_b/64];
                uint8_t lbi5_b = glbi5_b%64;
                int otype5_b = table_simple_store(block5_b, lbi5_b, cuckooed_hf_block);
                if(otype5_b>0){
                    return 1;
                }else{
                    return res_conflict(filter, block5_b, block2, glbi5_b, glbi5, cuckooed_hf_block, otype5_b, overflow_type2 );
                }
            }
            
            
            return 0;
    }
    
    return 0;
}
    

uint8_t MortonFilter::cuckoo_eviction_block(MortonFilter::Block *block, uint32_t *glbi){
//   std::cout << "cuckoo block eviction" << std::endl;
    uint64_t mFCA[2];
    mFCA[0]=block->FCA[0];
    mFCA[1]=block->FCA[1];
    int randindex = rand()%128;
    uint64_t mask = 1;
    for(int i =0; i<64; i++){
        if(i >= randindex && (mask & mFCA[1])>0){
            if( i % 2 == 0 ){
                block->FCA[1] -= (1ull<<i);
                *glbi = ((*glbi)/64)*64 + i/2;
                uint8_t cuckooed_slot = find_fsa_slot_noffset(block, i/2);
                uint8_t cuckooed_hf = block->FSA[cuckooed_slot];
                for(int j = cuckooed_slot; j > 0 ; j-- ){
                    block->FSA[j]=block->FSA[j-1];
                }
                
                block->FSA[0]=0;
                return cuckooed_hf;
            }
            else{
                block->FCA[1] -= (1ull<<(i-1));
                *glbi = ((*glbi)/64)*64 + i/2;
                uint8_t cuckooed_slot = find_fsa_slot_noffset(block, i/2);
                uint8_t cuckooed_hf = block->FSA[cuckooed_slot];
                for(int j = cuckooed_slot; j > 0 ; j-- ){
                    block->FSA[j]=block->FSA[j-1];
                }
                block->FSA[0]=0;
                return cuckooed_hf;
            }
        }
        mask<<=1;
    }
    mask=1;
    for(int i =0; i<64; i++){
        if(i > randindex-64 && (mask & mFCA[0])>0){
            if( i % 2 == 0 ){
                block->FCA[0] -= (1ull<<i);
                *glbi = ((*glbi)/64)*64 + 32 +i/2;
                uint8_t cuckooed_slot = find_fsa_slot_noffset(block, 32 + i/2);
                 uint8_t cuckooed_hf = block->FSA[cuckooed_slot];
                for(int i = cuckooed_slot; i > 0 ; i-- ){
                    block->FSA[i]=block->FSA[i-1];
                }
                block->FSA[0]=0;
                return cuckooed_hf;
            }
            else{
                block->FCA[0] -= (1ull<<(i-1));
                *glbi = ((*glbi)/64)*64 +32+ i/2;
                uint8_t cuckooed_slot = find_fsa_slot_noffset(block, 32 + i/2);
                uint8_t cuckooed_hf = block->FSA[cuckooed_slot];
                for(int i = cuckooed_slot; i > 0 ; i-- ){
                    block->FSA[i]=block->FSA[i-1];
                }
                block->FSA[0]=0;
                return cuckooed_hf;
            }
        }
        mask<<=1;
    }
    mask=1;
    for(int i =0; i<64; i++){
        if((mask & mFCA[1])>0){
            if( i % 2 == 0 ){
                block->FCA[1] -= (1ull<<i);
                *glbi = ((*glbi)/64)*64 + i/2;
                uint8_t cuckooed_slot = find_fsa_slot_noffset(block, i/2);
                uint8_t cuckooed_hf = block->FSA[cuckooed_slot];
                for(int j = cuckooed_slot; j > 0 ; j-- ){
                    block->FSA[j]=block->FSA[j-1];
                }
                
                block->FSA[0]=0;
                return cuckooed_hf;
            }
            else{
                block->FCA[1] -= (1ull<<(i-1));
                *glbi = ((*glbi)/64)*64 + i/2;
                uint8_t cuckooed_slot = find_fsa_slot_noffset(block, i/2);
                uint8_t cuckooed_hf = block->FSA[cuckooed_slot];
                for(int j = cuckooed_slot; j > 0 ; j-- ){
                    block->FSA[j]=block->FSA[j-1];
                }
                block->FSA[0]=0;
                return cuckooed_hf;
            }
        }
        mask<<=1;
    }
    return 0;
    
    
    
}


    inline bool MortonFilter::table_read_and_cmp(MortonFilter::Block *block, uint8_t lbi, uint8_t Fx) const {
        uint64_t mask0 = (1ull<<(2*lbi)) | (1ull<<(2 * lbi +1));
        uint64_t mask1 = (1ull<<(2*(lbi-32))) | (1ull<<(2 * (lbi-32) + 1));
        uint64_t bkt_size = 0;
        uint16_t fsa_slot = find_fsa_slot_noffset(block, lbi);
        if (lbi < 32 ){
            bkt_size = (block->FCA[1] & mask0) >> (2*lbi);
            
            for (int i = 0; i<bkt_size; i++ ){
                if(Fx==block->FSA[fsa_slot-i] ) return true;
            }
            return false;
        }
        else{
            bkt_size = (block->FCA[0] & mask1) >> (2*(lbi-32));
            
            for (int i = 0; i<bkt_size; i++ ){
                if(Fx==block->FSA[fsa_slot-i] ) return true;
            }
            return false;
        }
        
        
    }


bool MortonFilter::Contains(void* __restrict data, int len){
//    uint64_t murhash0;
//        MurmurHash3_x64_128( data, len, 0, &murhash0);
//    murhash0 = hasher(*key );
    
    //    TEST FOR XXHASH
//    xxh::hash_t<64> murhash0 = xxh::xxhash<64>(key,4);
    //    TEST FOR XXHASH
    
    
    //    TEST FOR CITYHASH
//    murhash0 = CityHash64((char*)data, len);
    //    TEST FOR CITYHASH
    
    //    TEST FOR FARMHASH
    uint64_t murhash0 = util::Fingerprint64((char*)data, len);
    //
    
    uint32_t glbi1 = (murhash0 & 0xffffffff00000000)>>32;
    int nb_buckets = 64 * nb_blocks;
    glbi1 = map(glbi1, nb_buckets);
    uint32_t glbi2;
    uint8_t lbi2 ;
    Block *block2;
    //    printf("MurmurHash Value : %llx \n", murhash0);
    //    printf("Global block index : %x \n", glbi1 );
    
    uint8_t hf = murhash0 & ((1ull << 8)-1);
    if(hf==0) {hf++;}
    
    Block *block1 = &filter[glbi1/64];
    uint8_t lbi1 = glbi1 % 64;
    bool match = table_read_and_cmp(block1, lbi1, hf);
    if (match || OTA_bit_is_unset(block1, lbi1)){
        return match;
    }
    else{
        glbi2 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(hf), nb_buckets);  // nb_buckets HAS to be > 64+OFF_RANGE+1
        block2 = &filter[glbi2/64];
        lbi2 = glbi2 % 64;
        return table_read_and_cmp(block2, lbi2, hf);
    }
    return false;
}

inline bool MortonFilter::Contains1(void*data, int len, int* second_bucket_items) const {
    
//    MurmurHash3_x64_128( data, len, 0, &murhash0);
//    murhash0 = hasher(*key);
//    char* key = (char*)data;
    //    TEST FOR XXHASH
//    xxh::hash_t<64> murhash0 = xxh::xxhash<64>(key,len);
    //    TEST FOR XXHASH
    
    //    TEST FOR CITYHASH
//    uint64_t murhash0 = CityHash64((char*)data, len);
    //    TEST FOR CITYHASH
    
    
    //    TEST FOR FARMHASH
    uint64_t murhash0 = util::Fingerprint64((char*)data, len);
    //    TEST FOR FARMHASH
    
    
    
    int nb_buckets = 64 * nb_blocks;
    uint32_t glbi1 = (murhash0 & 0xffffffff00000000)>>32;
    
    uint8_t hf = murhash0 & ((1ull << 8)-1);
    if(hf==0) {hf++;}
    glbi1 = map(glbi1, nb_buckets);
    Block *block1 = &filter[glbi1/64];
    uint8_t lbi1 = glbi1 % 64;
    bool match = table_read_and_cmp(block1, lbi1, hf);
    if (match || OTA_bit_is_unset(block1, lbi1)){
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
//    MurmurHash3_x64_128( data, len, 0, &murhash0);
//    murhash0 = hasher(*key);
    
    //    TEST FOR XXHASH
//    xxh::hash_t<64> murhash0 = xxh::xxhash<64>(key,4);
    //    TEST FOR XXHASH
    
//    TEST FOR CITYHASH
   murhash0 = CityHash64((char*)data, len);
//    TEST FOR CITYHASH
    
    int nb_buckets = 64 * nb_blocks;
    uint32_t glbi1 = (murhash0 & 0xffffffff00000000)>>32;
    glbi1 = map(glbi1, nb_buckets);
    uint8_t hf = murhash0 & ((1ull << 8)-1);
    if(hf==0) {hf++;}
    
    uint32_t glbi2 = map2(glbi1 + (1-((glbi1 & 1)<<1))*offset(hf), nb_buckets);  // nb_buckets HAS to be > 64+OFF_RANGE+1
    Block *block2 = &filter[glbi2/64];
    uint8_t lbi2 = glbi2 % 64;
    return table_read_and_cmp(block2, lbi2, hf);
    
    
}
