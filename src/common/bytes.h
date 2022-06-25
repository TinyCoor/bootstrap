//
// Created by 12132 on 2022/4/7.
//

#ifndef BYTES_H_
#define BYTES_H_
#include <cstdint>
namespace gfx {
inline bool is_platform_little_endian()
{
	int n = 1;
	return (*(char*)&n) == 1;
}

inline uint8_t get_upper_nybble(uint8_t value)
{
	return static_cast<uint8_t>((value & 0xf0) >> 4);
}

inline uint8_t get_lower_nybble(uint8_t value)
{
	return static_cast<uint8_t>(value & 0x0f);
}

inline uint16_t endian_swap_word(uint16_t value)
{
	return (value >> 8) | (value << 8);
}

inline uint32_t endian_swap_dword(uint32_t value)
{
	return ((value >> 24) & 0xff)
		|  ((value << 8) & 0xff0000)
		|  ((value >> 8) & 0xff00)
		|  ((value << 24) & 0xff000000);
}

inline uint64_t endian_swap_qword(uint64_t value) {
	return ((value & 0x00000000000000ffu) << 56) |
		((value & 0x000000000000ff00u) << 40) |
		((value & 0x0000000000ff0000u) << 24) |
		((value & 0x00000000ff000000u) << 8)  |
		((value & 0x000000ff00000000u) >> 8)  |
		((value & 0x0000ff0000000000u) >> 24) |
		((value & 0x00ff000000000000u) >> 40) |
		((value & 0xff00000000000000u) >> 56);
}

inline uint8_t set_lower_nybble(uint8_t original, uint8_t value)
{
	uint8_t res = original;
	res &= 0xF0;
	res |= (value & 0x0F);
	return res;
}

inline uint8_t set_upper_nybble(uint8_t original, uint8_t value)
{
	uint8_t res = original;
	res &= 0x0F;
	res |= ((value << 4) & 0xF0);
	return res;
}

static inline uint64_t rotl(uint64_t n, uint8_t c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) -1);
	c &= mask;
	return (n << c) | (n >>( (-c) & mask) );
}

static inline uint64_t rotr(uint64_t n, uint8_t c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) -1);
	c &= mask;
	return (n >> c) | (n << ( (-c) & mask) );
}
}
#endif // BYTES_H_
