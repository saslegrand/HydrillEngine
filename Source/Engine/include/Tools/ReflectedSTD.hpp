#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "EngineDLL.hpp"

#include "Generated/ReflectedSTD.rfkh.h"

/**
@basic class for all standard template classes.
Allow to use the same instance base
*/
class HY_CLASS() STDTemplateClass { STDTemplateClass_GENERATED };

struct HY_STRUCT() ENGINE_API HyString
{
private:
#pragma warning(disable:4251)
	std::string m_str = "";
#pragma warning(default:4251)

public:
	HyString() = default;

	HyString(const std::string & str)
		: m_str(str) {}
	HyString(const char* str)
		: m_str(str) {}
	HyString(std::string && str)
		: m_str(str) {}

	HyString& operator=(const std::string & str) noexcept
	{
		m_str.clear();

		m_str = str;

		return *this;
	}

	HyString& operator=(const char* str) noexcept
	{
		m_str.clear();

		m_str = str;

		return *this;
	}

	HyString& operator=(std::string&& str) noexcept
	{
		m_str.clear();

		m_str = std::move(str);

		return *this;
	}

	HyString& operator+=(const std::string & str) noexcept
	{
		m_str += str;

		return *this;
	}

	inline constexpr std::string operator+(const HyString& _Right) const
	{
		return m_str + _Right.string();
	}

	inline constexpr std::string operator+(const std::string& _Right) const
	{
		return m_str + _Right;
	}

	inline constexpr std::string operator+(const char* _Right) const
	{
		return m_str + _Right;
	}

	constexpr const char& operator[](size_t _Off) const noexcept
	{
		return m_str[_Off];
	}

	constexpr char& operator[](size_t _Off) noexcept
	{
		return m_str[_Off];
	}

	std::string string() noexcept
	{
		return m_str;
	}

	const std::string& string() const noexcept
	{
		return m_str;
	}

	constexpr size_t length() const
	{
		return m_str.length();
	}

	constexpr size_t size() const
	{
		return m_str.size();
	}

	constexpr size_t capacity() const
	{
		return m_str.capacity();
	}

	constexpr const char* c_str() const noexcept
	{
		return m_str.c_str();
	}

	constexpr char* data() noexcept
	{
		return m_str.data();
	}

	inline const char* data() const noexcept
	{
		return m_str.data();
	}

	constexpr bool empty() const noexcept
	{
		return m_str.empty();
	}

	constexpr int compare(const std::string& str) const
	{
		return m_str.compare(str);
	}

	constexpr int compare(const HyString& str) const
	{
		return m_str.compare(str.string());
	}

	HyString_GENERATED
};

/**
@brief Reflection class of the string from standard c++ library
*/
/*struct HY_STRUCT() HyString final : std::basic_string<char, std::char_traits<char>, std::allocator<char>>
{
	using std::basic_string<char, std::char_traits<char>, std::allocator<char>>::basic_string;
	using std::basic_string<char, std::char_traits<char>, std::allocator<char>>::operator=;

	HyString(const std::string& str)
		: std::basic_string<char, std::char_traits<char>, std::allocator<char>>(str.data())
	{
	}

	HyString_GENERATED
};*/

/**
@brief Reflection class of the vector from standard c++ library
*/
template <typename Type>
class HY_CLASS() HyVector final : public STDTemplateClass, public std::vector<Type>
{

public:
	/**
	@brief Give the size of the vector (see std::vector<>::size)

	@return size_t : size of the vector
	*/
	HY_METHOD()
		std::size_t size() const;

	/**
	@brief Return the ptr of an element given an index

	@param index : Index of the element to get

	@return void* : Ptr of the element
	*/
	HY_METHOD()
		void* GetElementAt(std::size_t index);

	/**
	@brief Erase an element at a given position

	@param index : Index of the element to erase
	*/
	HY_METHOD()
		void EraseAt(std::size_t index);

	/**
	@brief Swap two elements given an index and a direction (Unsafe method, index SHOULD not be 0 or vector size)

	@param index : Index of the element to swap
	@param swapDirection : Direction for the second element to swaps
	*/
	HY_METHOD()
		void SwapSigned(std::size_t index, int swapDirection);

	/**
	@brief Create a new element of template Type in the vector

	@return void* : Element added
	*/
	HY_METHOD()
		void* AddNewElement();

	HyVector_GENERATED
};

File_ReflectedSTD_GENERATED

#include "Tools/ReflectedSTD.inl"