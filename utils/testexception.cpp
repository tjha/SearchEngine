#include "exception.hpp"
#include <assert.h>
#include <iostream>
#include <string.h>

int main()
	{
	try
		{
		throw dex::outOfRangeException();
		}
	catch (dex::exception& e)
		{
		assert(strcmp(e.what(), "Out of Range Exception") == 0);
		}

	try
		{
		throw dex::invalidArgumentException();
		}
	catch (dex::exception& e)
		{
		assert(strcmp(e.what(), "Invalid Argument Exception") == 0);
		}

	try
		{
		throw dex::outOfRangeException();
		}
	catch (dex::outOfRangeException& e)
		{
		assert(strcmp(e.what(), "Out of Range Exception") == 0);
		}

	try
		{
		throw dex::invalidArgumentException();
		}
	catch (dex::invalidArgumentException& e)
		{
		assert(strcmp(e.what(), "Invalid Argument Exception") == 0);
		}

	try
		{
		throw dex::outOfRangeException();
		}
	catch (dex::invalidArgumentException& e)
		{
		assert(false);
		}
	catch (dex::outOfRangeException& e)
		{
		assert(strcmp(e.what(), "Out of Range Exception") == 0);
		}

	return 0;
	}
