#ifndef HEADER_D1187EA6_D4D4_11E9_B4C1_784F43782D09
#define HEADER_D1187EA6_D4D4_11E9_B4C1_784F43782D09

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

template<typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> data)
{
	std::sort(std::begin(data),std::end(data));
	std::copy(std::begin(data),std::end(data),std::ostream_iterator<T>(out, "\n"));
	return out;
}

#endif // HEADER_D1187EA6_D4D4_11E9_B4C1_784F43782D09
