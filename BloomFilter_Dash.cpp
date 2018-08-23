//
//  BloomFilter_Dash.cpp
//  
//
//  Created by Ludovic Theobald on 17/8/2561 BE.
//

#include "BloomFilter_Dash.hpp"
#include "MurmurHash3.h"
#include "farmhash-master/src/farmhash.h"

BloomFilter_Dash::BloomFilter_Dash(uint64_t size, uint8_t numHashes)
: m_bits(size),
m_numHashes(numHashes) {}

//std::array<uint64_t, 2> hash(const uint8_t *data,
//                             std::size_t len) {
//  std::array<uint64_t, 2> hashValue;
//  MurmurHash3_x64_128(data, len, 0, hashValue.data());
//
//  return hashValue;
//}

std::array<uint32_t, 20> BloomFilter_Dash::hash (const uint8_t *data,
                             std::size_t len) const {
    std::array<uint32_t, 20> hashValue;
    for(int i = 0; i< m_numHashes ; i++){
        MurmurHash3_x86_32(data,len,i,&hashValue[i]);
        hashValue[i]=hashValue[i]%m_bits.size();
    }
    return hashValue;
}

//std::array<uint32_t, 20> BloomFilter_Dash::hash (const uint8_t *data,
//                                                 std::size_t len) const {
//    std::array<uint32_t, 20> hashValue;
//    for(int i = 0; i< m_numHashes ; i++){
////        hashValue[i]=util::Hash32WithSeed(data, len, i) % m_bits.size();
//          hashValue[i] = util::Fingerprint32((char*)data, len) % (m_bits.size()-i);
//    }
//    return hashValue;
//}



inline uint64_t nthHash(uint8_t n,
                        uint64_t hashA,
                        uint64_t hashB,
                        uint64_t filterSize) {
    return (hashA + n * hashB) % filterSize;
}

void BloomFilter_Dash::add(const uint8_t *data, std::size_t len) {
    auto hashValues = hash(data, len);
    
    for (int n = 0; n < m_numHashes; n++) {
        m_bits[hashValues[n]] = true;
    }
}

bool BloomFilter_Dash::possiblyContains(const uint8_t *data, std::size_t len) const  {
    auto hashValues = hash(data, len);
    
    for (int n = 0; n < m_numHashes; n++) {
        if (!m_bits[hashValues[n]]) {
            return false;
        }
    }
    
    return true;
}

int BloomFilter_Dash::SizeInBytes(){
    return m_bits.size();
}
