#include <atomic>
#include <thread>
//#include <assert.h>

#define TREADNUM 16

std::atomic<unsigned int> Tas = {0};
std::atomic<unsigned int> TTas = {0};
std::atomic<unsigned int> now_serving = {0};
std::atomic<unsigned int> next_ticket = {0};

//------------------------------------------------------------------------------------------
void Taslock(){
	int expected = 0;
	while (!Tas.compare_exchange_weak(expected, 1, std::memory_order_acquire)){
		expected = 0;
		std::this_thread::yield();
	}
}

void Tasunlock(){
	Tas.store(0, std::memory_order_release);
}
//------------------------------------------------------------------------------------------

void TTaslock(){
	int expected = 0;
	do {
		expected = 0;
		while (TTas.load(std::memory_order_relaxed)){
			std::this_thread::yield();
		}
	}
	while (!TTas.compare_exchange_weak(expected, 1, std::memory_order_acquire));
}

void TTasunlock(){
	TTas.store(0, std::memory_order_release);
}
//------------------------------------------------------------------------------------------

void Ticketlock(){
	
	const int ticket = next_ticket.fetch_add(1, std::memory_order_acquire);
	while (now_serving.load(std::memory_order_relaxed) != ticket){
		 __asm volatile("pause" :::);
	}
}

void Ticketunlock(){
	const int pass = now_serving.load(std::memory_order_relaxed) + 1;
	now_serving.store(pass, std::memory_order_release);
}
//------------------------------------------------------------------------------------------


int counter = 0;
#define MAXCOUNT 1000000

void routine(void (*lock)(), void (*unlock)()){
	for (int i = 0; i < MAXCOUNT; i++) {
		(*lock)();
		counter++;
		(*unlock)();
	}
}


long calcTime(int workers, void (*lock)(), void (*unlock)()) {
	auto start = std::chrono::high_resolution_clock::now();

	std::thread* threads[TREADNUM] = {};
    counter = 0;
    for (int i = 0; i < workers; i++) {
    	threads[i] = new std::thread(routine, lock, unlock);
    }
    for (int i = 0; i < numThreads; i++) {
        threads[i]->join();
        delete threads[i];
    }

	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main(int argc, char * argv[]) {

	for (int i = 1; i < TREADNUM; i++){
		long tas = calcTime(i, Taslock, Tasunlock);
		long ttas = calcTime(i, TTaslock, TTasunlock)
		long ticket = calcTime(i, Ticketlock, Ticketunlock)
		printf("%d %ld\n", i, dt);
		fflush(0);
	}

	return 0;
}