#include <atomic>
#include <thread>

#define TREADNUM 15

std::atomic<unsigned int> Tas = {0};
std::atomic<unsigned int> TTas = {0};
std::atomic<unsigned int> now_serving = {0};
std::atomic<unsigned int> next_ticket = {0};

//------------------------------------------------------------------------------------------
void Taslock(){
	unsigned int expected = 0;
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
	unsigned int expected = 0;
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
#define MAXCOUNT 360360 //делится на все чичсла от 1 до 15

void routine(void (*lock)(), void (*unlock)(), int maxc){
	for (int i = 0; i < maxc; i++) {
		(*lock)();
		for (int i = 0; i < 100; ++i)
		{
			void* a = malloc((i+25) * 200);
			free(a);
		}
		counter++;
		(*unlock)();
	}
}


long measure(int workers, void (*lock)(), void (*unlock)()) {
	counter = 0;
	std::thread* threads[TREADNUM] = {};
	auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < workers; i++) {
    	threads[i] = new std::thread(routine, lock, unlock, MAXCOUNT/workers);
    }
    for (int i = 0; i < workers; i++) {
        threads[i] -> join();
        delete threads[i];
    }
	auto end = std::chrono::high_resolution_clock::now();
	if (counter != MAXCOUNT){
		printf("SMTH WENT WRONG");
	}
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main(int argc, char * argv[]) {

	FILE* ftas = fopen("tas", "w");
	FILE* fttas = fopen("ttas", "w");
	FILE* fticket = fopen("ticket", "w");

	for (int i = 1; i <= TREADNUM; i++){
		
		printf("начали раунд %i\n", i);
		long tas = measure(i, Taslock, Tasunlock);

		printf("померяли tas \n");
		long ttas = measure(i, TTaslock, TTasunlock);
		printf("померяли ttas \n");

		long ticket = measure(i, Ticketlock, Ticketunlock);
		printf("померяли ticket \n");

		fprintf(ftas,"%d %ld\n", i, tas);
		fprintf(fttas,"%d %ld\n", i, ttas);
		fprintf(fticket,"%d %ld\n", i, ticket);
		printf("закончили раунд %i\n", i);
	}

	fclose(ftas);
	fclose(fttas);
	fclose(fticket);

	return 0;
}