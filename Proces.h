#ifndef PROCES_H_
#define PROCES_H_

typedef struct proces {
	char nume[21];
	int timp;
	int prioritate;
	int good; 
	int bad;
	struct proces* next;
}Proces;

void init (Proces **proces );

void enqueue(Proces **proces, char *nume, int timp, int prioritate, int good, int bad);

void dequeue(Proces **proces);

void show(Proces **proces, FILE *f2);

void tick(Proces **running, Proces **ready);

void tick2(Proces **running, Proces **ready);

void tick3(Proces **running, Proces **ready, int cuanta, int *contor);

int max_prioritate(Proces **ready);

Proces* nod_maxim(Proces **ready, int maxim);

void tick4(Proces **running, Proces **ready, int cuanta, int *contor);

void tick5(Proces **running, Proces **ready, int cuanta, int *contor);

void multipleadd(Proces **ready, char *string);

void add(Proces **ready, char *string);

void wait(Proces **running, Proces **waiting);

void wait5(Proces **running, Proces **waiting);

void event(Proces **waiting, Proces **ready, char *string);

void FCFS(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running);

void SJF(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running);

void RoundRobin(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running, int cuanta, int *contor);

void Planificare_prioritati(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running, int cuanta, int *contor);

void Planificare_dinamica(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running, int cuanta, int *contor);

#endif
