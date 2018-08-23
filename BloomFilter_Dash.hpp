//
//  BloomFilter_Dash.hpp
//  
//
//  Created by Ludovic Theobald on 17/8/2561 BE.
//

#ifndef BloomFilter_Dash_hpp
#define BloomFilter_Dash_hpp

#include <vector>
#include <stdint.h>
#include <array>



struct BloomFilter_Dash {
    BloomFilter_Dash(uint64_t size, uint8_t numHashes);
    
    std::array<uint32_t, 20> hash(const uint8_t *data,
                                                    std::size_t len) const;
    void add(const uint8_t *data, std::size_t len);
    bool possiblyContains(const uint8_t *data, std::size_t len) const;
    int SizeInBytes();
private:
    uint8_t m_numHashes;
    std::vector<bool> m_bits;
};


#endif /* BloomFilter_Dash_hpp */
