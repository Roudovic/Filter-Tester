#include "Hasher.h"
#include "MurmurHash3.h"

Hasher::Hasher()
{
    //ctor
}

Hasher::~Hasher()
{
    //dtor
}

std::array<uint64_t, 2> Hasher::hash(const uint8_t *data,
                             std::size_t len) {
  std::array<uint64_t, 2> hashValue;
  MurmurHash3_x64_128(data, len, 0, hashValue.data());

  return hashValue;
}


