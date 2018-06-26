#ifndef HASHER_H
#define HASHER_H
#include <array>

class Hasher
{
    public:
        Hasher();
        virtual ~Hasher();
        static std::array<uint64_t, 2> hash(const uint8_t *data,
                             std::size_t len);

    protected:

    private:
};

#endif // HASHER_H
