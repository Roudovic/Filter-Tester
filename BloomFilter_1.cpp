#include "BloomFilter_1.h"
#include "MurmurHash3.h"
#include "farmhash-master/src/farmhash.h"



BloomFilter_1::BloomFilter_1(uint64_t size, uint8_t numHashes)
      : m_bits(size),
        m_numHashes(numHashes) {}


std::array<uint64_t, 2> farmhash(const uint8_t *data,
                             std::size_t len) {
    std::array<uint64_t, 2> hashValue;
    util::uint128_t hash128 = util::Fingerprint128((char*)data, len);
    hashValue[0]= util::Uint128Low64(hash128);
    hashValue[1]= util::Uint128High64(hash128);
    
    return hashValue;
}


inline uint64_t nthHash(uint8_t n,
                        uint64_t hashA,
                        uint64_t hashB,
                        uint64_t filterSize) {
    return (hashA + n * hashB) % filterSize;
}



void BloomFilter_1::add(const uint8_t *data, std::size_t len) {
  auto hashValues = farmhash(data,len);
  uint16_t word = hashValues[1]%m_bits.size();
  for (int n = 0; n < m_numHashes; n++) {
      m_bits[word][nthHash(n, hashValues[0], hashValues[1], WORD_SIZE)] = true;
  }
}

bool BloomFilter_1::possiblyContains(const uint8_t *data, std::size_t len) const {
  auto hashValues = farmhash(data, len);
  uint16_t word = hashValues[1]%m_bits.size();
  for (int n = 0; n < m_numHashes; n++) {
      if (!m_bits[word][nthHash(n, hashValues[0], hashValues[1], WORD_SIZE)]) {
          return false;
      }
  }

  return true;
}
