#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <vector>
#include <stdint.h>
#include <array>



struct BloomFilter {
  BloomFilter(uint64_t size, uint8_t numHashes);

  void add(const uint8_t *data, std::size_t len);
  bool possiblyContains(const uint8_t *data, std::size_t len) const;
    int SizeInBytes();
private:
  uint8_t m_numHashes;
  std::vector<bool> m_bits;
};

#endif
