
inline Color3 Color3::operator+(const Color3& a) const
{
	return { a.r + this->r, a.g + this->g, a.b + this->b};
}

inline Color3 Color3::operator-(const Color3& a) const
{
	return { a.r - this->r, a.g - this->g, a.b - this->b};
}

inline Color3 Color3::operator*(float a) const
{
	return { this->r * a, this->g * a, this->b * a};
}

inline Color3 Color3::operator/(float a) const
{
	return { this->r / a, this->g / a, this->b / a};
}



inline Color4 Color4::operator+(const Color4& a) const
{
	return { a.r + this->r, a.g + this->g, a.b + this->b, a.a + this->a };
}

inline Color4 Color4::operator-(const Color4& a) const
{
	return { a.r - this->r, a.g - this->g, a.b - this->b, a.a - this->a };
}

inline Color4 Color4::operator*(float a) const
{
	return { this->r * a, this->g * a, this->b * a, this->a };
}

inline Color4 Color4::operator/(float a) const
{
	return { this->r / a, this->g / a, this->b / a, this->a };
}