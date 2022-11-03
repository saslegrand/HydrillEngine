#pragma once

#include <array>
#include <iostream>

#include "EngineDLL.hpp"

// Just a minimal adaptation of https://github.com/graeme-hill/crossguid

/*
@brief It's a Global Unique ID, meant to link data 
*/
class ENGINE_API HYGUID
{
private:
	std::array<unsigned char, 16> m_bytes = { 0 };

public:
	HYGUID();
	HYGUID(HYGUID const&) = default;
	HYGUID(HYGUID&&     ) = default;
	HYGUID(const std::array<unsigned char, 16>& bytes);
	HYGUID(const std::string_view& string);
	~HYGUID() = default;

private:
	void ResetToZero();
	std::string ToString() const;

public:
	static HYGUID NewGUID();

	const std::array<unsigned char, 16>& Bytes() const;

	operator std::string() const;

	bool operator==(const HYGUID& other) const;
	bool operator!=(const HYGUID& other) const;
	bool operator<(const HYGUID& other) const;

	HYGUID& operator=(HYGUID const&) = default;
	HYGUID& operator=(HYGUID&&     ) = default;
};

namespace details
{
	template <typename...> struct hash;

	template<typename T>
	struct hash<T> : public std::hash<T>
	{
		using std::hash<T>::hash;
	};


	template <typename T, typename... Rest>
	struct hash<T, Rest...>
	{
		inline std::size_t operator()(const T& v, const Rest&... rest) {
			std::size_t seed = hash<Rest...>{}(rest...);
			seed ^= hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}

namespace std
{
	template<>
	struct hash<HYGUID>
	{
		std::size_t operator()(const HYGUID& guid) const
		{
			const uint64_t* p = reinterpret_cast<const uint64_t*>(guid.Bytes().data());
			return details::hash<uint64_t, uint64_t>{}(p[0], p[1]);
		}
	};
}