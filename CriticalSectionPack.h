#ifndef CRTSECPCK
#define	CRTSECPCK
#include <mutex>
#include <condition_variable>
struct CriticalSectionPack{
	std::mutex mtx;
	std::condition_variable cv;
	bool ready = false;
};
#endif