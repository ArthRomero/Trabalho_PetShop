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
#include <Windows.h>

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
int countCat = 0;
int countDog = 0;
std::mutex Catmtx;
std::mutex Dogmtx;

list list1;
semaphore catSemaphore;
semaphore dogSemaphore;
*/

/*int verification(int id, int kind)
{
	
	int count = list1.getCount();

	

	if (count < MAX_PETS)
	{
		if (count == 0)
		{
			list1.setKind(kind);
			return 1;
		}
		
		
		if (kind != list1.getKind())
		{
			if (kind == 1)
			{
				return 0;
			}
			else if (kind == 2)
			{
				return 0;
			}
		}
			
		return 1;
	}
	return 0;
}
*/

std::mutex mtx;

/*
A ideia era fazer por semaforo, mais não entendemos muito sobre, até tentamos mas não estava dando certo, e achamos essa solução, 
onde apenas um pet irá comer por vez, dessa forma não haverá disputa entre cães e gatos.

Deixe comentado tanto a classe como a função que estava sendo usada na tentativa da solução por semáforo.

A ideia era que teria uma classe lista para guardar o tipo do animal, e quantos animais haviam dentro da sala, 
e uma função para fazer a verificação se o tipo do animal era o mesmo, e se ainda havia espaço dentro da sala.
*/
void cat(int const id)
{
	while (true)
	{
		do_stuff(id, "cat", "playing");
		if (countCat < MAX_PETS)
		{
			catSemaphore.acquire();

			Catmtx.lock();
			countCat++;
			if (countCat == 1) dogSemaphore.acquire();
			Catmtx.unlock();
			
			catSemaphore.release();
			
			do_stuff(id, "cat", "eating");

			Catmtx.lock();
			countCat--;
			if (countCat == 0) dogSemaphore.release();
			Catmtx.unlock();
	
		}
		
	}
}

void dog(int const id)
{
	while (true)
	{
		do_stuff(id, "dog", "playing");
		
		if (countDog < MAX_PETS)
		{
			dogSemaphore.acquire();
			Dogmtx.lock();
			countDog++;
			if (countDog == 1) catSemaphore.acquire();
			Dogmtx.unlock();

			dogSemaphore.release();

			do_stuff(id, "dog", "eating");
		
			Dogmtx.lock();
			countDog--;
			if (countDog == 0) catSemaphore.release();
			Dogmtx.unlock();
		}
	
	}
}

