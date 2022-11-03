
template <TBaseEnum TEnum>
bool Flags<TEnum>::TestBitFromInt(uint64_t flag)
{
	return static_cast<uint64_t>(bits.to_ullong()) & flag;
}

template <TBaseEnum TEnum>
void Flags<TEnum>::EnableFromInt(uint64_t flag)
{
	bits |= flag;
}

template <TBaseEnum TEnum>
void Flags<TEnum>::DisableFromInt(uint64_t flag)
{
	bits &= ~flag;
}

template <TBaseEnum TEnum>
int64_t Flags<TEnum>::GetFlagAsInt() const
{
	return bits.to_ullong();
}

template <TBaseEnum TEnum>
void Flags<TEnum>::SetFlagFromInt(uint64_t newValue)
{
	bits = newValue;
}

template <TBaseEnum TEnum>
template <TBaseEnum TEnumVal>
bool Flags<TEnum>::TestBit(TEnumVal enumValue) const
{
	return static_cast<int64_t>(bits.to_ullong()) & static_cast<int64_t>(enumValue);
}

template <TBaseEnum TEnum>
template <TArgsBaseEnum ...TEnumVal>
void Flags<TEnum>::Enable(TEnumVal... enumValues)
{
	for (auto arg : { enumValues... })
		bits |= static_cast<int64_t>(arg);
}

template <TBaseEnum TEnum>
template <TArgsBaseEnum ...TEnumVal>
void Flags<TEnum>::Disable(TEnumVal... enumValues)
{
	for (auto arg : { enumValues... })
		bits &= ~static_cast<int64_t>(arg);
}

template <TBaseEnum TEnum>
inline std::string Flags<TEnum>::ToString(std::size_t size)
{
	std::string str = bits.to_string();
	std::reverse(str.begin(), str.end());

	size = Maths::Clamp(size, static_cast<std::size_t>(0), Max_Bitset);
	str.resize(size);

	return str;
}

template <TBaseEnum TEnum>
inline void Flags<TEnum>::Reset()
{
	bits.reset();
}