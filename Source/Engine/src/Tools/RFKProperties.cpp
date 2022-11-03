#include "Generated/RFKProperties.rfks.h"

PRange::PRange(float min, float max) noexcept
{
	bool isHigher = max > min;
	maxValue = isHigher ? max : min;
	minValue = isHigher ? min : max;
}

PRange::PRange(float value) noexcept
	: minValue(-Maths::Abs(value)), maxValue(Maths::Abs(value))
{}



PHeader::PHeader(const char* title) noexcept
	: m_title(title)
{

}

std::string PHeader::GetTitle() const
{
	return m_title;
}