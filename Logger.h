#ifndef LOGGER
#define LOGGER
#include <fstream>
#include <mutex>

#define FILE_NAME "logger.log"

using std::chrono::system_clock;

/*
class created to log to file.
to get instance: Logger& log = Logger::getInstance(); //// REFERENCE IS IMPORTANT!
User Guide:
1)	Logger& log = Logger::getInstance(); 
	log << "example" << 1234 << std::endl;

2)	Logger::getInstance() << "example1" << 1234 << std::endl;

3)	not recomanded 
	Logger log = Logger::getInstance(); 
	log << "example"; 
	log << 1234;
	log << std::endl;

*/

class Logger {
private:
	
	Logger ();

	//variables needed to logging to file;
	std::ostream* outStream;
	//bool bLineBegining tells if logger starts to log in new line of file
	bool bLineBegining;
	//header od logger, in the begining of every line
	const std::string loggerHeader = " LOG: ";
	//This is the key: std::endl is a template function, and this is the signature of that function (For std::ostream).
	using endl_type = std::ostream&(std::ostream&);

	//pointer to only existing instance of Logger
	static Logger* pInstance;
	//clock, starts when logger is being created
	clock_t clockBegin;

public:
	/*
	Default destructor.
	*/
	~Logger ();
	/*
	getInstance calls constructor if logger is not created yet,
	returns poiter to instance of logger (pInstance)
	*/
	static Logger& getInstance ();
	//Overload for std::endl only:
	void operator<<(endl_type endl);

	//Overload for anything else:
	template<typename T>
	Logger& operator<< (const T& data);
};


template<typename T>
Logger& Logger::operator<< (const T& data) {
	if (bLineBegining) {
		bLineBegining = false;
		clock_t clockEnd;
		clockEnd = clock ();
		long myTime = (long) (clockEnd - clockBegin);
		(*outStream) << "[" << (myTime / 1000) << "." << (myTime % 1000) << "] " << '\t' << loggerHeader << '\t' << data; // TODO zmodyfikowac na potrzeby Supplicanta
	}
	else
		(*outStream) << " " << data;


	return Logger::getInstance ();
}

#endif