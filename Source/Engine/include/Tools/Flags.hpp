#pragma once

#include <string>
#include <bitset>

#include "Maths/Maths.hpp"

#include "Generated/Flags.rfkh.h"

constexpr std::size_t Max_Bitset = 64;

template <typename TEnum>
concept TBaseEnum = std::is_enum_v<TEnum>;

template <typename ...Args>
concept TArgsBaseEnum = std::is_enum_v<Args...>;

template <TBaseEnum TEnum>
class HY_CLASS() Flags
{
private:
	std::bitset<Max_Bitset> bits;

	/**
	@brief Check if a given flag is activated

	@param flag : flag to check

	@return : true if activated, false otherwise
	*/
	HY_METHOD()
		bool TestBitFromInt(uint64_t flag);

	/**
	@brief Activate a given flag

	@param flag : flag to activate
	*/
	HY_METHOD()
		void EnableFromInt(uint64_t flag);

	/**
	@brief Remove a given flag

	@param flag : flag to remove
	*/
	HY_METHOD()
		void DisableFromInt(uint64_t flag);

public:
	/**
	@brief Return current flag in binary string

	@param size : Amout of bits to return

	@return std::string : Binary string
	*/
	HY_METHOD()
	std::string ToString(std::size_t size = MAX_BITSET);

	/**
	@brief Reset the current flag (set to 0)
	*/
	void Reset();

	/**
	@brief Test if a given enum value is activated

	@param enumValue : Enum value to test

	@return bool : true if activated, false otherwise
	*/
	template <TBaseEnum TEnumVal>
	bool TestBit(TEnumVal enumValue) const;

	/**
	@brief Activate the given enum values

	@param enumValues... : Enum values to activate
	*/
	template <TArgsBaseEnum ...TEnumVal>
	void Enable(TEnumVal... enumValues);

	/**
	@brief Remove the given enum values

	@param enumValues... : Enum values to remove
	*/
	template <TArgsBaseEnum ...TEnumVal>
	void Disable(TEnumVal... enumValues);

	/**
	@brief Get the current value of the flag

	@return int64_t : Flag value
	*/
	HY_METHOD()
		int64_t GetFlagAsInt() const;

	/**
	@brief Set the current flag a new given value

	@param newValue : New flag value
	*/
	HY_METHOD()
		void SetFlagFromInt(uint64_t newValue);

	Flags_GENERATED
};

#include "Tools/Flags.inl"

File_Flags_GENERATED