// exception.hpp
// Classes for throwing exceptions
//
// 2019-09-23: File Created: Combsc
// 2019-09-25: Continued working: Combsc

namespace dex
	{

	// If you have an exception that does not fall under any exception made
	// so far, write a new implementation yourself.
	class exception
		{
		public:	
			exception( ){ }
			exception( const exception &other ){ }
			virtual ~exception(){ }
			exception &operator=( const exception &other )
				{
				return *this;
				}
			virtual const char *what( )
				{
				return "Exception Occurred";
				}
		};

	class outOfRangeException : public exception
		{
		public:
			virtual ~outOfRangeException( ){ };
			virtual const char *what( )
				{
				return "Out of Range Exception";
				}
		};

	class invalidArgumentException : public exception
		{
		public:
			virtual ~invalidArgumentException( ){ };
			virtual const char *what( )
				{
				return "Invalid Argument Exception";
				}
		};
	}

	
