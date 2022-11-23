/*
	CURITIBA 11/2022
	UNIVERSIDADE POSITIVO
	PROGRAMACAO CONCORRENTE/DESENVOLVIMENTO DE SISTEMAS

	TRABALHO 2
	- ADAPTACAO DO "PROBLEMA DO BANHEIRO UNICO" (VER DETALHES NA ESPECIFICACAO)

	TAREFA
	- COMPLETAR/COMPLEMENTAR AS FUNCOES "CAT" E "DOG" NO FINAL DESTE ARQUIVO

	REGRAS
	- VOCE PODE ADICIONAR CODIGO A VONTADE DESDE QUE SEJA A PARTIR DA LINHA COM O COMENTARIO "TODO"
	- VOCE PODE INCLUIR CABECALHOS A VONTADE
	- NADA DO QUE ESTA ESCRITO PODE SER APAGADO

	INFORMACOES IMPORTANTES
	- A ACAO "EATING" EH CRITICA, A ACAO "PLAYING" EH NAO-CRITICA
	- DEVE HAVER EXCLUSAO MUTUA ENTRE GATOS E CACHORROS NA AREA DE COMIDA
	- O NUMERO DE PETS NA AREA DE COMIDA NAO PODE ULTRAPASAR O VALOR DA MACRO "MAX_PETS"
	- NAO DEVE HAVER STARVATION DE GATOS OU CACHORROS

	DICAS
	- HA UMA CLASSE "SEMAFORO" DISPONIVEL PARA USO
	- LEMBRE-SE DE COMPILAR PARA C++11 (-std=c++11) OU SUPERIOR
	- A COMPREENSAO DO CODIGO EH PARTE DO TRABALHO
*/

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class semaphore
{
	long count;
	std::mutex mtx;
	std::condition_variable cv;

public:

	semaphore(long const c = 0) : count(c) {}

	semaphore(semaphore const&) = delete;
	semaphore(semaphore&&) = default;
	semaphore& operator=(semaphore const&) = delete;
	semaphore& operator=(semaphore&&) = default;
	~semaphore() = default;

	void acquire() //aka "wait", "down", "p"
	{
		auto lock = std::unique_lock<std::mutex>(mtx);
		while (!count) cv.wait(lock);
		--count;
	}

	void release() //aka "signal", "up", "v"
	{
		auto lock = std::unique_lock<std::mutex>(mtx);
		++count;
		cv.notify_one();
	}
};

#define MAX_PETS 16
#define MAX_SLEEP_US 4
#define NUM_THREADS 100

void do_stuff(int const id, char const* kind, char const* action)
{
	std::printf("pet #%d (%s) started %s...\n", id, kind, action);
	std::this_thread::sleep_for(std::chrono::microseconds(std::rand() % MAX_SLEEP_US));
	std::printf("pet #%d (%s) stopped %s...\n", id, kind, action);
}

void cat(int const);
void dog(int const);

int main()
{
	auto pets = std::vector<std::thread>(NUM_THREADS);

	for (int i = 0; i < pets.size(); ++i)
	{
		pets.at(i) = std::thread(i % 2 ? cat : dog, i);
	}

	for (int i = 0; i < pets.size(); ++i)
	{
		pets.at(i).join();
	}

	return 0;
}

//////////////////////////////////////////////////////////////
//TODO
//////////////////////////////////////////////////////////////






semaphore dogSemaphore = 1;
semaphore catSemaphore = 1;
int countRoom = 0;
int kind = 0;
std::mutex Catmtx;
std::mutex Dogmtx;


void cat(int const id)
{
	while (true)
	{
		int mykind = 1;

		do_stuff(id, "cat", "playing");

		if (countRoom < MAX_PETS)
		{	
			Catmtx.lock();
			if (countRoom > 0 && kind == 2)
				dogSemaphore.acquire();
			Catmtx.unlock();

			catSemaphore.acquire();


			Catmtx.lock();
			countRoom++;
			printf("countC = %d\n", countRoom);
			kind = mykind;
			if (countRoom == 1) dogSemaphore.acquire();
			Catmtx.unlock();

			catSemaphore.release();

			do_stuff(id, "cat", "eating");

			Catmtx.lock();
			countRoom--;
			if (countRoom == 0) dogSemaphore.release();
			Catmtx.unlock();
	
		}
		
	}
}

void dog(int const id)
{
	while (true)
	{
		int mykind = 2;
		do_stuff(id, "dog", "playing");
		

		if (countRoom < MAX_PETS)
		{
			Dogmtx.lock();
			if (countRoom > 0 && kind == 1)
				catSemaphore.acquire();
			Dogmtx.unlock();

			dogSemaphore.acquire();


			Dogmtx.lock();
			printf("countD = %d\n", countRoom);
			countRoom++;
			kind = mykind;
			if (countRoom == 1) catSemaphore.acquire();
			Dogmtx.unlock();

			dogSemaphore.release();

			do_stuff(id, "dog", "eating");

			Dogmtx.lock();
			countRoom--;
			if (countRoom == 0) catSemaphore.release();
			Dogmtx.unlock();
		}

	}
}

