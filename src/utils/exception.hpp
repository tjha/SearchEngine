// exception.hpp
// Classes for throwing exceptions
//
// 2019-10-17: Add include guard: jasina
// 2019-09-23: File Created: Combsc
// 2019-09-25: Continued working: Combsc

#ifndef DEX_EXCEPTION
#define DEX_EXCEPTION

namespace dex
	{

	// If you have an exception that does not fall under any exception made
	// so far, write a new implementation yourself.
	class exception
		{
		public:
			virtual const char *what( )
				{
				return "Exception";
				}
		};
	

	class outOfRangeException : public exception
		{
		public:
			virtual const char *what( )
				{
				return "Out of Range Exception";
				}
		};

	class invalidArgumentException : public exception
		{
		public:
			virtual const char *what( )
				{
				return "Invalid Argument Exception";
				}
		};
	
	class fileWriteException : public exception
		{
		public:
			virtual const char *what( )
				{
				return "File Write Exception";
				}
		};
	}

#endif
