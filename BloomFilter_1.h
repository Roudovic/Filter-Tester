#ifndef BLOOMFILTER_1_H
#define BLOOMFILTER_1_H


#include <vector>
#include <stdint.h>
#include <array>
#define WORD_SIZE 32

class BloomFilter_1 {
public:
  BloomFilter_1(uint64_t size, uint8_t numHashes);

  void add(const uint8_t *data, std::size_t len);
  bool possiblyContains(const uint8_t *data, std::size_t len) const;

private:

  uint8_t m_numHashes;
    std::vector<std::array<bool,32>> m_bits;


};


#endif // BLOOMFILTER_1_H
