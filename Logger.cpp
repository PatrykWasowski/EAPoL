#include "Logger.h"

using namespace std;

Logger* Logger::pInstance = nullptr;

Logger::Logger () {
	bLineBegining = true;
	outStream = new std::ofstream (FILE_NAME, std::ofstream::out);
	clockBegin = clock ();
}

Logger::~Logger () {
	delete outStream;
}

void Logger::operator<<(endl_type endl) {
	bLineBegining = true;
	(*outStream) << endl;
}


Logger& Logger::getInstance () {
	if (!pInstance) {
		mutex mut;
		mut.lock ();
		if (!pInstance)
			pInstance = new Logger;
		mut.unlock ();
	}
	return *pInstance;
}