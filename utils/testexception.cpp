#include "exception.hpp"
#include <iostream>

int main()
	{
	try
		{
		throw dex::outOfRangeException();
		}
	catch (dex::exception& e)
		{
		std::cout << e.what() << " caught\n";
		}

	try
		{
		throw dex::invalidArgumentException();
		}
	catch (dex::exception& e)
		{
		std::cout << e.what() << " caught\n";
		}

	try
		{
		throw dex::outOfRangeException();
		}
	catch (dex::outOfRangeException& e)
		{
		std::cout << e.what() << " caught\n";
		}

	try
		{
		throw dex::invalidArgumentException();
		}
	catch (dex::invalidArgumentException& e)
		{
		std::cout << e.what() << " caught\n";
		}

	return 0;
	}
