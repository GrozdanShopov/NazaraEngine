// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <limits>
#include <utility>
#include <Nazara/Core/Debug.hpp>

#ifdef NAZARA_COMPILER_MSVC
	// Bits tricks require us to disable some warnings under VS
	#pragma warning(disable: 4146)
	#pragma warning(disable: 4804)
#endif

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Bitset
	* \brief Core class that represents a set of bits
	*
	* This class meets the requirements of Container, AllocatorAwareContainer, SequenceContainer
	*/

	/*!
	* \brief Constructs a Bitset object by default
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>::Bitset() :
	m_bitCount(0)
	{
	}

	/*!
	* \brief Constructs a Bitset object of bitCount bits to value val
	*
	* \param bitCount Number of bits
	* \param val Value of those bits, by default false
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>::Bitset(unsigned int bitCount, bool val) :
	Bitset()
	{
		Resize(bitCount, val);
	}

	/*!
	* \brief Constructs a Bitset object from the contents initialized with a copy of the null-terminated character string pointed to by bits
	*
	* \param bits Null-terminated character string containing only '0' and '1'
	*
	* \remark The length of the string is determined by the first null character, if there is no null character, the behaviour is undefined
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>::Bitset(const char* bits) :
	Bitset(bits, std::strlen(bits))
	{
	}

	/*!
	* \brief Constructs a Bitset object from the contents initialized with a copy of the character string pointed to by bits takings the bitCount first characters
	*
	* \param bits Character string containing only '0' and '1'
	* \param bitCount Number of characters to take into consideration
	*
	* \remark If the length of the string is inferior to the bitCount, the behaviour is undefined
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>::Bitset(const char* bits, unsigned int bitCount) :
	m_blocks(ComputeBlockCount(bitCount), 0U),
	m_bitCount(bitCount)
	{
		for (unsigned int i = 0; i < bitCount; ++i)
		{
			switch (*bits++)
			{
				case '1':
					// We adapt the index (inversion in comparison to the string)
					Set(m_bitCount - i - 1, true);
					break;

				case '0':
					// Each block is zero-initialised, nothing to do
					break;

				default:
					NazaraAssert(false, "Unexpected char (neither 1 nor 0)");
					break;
			}
		}
	}

	/*!
	* \brief Constructs a Bitset object from a Nz::String
	*
	* \param bits String containing only '0' and '1'
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>::Bitset(const String& bits) :
	Bitset(bits.GetConstBuffer(), bits.GetSize())
	{
	}

	/*!
	* \brief Clears the content of the bitset, GetSize() is now equals to 0
	*
	* \remark The memory allocated is not released
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Clear() noexcept
	{
		m_bitCount = 0;
		m_blocks.clear();
	}

	/*!
	* \brief Counts the number of bits set to 1
	* \return Number of bits set to 1
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::Count() const
	{
		if (m_blocks.empty())
			return 0;

		unsigned int count = 0;
		for (unsigned int i = 0; i < m_blocks.size(); ++i)
			count += CountBits(m_blocks[i]);

		return count;
	}

	/*!
	* \brief Flips each bit of the bitset
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Flip()
	{
		for (Block& block : m_blocks)
			block ^= fullBitMask;

		ResetExtraBits();
	}

	/*!
	* \brief Finds the first bit set to one in the bitset
	* \return Index of the first bit
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::FindFirst() const
	{
		return FindFirstFrom(0);
	}

	/*!
	* \brief Finds the next bit set to one in the bitset
	* \return Index of the next bit if exists or npos
	*
	* \param bit Index of the bit, the search begin with bit + 1
	*
	* \remark Produce a NazaraAssert if bit is greather than number of bits in bitset
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::FindNext(unsigned int bit) const
	{
		NazaraAssert(bit < m_bitCount, "Bit index out of range");

		if (++bit >= m_bitCount)
			return npos;

		// The block of the bit and its index
		unsigned int blockIndex = GetBlockIndex(bit);
		unsigned int bitIndex = GetBitIndex(bit);

		// We get the block
		Block block = m_blocks[blockIndex];

		// We ignore the X first bits
		block >>= bitIndex;

		// If the block is not empty, it's good, else we must keep trying with the next block
		if (block)
			return IntegralLog2Pot(block & -block) + bit;
		else
			return FindFirstFrom(blockIndex + 1);
	}

	/*!
	* \brief Gets the ith block
	* \return Block in the bitset
	*
	* \param i Index of the block
	*
	* \remark Produce a NazaraAssert if i is greather than number of blocks in bitset
	*/

	template<typename Block, class Allocator>
	Block Bitset<Block, Allocator>::GetBlock(unsigned int i) const
	{
		NazaraAssert(i < m_blocks.size(), "Block index out of range");

		return m_blocks[i];
	}

	/*!
	* \brief Gets the number of blocks
	* \return Number of blocks
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::GetBlockCount() const
	{
		return m_blocks.size();
	}

	/*!
	* \brief Gets the capacity of the bitset
	* \return Capacity of the bitset
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::GetCapacity() const
	{
		return m_blocks.capacity()*bitsPerBlock;
	}

	/*!
	* \brief Gets the number of bits
	* \return Number of bits
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::GetSize() const
	{
		return m_bitCount;
	}

	/*!
	* \brief Performs the "AND" operator between two bitsets
	*
	* \param a First bitset
	* \param b Second bitset
	*
	* \remark The "AND" is performed with all the bits of the smallest bitset and the capacity of this is set to the largest of the two bitsets
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::PerformsAND(const Bitset& a, const Bitset& b)
	{
		std::pair<unsigned int, unsigned int> minmax = std::minmax(a.GetBlockCount(), b.GetBlockCount());

		// We reinitialise our blocks with zero
		m_blocks.clear();
		m_blocks.resize(minmax.second, 0U);
		m_bitCount = std::max(a.GetSize(), b.GetSize());

		// In case of the "AND", we can stop with the smallest size (because x & 0 = 0)
		for (unsigned int i = 0; i < minmax.first; ++i)
			m_blocks[i] = a.GetBlock(i) & b.GetBlock(i);

		ResetExtraBits();
	}

	/*!
	* \brief Performs the "NOT" operator of the bitset
	*
	* \param a Bitset to negate
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::PerformsNOT(const Bitset& a)
	{
		m_blocks.resize(a.GetBlockCount());
		m_bitCount = a.GetSize();

		for (unsigned int i = 0; i < m_blocks.size(); ++i)
			m_blocks[i] = ~a.GetBlock(i);

		ResetExtraBits();
	}

	/*!
	* \brief Performs the "OR" operator between two bitsets
	*
	* \param a First bitset
	* \param b Second bitset
	*
	* \remark The "OR" is performed with all the bits of the smallest bitset and the others are copied from the largest and the capacity of this is set to the largest of the two bitsets
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::PerformsOR(const Bitset& a, const Bitset& b)
	{
		const Bitset& greater = (a.GetBlockCount() > b.GetBlockCount()) ? a : b;
		const Bitset& lesser = (a.GetBlockCount() > b.GetBlockCount()) ? b : a;

		unsigned int maxBlockCount = greater.GetBlockCount();
		unsigned int minBlockCount = lesser.GetBlockCount();
		m_blocks.resize(maxBlockCount);
		m_bitCount = greater.GetSize();

		for (unsigned int i = 0; i < minBlockCount; ++i)
			m_blocks[i] = a.GetBlock(i) | b.GetBlock(i);

		for (unsigned int i = minBlockCount; i < maxBlockCount; ++i)
			m_blocks[i] = greater.GetBlock(i); // (x | 0 = x)

		ResetExtraBits();
	}

	/*!
	* \brief Performs the "XOR" operator between two bitsets
	*
	* \param a First bitset
	* \param b Second bitset
	*
	* \remark The "XOR" is performed with all the bits of the smallest bitset and the others are copied from the largest and the capacity of this is set to the largest of the two bitsets
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::PerformsXOR(const Bitset& a, const Bitset& b)
	{
		const Bitset& greater = (a.GetBlockCount() > b.GetBlockCount()) ? a : b;
		const Bitset& lesser = (a.GetBlockCount() > b.GetBlockCount()) ? b : a;

		unsigned int maxBlockCount = greater.GetBlockCount();
		unsigned int minBlockCount = lesser.GetBlockCount();
		m_blocks.resize(maxBlockCount);
		m_bitCount = greater.GetSize();

		for (unsigned int i = 0; i < minBlockCount; ++i)
			m_blocks[i] = a.GetBlock(i) ^ b.GetBlock(i);

		for (unsigned int i = minBlockCount; i < maxBlockCount; ++i)
			m_blocks[i] = greater.GetBlock(i); // (x ^ 0 = x)

		ResetExtraBits();
	}

	/*!
	* \brief Checks if bitsets have one block in common
	*
	* \param bitset Bitset to test
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::Intersects(const Bitset& bitset) const
	{
		// We only test the blocks in common
		unsigned int sharedBlocks = std::min(GetBlockCount(), bitset.GetBlockCount());
		for (unsigned int i = 0; i < sharedBlocks; ++i)
		{
			Block a = GetBlock(i);
			Block b = bitset.GetBlock(i);
			if (a & b)
				return true;
		}

		return false;
	}

	/*!
	* \brief Reserves enough blocks to contain bitCount bits
	*
	* \param bitCount Number of bits to reserve
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Reserve(unsigned int bitCount)
	{
		m_blocks.reserve(ComputeBlockCount(bitCount));
	}

	/*!
	* \brief Resizes the bitset to the size of bitCount
	*
	* \param bitCount Number of bits to resize
	* \param defaultVal Value of the bits if new size is greather than the old one
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Resize(unsigned int bitCount, bool defaultVal)
	{
		// We begin with changing the size of container, with the correct value of initialisation
		unsigned int lastBlockIndex = m_blocks.size() - 1;
		m_blocks.resize(ComputeBlockCount(bitCount), (defaultVal) ? fullBitMask : 0U);

		unsigned int remainingBits = GetBitIndex(m_bitCount);
		if (bitCount > m_bitCount && remainingBits > 0 && defaultVal)
			// Initialisation of unused bits in the last block before the size change
			m_blocks[lastBlockIndex] |= fullBitMask << remainingBits;

		m_bitCount = bitCount;
		ResetExtraBits();
	}

	/*!
	* \brief Resets the bitset to zero bits
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Reset()
	{
		Set(false);
	}

	/*!
	* \brief Resets the bit at the index
	*
	* \param bit Index of the bit
	*
	* \see UnboundReset
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Reset(unsigned int bit)
	{
		Set(bit, false);
	}

	/*!
	* \brief Sets the bitset to val
	*
	* \param val Value of the bits
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Set(bool val)
	{
		std::fill(m_blocks.begin(), m_blocks.end(), (val) ? fullBitMask : Block(0U));
		if (val)
			ResetExtraBits();
	}

	/*!
	* \brief Sets the bit at the index
	*
	* \param bit Index of the bit
	* \param val Value of the bit
	*
	* \remark Produce a NazaraAssert if bit is greather than number of bits in bitset
	*
	* \see UnboundSet
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Set(unsigned int bit, bool val)
	{
		NazaraAssert(bit < m_bitCount, "Bit index out of range");

		Block& block = m_blocks[GetBlockIndex(bit)];
		Block mask = Block(1U) << GetBitIndex(bit);

		// Activation of the bit without branching
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		block = (block & ~mask) | (-val & mask);
	}

	/*!
	* \brief Set the ith block
	*
	* \param i Index of the block
	* \param block Block to set
	*
	* \remark Produce a NazaraAssert if i is greather than number of blocks in bitset
	*/
	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::SetBlock(unsigned int i, Block block)
	{
		NazaraAssert(i < m_blocks.size(), "Block index out of range");

		m_blocks[i] = block;
		if (i == m_blocks.size()-1)
			ResetExtraBits();
	}

	/*!
	* \brief Swaps the two bitsets
	*
	* \param bitset Other bitset to swap
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::Swap(Bitset& bitset)
	{
		std::swap(m_bitCount, bitset.m_bitCount);
		std::swap(m_blocks,   bitset.m_blocks);
	}

	/*!
	* \brief Tests the ith bit
	* \return true if bit is set
	*
	* \param bit Index of the bit
	*
	* \remark Produce a NazaraAssert if bit is greather than number of bits in bitset
	*
	* \see UnboundTest
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::Test(unsigned int bit) const
	{
		NazaraAssert(bit < m_bitCount, "Bit index out of range");

		return (m_blocks[GetBlockIndex(bit)] & (Block(1U) << GetBitIndex(bit))) != 0;
	}

	/*!
	* \brief Tests each block
	* \return true if each block is set
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::TestAll() const
	{
		// Special case for the last block
		Block lastBlockMask = GetLastBlockMask();

		for (unsigned int i = 0; i < m_blocks.size(); ++i)
		{
			Block mask = (i == m_blocks.size() - 1) ? lastBlockMask : fullBitMask;
			if (m_blocks[i] == mask) // The extra bits are set to zero, thus we can't test without proceeding with a mask
				return false;
		}

		return true;
	}

	/*!
	* \brief Tests if one bit is set
	* \return true if one bit is set
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::TestAny() const
	{
		if (m_blocks.empty())
			return false;

		for (unsigned int i = 0; i < m_blocks.size(); ++i)
		{
			if (m_blocks[i])
				return true;
		}

		return false;
	}

	/*!
	* \brief Tests if one bit is not set
	* \return true if one bit is not set
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::TestNone() const
	{
		return !TestAny();
	}

	/*!
	* \brief Converts the bitset to template type
	* \return The conversion of the bitset
	*
	* \remark Produce a NazaraAssert if the template type can not hold the number of bits
	*/

	template<typename Block, class Allocator>
	template<typename T>
	T Bitset<Block, Allocator>::To() const
	{
		static_assert(std::is_integral<T>() && std::is_unsigned<T>(), "T must be a unsigned integral type");

		NazaraAssert(m_bitCount <= std::numeric_limits<T>::digits, "Bit count cannot be greater than T bit count");

		T value = 0;
		for (unsigned int i = 0; i < m_blocks.size(); ++i)
			value |= static_cast<T>(m_blocks[i]) << i*bitsPerBlock;

		return value;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object with only '0' and '1'
	*/

	template<typename Block, class Allocator>
	String Bitset<Block, Allocator>::ToString() const
	{
		String str(m_bitCount, '0');

		for (unsigned int i = 0; i < m_bitCount; ++i)
		{
			if (Test(i))
				str[m_bitCount - i - 1] = '1'; // Inversion de l'indice
		}

		return str;
	}

	/*!
	* \brief Resets the bit at the index
	*
	* \param bit Index of the bit
	*
	* \remark if bit is greather than the number of bits, the bitset is enlarged and the added bits are set to false
	*
	* \see Reset
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::UnboundedReset(unsigned int bit)
	{
		UnboundedSet(bit, false);
	}

	/*!
	* \brief Sets the bit at the index
	*
	* \param bit Index of the bit
	* \param val Value of the bit
	*
	* \remark if bit is greather than the number of bits, the bitset is enlarged and the added bits are set to false and the one at bit is set to val
	*
	* \see Set
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::UnboundedSet(unsigned int bit, bool val)
	{
		if (bit < m_bitCount)
			Set(bit, val);
		else if (val)
		{
			// On élargit le bitset seulement s'il y a un bit à marquer
			Resize(bit + 1, false);
			Set(bit, true);
		}
	}

	/*!
	* \brief Tests the ith bit
	* \return true if bit is set
	*
	* \param bit Index of the bit
	*
	* \see Test
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::UnboundedTest(unsigned int bit) const
	{
		if (bit < m_bitCount)
			return Test(bit);
		else
			return false;
	}

	/*!
	* \brief Gets the ith bit
	* \return bit in ith position
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit Bitset<Block, Allocator>::operator[](int index)
	{
		return Bit(m_blocks[GetBlockIndex(index)], Block(1U) << GetBitIndex(index));
	}

	/*!
	* \brief Gets the ith bit
	* \return bit in ith position
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::operator[](int index) const
	{
		return Test(index);
	}

	/*!
	* \brief Negates the bitset
	* \return A new bitset which is the "NOT" of this bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> Bitset<Block, Allocator>::operator~() const
	{
		Bitset bitset;
		bitset.PerformsNOT(*this);

		return bitset;
	}

	/*!
	* \brief Sets this bitset from a Nz::String
	* \return A reference to this
	*
	* \param bits String containing only '0' and '1'
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>& Bitset<Block, Allocator>::operator=(const String& bits)
	{
		Bitset bitset(bits);
		std::swap(*this, bitset);

		return *this;
	}

	/*!
	* \brief Performs an "AND" with another bitset
	* \return A reference to this
	*
	* \param bitset Other bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>& Bitset<Block, Allocator>::operator&=(const Bitset& bitset)
	{
		PerformsAND(*this, bitset);

		return *this;
	}

	/*!
	* \brief Performs an "OR" with another bitset
	* \return A reference to this
	*
	* \param bitset Other bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>& Bitset<Block, Allocator>::operator|=(const Bitset& bitset)
	{
		PerformsOR(*this, bitset);

		return *this;
	}

	/*!
	* \brief Performs an "XOR" with another bitset
	* \return A reference to this
	*
	* \param bitset Other bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>& Bitset<Block, Allocator>::operator^=(const Bitset& bitset)
	{
		PerformsXOR(*this, bitset);

		return *this;
	}

	/*!
	* \brief Finds the position of the first bit set to true after the blockIndex
	* \return The position of the bit
	*
	* \param blockIndex Index of the block
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::FindFirstFrom(unsigned int blockIndex) const
	{
		if (blockIndex >= m_blocks.size())
			return npos;

		// We are looking for the first non-null block
		unsigned int i = blockIndex;
		for (; i < m_blocks.size(); ++i)
		{
			if (m_blocks[i])
				break;
		}

		// Do we have a non-null block ?
		if (i == m_blocks.size())
			return npos;

		Block block = m_blocks[i];

		// Compute the position of LSB in the block (and adjustement of the position)
		return IntegralLog2Pot(block & -block) + i*bitsPerBlock;
	}

	/*!
	* \brief Gets the mask associated to the last block
	* \return Block which represents the mask
	*/

	template<typename Block, class Allocator>
	Block Bitset<Block, Allocator>::GetLastBlockMask() const
	{
		return (Block(1U) << GetBitIndex(m_bitCount)) - 1U;
	}

	/*!
	* \brief Sets to '0' the last bits unassigned in the last block
	*/

	template<typename Block, class Allocator>
	void Bitset<Block, Allocator>::ResetExtraBits()
	{
		Block mask = GetLastBlockMask();
		if (mask)
			m_blocks.back() &= mask;
	}

	/*!
	* \brief Computes the block count with the index of the bit
	* \return Number of the blocks to contain the bit
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::ComputeBlockCount(unsigned int bitCount)
	{
		return GetBlockIndex(bitCount) + ((GetBitIndex(bitCount) != 0U) ? 1U : 0U);
	}

	/*!
	* \brief Computes the bit position in the block
	* \return Index of the bit in the block
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::GetBitIndex(unsigned int bit)
	{
		return bit & (bitsPerBlock - 1U); // bit % bitsPerBlock
	}

	/*!
	* \brief Computes the block index with the index of the bit
	* \return Index of the block containing the bit
	*/

	template<typename Block, class Allocator>
	unsigned int Bitset<Block, Allocator>::GetBlockIndex(unsigned int bit)
	{
		return bit / bitsPerBlock;
	}

	/*!
	* \brief Flips the bit
	* \return A reference to this
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::Flip()
	{
		m_block ^= m_mask;

		return *this;
	}

	/*!
	* \brief Resets the bit
	* \return A reference to this
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::Reset()
	{
		return Set(false);
	}

	/*!
	* \brief Sets the bit to a value
	* \return A reference to this
	*
	* \param val Value of the bit
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::Set(bool val)
	{
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		m_block = (m_block & ~m_mask) | (-val & m_mask);

		return *this;
	}

	/*!
	* \brief Tests the bit
	* \return A reference to this
	*/

	template<typename Block, class Allocator>
	bool Bitset<Block, Allocator>::Bit::Test() const
	{
		return m_block & m_mask;
	}

	/*!
	* \brief Gets the adress of the bit
	* \return Nullptr
	*
	* \see std::addressof
	*/

	template<typename Block, class Allocator>
	template<bool BadCall>
	void* Bitset::Bit::operator&() const
	{
		// The template is necessary to make it fail only when used
		static_assert(!BadCall, "It is impossible to take the address of a bit in a bitset");

		return nullptr;
	}

	/*!
	* \brief Converts this to bool
	* \return true if bit set to '1'
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator>::Bit::operator bool() const
	{
		return Test();
	}

	/*!
	* \brief Sets the bit to a value
	* \return A reference to this
	*
	* \param val Value of the bit
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::operator=(bool val)
	{
		return Set(val);
	}

	/*!
	* \brief Sets the bit to the value of another one
	* \return A reference to this
	*
	* \param bit Other bit
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::operator=(const Bit& bit)
	{
		return Set(bit);
	}

	/*!
	* \brief Performs the operator "OR" on this bit with a boolean
	* \return A reference to this
	*
	* \param val Value
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::operator|=(bool val)
	{
		// Version without branching:
		Set((val) ? true : Test());

		// With branching:
		/*
		if (val)
			Set();
		*/

		return *this;
	}

	/*!
	* \brief Performs the operator "AND" on this bit with a boolean
	* \return A reference to this
	*
	* \param val Value
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::operator&=(bool val)
	{
		// Version without branching:
		Set((val) ? Test() : false);

		// With branching:
		/*
		if (!val)
			Reset();
		*/

		return *this;
	}

	/*!
	* \brief Performs the operator "XOR" on this bit with a boolean
	* \return A reference to this
	*
	* \param val Value
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::operator^=(bool val)
	{
		// Version without branching:
		Set((val) ? !Test() : Test());

		// With branching:
		/*
		if (val)
			Flip();
		*/

		return *this;
	}

	/*!
	* \brief Performs the operator "RESET" on this bit with a boolean
	* \return A reference to this
	*
	* \param val Value
	*/

	template<typename Block, class Allocator>
	typename Bitset<Block, Allocator>::Bit& Bitset<Block, Allocator>::Bit::operator-=(bool val)
	{
		// Version without branching:
		Set((val) ? false : Test());

		// With branching:
		/*
		if (val)
			Reset();
		*/

		return *this;
	}

	/*!
	* \brief Compares two bitsets
	* \return true if the two bitsets are the same
	*
	* \param lhs First bitset to compare with
	* \param rhs Other bitset to compare with
	*
	* \remark If one is bigger, they are equal only if the largest has the last bit set to '0'
	*/

	template<typename Block, class Allocator>
	bool operator==(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		// The comparison uses that (uint8) 00001100 == (uint16) 00000000 00001100
		// and thus conserve this property
		const Bitset<Block, Allocator>& greater = (lhs.GetBlockCount() > rhs.GetBlockCount()) ? lhs : rhs;
		const Bitset<Block, Allocator>& lesser = (lhs.GetBlockCount() > rhs.GetBlockCount()) ? rhs : lhs;

		unsigned int maxBlockCount = greater.GetBlockCount();
		unsigned int minBlockCount = lesser.GetBlockCount();

		// We test the blocks in common to check the equality of bits
		for (unsigned int i = 0; i < minBlockCount; ++i)
		{
			if (lhs.GetBlock(i) != rhs.GetBlock(i))
				return false;
		}

		// Now we check for the blocks that only the biggest bitset owns, and to be equal, they must be set to '0'
		for (unsigned int i = minBlockCount; i < maxBlockCount; ++i)
			if (greater.GetBlock(i))
				return false;

		return true;
	}

	/*!
	* \brief Compares two bitsets
	* \return false if the two bitsets are the same
	*
	* \param lhs First bitset to compare with
	* \param rhs Other bitset to compare with
	*/

	template<typename Block, class Allocator>
	bool operator!=(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}

	/*!
	* \brief Compares two bitsets
	* \return true if the binary number represented by the lhs bitset is smaller
	*
	* \param lhs First bitset to compare with
	* \param rhs Other bitset to compare with
	*/

	template<typename Block, class Allocator>
	bool operator<(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		const Bitset<Block, Allocator>& greater = (lhs.GetBlockCount() > rhs.GetBlockCount()) ? lhs : rhs;
		const Bitset<Block, Allocator>& lesser = (lhs.GetBlockCount() > rhs.GetBlockCount()) ? rhs : lhs;

		unsigned int maxBlockCount = greater.GetBlockCount();
		unsigned int minBlockCount = lesser.GetBlockCount();

		// If the greatest bitset has a single bit active in a block outside the lesser bitset range, then it is greater
		for (unsigned int i = maxBlockCount; i > minBlockCount; ++i)
		{
			if (greater.GetBlock(i))
				return lhs.GetBlockCount() < rhs.GetBlockCount();
		}

		// Compare the common blocks
		for (unsigned int i = 0; i < minBlockCount; ++i)
		{
			unsigned int index = (minBlockCount - i - 1); // Compare from the most significant block to the less significant block
			if (lhs.GetBlock(index) < rhs.GetBlock(index))
				return true;
		}

		return false; // They are equal
	}

	/*!
	* \brief Compares two bitsets
	* \return true if the binary number represented by the lhs bitset is smaller or equal
	*
	* \param lhs First bitset to compare with
	* \param rhs Other bitset to compare with
	*/

	template<typename Block, class Allocator>
	bool operator<=(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		return lhs < rhs || lhs == rhs;
	}

	/*!
	* \brief Compares two bitsets
	* \return true if the binary number represented by the lhs bitset is greather
	*
	* \param lhs First bitset to compare with
	* \param rhs Other bitset to compare with
	*/

	template<typename Block, class Allocator>
	bool operator>(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		return rhs < lhs;
	}

	/*!
	* \brief Compares two bitsets
	* \return true if the binary number represented by the lhs bitset is greather or equal
	*
	* \param lhs First bitset to compare with
	* \param rhs Other bitset to compare with
	*/

	template<typename Block, class Allocator>
	bool operator>=(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		return rhs <= lhs;
	}

	/*!
	* \brief Performs the operator "AND" between two bitsets
	* \return The result of operator "AND"
	*
	* \param lhs First bitset
	* \param rhs Second bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> operator&(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		Bitset<Block, Allocator> bitset;
		bitset.PerformsAND(lhs, rhs);

		return bitset;
	}

	/*!
	* \brief Performs the operator "OR" between two bitsets
	* \return The result of operator "OR"
	*
	* \param lhs First bitset
	* \param rhs Second bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> operator|(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		Bitset<Block, Allocator> bitset;
		bitset.PerformsOR(lhs, rhs);

		return bitset;
	}

	/*!
	* \brief Performs the operator "XOR" between two bitsets
	* \return The result of operator "XOR"
	*
	* \param lhs First bitset
	* \param rhs Second bitset
	*/

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> operator^(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs)
	{
		Bitset<Block, Allocator> bitset;
		bitset.PerformsXOR(lhs, rhs);

		return bitset;
	}
}


namespace std
{
	/*!
	* \brief Swaps two bitsets, specialisation of std
	*
	* \param lhs First bitset
	* \param rhs Second bitset
	*/

	template<typename Block, class Allocator>
	void swap(Nz::Bitset<Block, Allocator>& lhs, Nz::Bitset<Block, Allocator>& rhs)
	{
		lhs.Swap(rhs);
	}
}

#ifdef NAZARA_COMPILER_MSVC
	// Reenable those warnings
	#pragma warning(default: 4146)
	#pragma warning(default: 4804)
#endif

#include <Nazara/Core/DebugOff.hpp>
