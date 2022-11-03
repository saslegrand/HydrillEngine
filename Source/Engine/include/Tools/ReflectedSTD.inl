
//inline HyString::HyString(std::string const& copy) 
//	: std::string(copy.data())
//{
//}

template <typename Type>
std::size_t HyVector<Type>::size() const
{
	return std::vector<Type>::size();
}

template <typename Type>
void* HyVector<Type>::GetElementAt(std::size_t index)
{
	Type* type = &std::vector<Type>::operator[](index);

	return type;
}

template <typename Type>
void* HyVector<Type>::AddNewElement()
{
	return &std::vector<Type>::emplace_back(Type());
}

template <typename Type>
void HyVector<Type>::EraseAt(std::size_t index)
{
	std::vector<Type>::erase(std::vector<Type>::begin() + index);
}

template <typename Type>
void HyVector<Type>::SwapSigned(std::size_t index, int swapDirection)
{
	std::swap(std::vector<Type>::operator[](index), std::vector<Type>::operator[](index + swapDirection));
}