#include "BloomFilter.h"
#include "MurmurHash3.h"
#include "farmhash-master/src/farmhash.h"

BloomFilter::BloomFilter(uint64_t size, uint8_t numHashes)
      : m_bits(size),
        m_numHashes(numHashes) {}

//std::array<uint64_t, 2> hash(const uint8_t *data,
//                             std::size_t len) {
//  std::array<uint64_t, 2> hashValue;
//  MurmurHash3_x64_128(data, len, 0, hashValue.data());
//
//  return hashValue;
//}

std::array<uint64_t, 2> hash(const uint8_t *data,
                             std::size_t len) {
    std::array<uint64_t, 2> hashValue;
    util::uint128_t zer(0,0);
//    util::uint128_t hash128 = util::Hash128WithSeed((char*) data, len, zer);
//    hashValue[0]= util::Uint128Low64(hash128);
//    hashValue[1]= util::Uint128High64(hash128);
    hashValue[0]= util::Hash32WithSeed((char*)data,len,0);
    hashValue[1]= util::Hash32WithSeed((char*)data,len,1);

    return hashValue;
}


inline uint64_t nthHash(uint8_t n,
                        uint64_t hashA,
                        uint64_t hashB,
                        uint64_t filterSize) {
    return (hashA + n * hashB) % filterSize;
}

void BloomFilter::add(const uint8_t *data, std::size_t len) {
  auto hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())] = true;
  }
}

bool BloomFilter::possiblyContains(const uint8_t *data, std::size_t len) const  {
  auto hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      if (!m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())]) {
          return false;
      }
  }

  return true;
}

int BloomFilter::SizeInBytes(){
    return m_bits.size();
}
