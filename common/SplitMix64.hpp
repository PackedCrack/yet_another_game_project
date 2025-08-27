#pragma once

//
//
namespace common
{
template<typename derived_t>
class SplitMix64
{
protected:
	[[nodiscard]] std::uint64_t splitmix64(std::uint64_t state) const noexcept
	{
		// https://rosettacode.org/wiki/Pseudo-random_numbers/Splitmix64
		state += 0x9e3779b97f4a7c15;					/* increment the state variable */
		std::uint64_t z = state;						/* copy the state to a working variable */
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;		/* xor the variable with the variable right bit shifted 30 then multiply by a constant */
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;		/* xor the variable with the variable right bit shifted 27 then multiply by a constant */
		return z ^ (z >> 31);							/* return the variable xored with itself right bit shifted 31 */
	}
};
}	// namespace common