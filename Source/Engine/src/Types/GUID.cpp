#include "Types/GUID.hpp"

#include <combaseapi.h>


HYGUID::HYGUID()
	: m_bytes{ {0} }
{ }

HYGUID::HYGUID(const std::array<unsigned char, 16>& bytes)
	: m_bytes(bytes)
{ }


bool isValidHexChar(char ch)
{
	// 0-9
	if (ch >= '0' && ch <= '9')
		return true;

	// a-f
	if (ch >= 'a' && ch <= 'f')
		return true;

	// A-F
	if (ch >= 'A' && ch <= 'F')
		return true;

	return false;
}

unsigned char hexDigitToChar(char ch)
{
	// 0-9
	if (ch >= '0' && ch <= '9')
		return ch - '0';

	// a-f (10 to 15 in hexa)
	if (ch >= 'a' && ch <= 'f')
		return ch - ('a' - 10);

	// A-F (10 to 15 in hexa)
	if (ch >= 'A' && ch <= 'F')
		return ch - ('A' - 10);

	return 0;
}

unsigned char hexPairToChar(char a, char b)
{
	return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}

HYGUID::HYGUID(const std::string_view& string)
{
	char charOne = '\0';
	char charTwo = '\0';

	bool lookingForFirstChar = true;

	unsigned nextByte = 0;

	for (const char& ch : string)
	{
		if (ch == '-')
			continue;

		if (nextByte >= 16 || !isValidHexChar(ch))
		{
			// INVALID STRING : Too many characters or invalid one
			ResetToZero();
			return;
		}

		if (lookingForFirstChar)
		{
			charOne = ch;
			lookingForFirstChar = false;
		}
		else
		{
			charTwo = ch;
			auto byte = hexPairToChar(charOne, charTwo);
			m_bytes[nextByte++] = byte;
			lookingForFirstChar = true;
		}
	}

	if (nextByte < 16)
	{
		// Not enough characters
		ResetToZero();
		return;
	}
}

void HYGUID::ResetToZero()
{
	std::fill(m_bytes.begin(), m_bytes.end(), static_cast<unsigned char>(0));
}


HYGUID HYGUID::NewGUID()
{
	GUID newID;
	HRESULT result = CoCreateGuid(&newID);

	if (result != S_OK)
		return NewGUID();

	std::array<unsigned char, 16> bytes =
	{
		static_cast<unsigned char>((newID.Data1 >> 24) & 0xFF),
		static_cast<unsigned char>((newID.Data1 >> 16) & 0xFF),
		static_cast<unsigned char>((newID.Data1 >> 8) & 0xFF),
		static_cast<unsigned char>((newID.Data1) & 0xff),

		static_cast<unsigned char>((newID.Data2 >> 8) & 0xFF),
		static_cast<unsigned char>((newID.Data2) & 0xff),

		static_cast<unsigned char>((newID.Data3 >> 8) & 0xFF),
		static_cast<unsigned char>((newID.Data3) & 0xFF),

		static_cast<unsigned char>(newID.Data4[0]),
		static_cast<unsigned char>(newID.Data4[1]),
		static_cast<unsigned char>(newID.Data4[2]),
		static_cast<unsigned char>(newID.Data4[3]),
		static_cast<unsigned char>(newID.Data4[4]),
		static_cast<unsigned char>(newID.Data4[5]),
		static_cast<unsigned char>(newID.Data4[6]),
		static_cast<unsigned char>(newID.Data4[7])
	};

	return HYGUID{ std::move(bytes) };
}

const std::array<unsigned char, 16>& HYGUID::Bytes() const
{
	return m_bytes;
}


std::string HYGUID::ToString() const
{
	char one[10], two[6], three[6], four[6], five[14];

	snprintf(one, 10, "%02x%02x%02x%02x",
		m_bytes[0], m_bytes[1], m_bytes[2], m_bytes[3]);
	snprintf(two, 6, "%02x%02x",
		m_bytes[4], m_bytes[5]);
	snprintf(three, 6, "%02x%02x",
		m_bytes[6], m_bytes[7]);
	snprintf(four, 6, "%02x%02x",
		m_bytes[8], m_bytes[9]);
	snprintf(five, 14, "%02x%02x%02x%02x%02x%02x",
		m_bytes[10], m_bytes[11], m_bytes[12], m_bytes[13], m_bytes[14], m_bytes[15]);

	const std::string sep("-");
	std::string out(one);

	out += sep + two;
	out += sep + three;
	out += sep + four;
	out += sep + five;

	return out;
}

HYGUID::operator std::string() const
{
	return ToString();
}


bool HYGUID::operator==(const HYGUID& other) const
{
	return m_bytes == other.m_bytes;
}

bool HYGUID::operator!=(const HYGUID& other) const
{
	return !((*this == other));
}

bool HYGUID::operator<(const HYGUID& rhs) const
{
	return Bytes() < rhs.Bytes();
}