// typeTraits.hpp
// DIY implementation of some templating utilities from the STL's type_traits header. Implementtions are mostly taken
// from https://en.cppreference.com/
//
// 2019-11-04: File created: jasina

#ifndef DEX_TYPE_TRAITS
#define DEX_TYPE_TRAITS

#include <cstddef>

namespace dex
	{
	// CTRLF: conditional
	template < bool boolean, class classIfTrue, class classIfFalse >
	struct conditional
		{
		typedef classIfTrue type;
		};

	template < class classIfTrue, class classIfFalse >
	struct conditional< false, classIfTrue, classIfFalse >
		{
		typedef classIfFalse type;
		};

	// CTRLF: enableIf
	template < bool boolean, class classIfTrue = void >
	struct enableIf { };

	template < class classIfTrue >
	struct enableIf< true, classIfTrue >
		{
		typedef classIfTrue type;
		};


	// CTRLF: isIntegral
	template < class T >
	struct isIntegral
		{
		static const bool value = false;
		};

	template < >
	struct isIntegral< bool >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const bool >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< char >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< unsigned char >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const char >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const unsigned char >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< char16_t >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const char16_t >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< char32_t >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const char32_t >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< wchar_t >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const wchar_t >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< short >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< unsigned short >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const short >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const unsigned short >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< int >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< unsigned >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const int >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const unsigned >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< unsigned long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const unsigned long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< long long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< unsigned long long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const long long >
		{
		static const bool value = true;
		};
	template < >
	struct isIntegral< const unsigned long long >
		{
		static const bool value = true;
		};
	}

#endif
