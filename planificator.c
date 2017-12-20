#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Proces.h"

int main (int argc, char **argv) {
	FILE *f1, *f2;
	int cuanta;
	int contor = 0;
	Proces *running, *ready, *waiting;
	init(&running);
	init(&ready);
	init(&waiting);
	f1 = fopen(argv[1], "r");
	f2 = fopen(argv[2], "w");
	if (f1 == 0 || f2 == 0) {
		puts("Eroare la deschidere fisiere\n");
		return 1;
	}
	int tip_planificare;
	fscanf(f1, "%d", &tip_planificare);
	switch(tip_planificare) {
		case 1:
			FCFS(f1, f2, &ready, &waiting, &running);
			break;
		case 2:
			SJF(f1, f2, &ready, &waiting, &running);
			break;
		case 3:
			fscanf(f1, " %d", &cuanta);
			RoundRobin(f1, f2, &ready, &waiting, &running, cuanta, &contor);
			break;
		case 4:
			fscanf(f1, " %d", &cuanta);
			Planificare_prioritati(f1, f2, &ready, &waiting, &running, cuanta, &contor);
			break;
		case 5:
			fscanf(f1, "%d", &cuanta);
			Planificare_dinamica(f1, f2, &ready, &waiting, &running, cuanta, &contor);
	}
	fclose(f1);
	fclose(f2);
	free(running);
	free(ready);
	free(waiting);
	return 0;
}

//Initializeaza un proces
void init(Proces **proces) {
	*proces = NULL;
}

//Adaugare la final
void enqueue(Proces **proces, char *nume, int timp, int prioritate, int good, int bad) {
	Proces *new = (Proces*) malloc(sizeof(Proces));
	strcpy(new->nume, nume);
	new->timp = timp;
	new->prioritate = prioritate;
	new->good = good;
	new->bad = bad;
	new->next = NULL;
	if (*proces == NULL) {
		*proces = new; 
	}
	else {
		Proces *temp;
		temp = *proces;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = new;
	}
}

//Se scoate nodul de la inceputul listei
void dequeue(Proces **proces) {
	if (*proces != NULL) {
		Proces *temp;
		temp = *proces;
		*proces = (*proces)->next;
		free(temp);
	} 
	if (*proces == NULL) {
		free(*proces);
	}
}

//Functie pentru adaugarea din ready in running
void tick(Proces **running, Proces **ready) {
	if (*running != NULL) {
		(*running)->timp --;
		if ((*running)->timp == 0) {
			dequeue(running);
			if (*ready) { 
				enqueue(running, (*ready)->nume, (*ready)->timp, (*ready)->prioritate, 0, 0);
				dequeue(ready);
			}
		}
	}
	else {
		if (*ready) { 
			enqueue(running, (*ready)->nume, (*ready)->timp, (*ready)->prioritate, 0,0);
			dequeue(ready);
		}
	}
}
	
void tick2(Proces **running, Proces **ready) {
	Proces *temp, *temp2;
	if (*running != NULL) {
		(*running)->timp --;
		if ((*running)->timp == 0) {
			dequeue(running);
			if (*ready) { 
				int min = 100; //Timpul cel mai scurt de viata
				temp = *ready;
				temp2 = temp;
				Proces *minim; //Nodul cu timpul cel mai scurt de viata
				//Cazul in care nodul cu timpul minim este primul	
				if (temp->timp < min) {
					minim = temp;
					min = temp->timp;
				}
				//Daca nu este primul, parcurgem lista
				while (temp->next != NULL) {
					temp = temp->next;
					if (temp->timp < min) {
						minim = temp;
						min = temp->timp;
					}
				}
				enqueue(running, minim->nume, minim->timp, minim->prioritate, 0,0);
				//Cazul in care nodul este primul, pentru a-l elimina
				if (temp2->timp == min) {
					dequeue(ready);
				}
				else { 
					//Parcurgem pana la nodul cu timpul minim
					while (temp2->next->timp != min) {
						temp2 = temp2->next;
					}
					//daca nodul cu timpul minim nu este ultimul
					if (temp2->next->next) {
						temp2->next = temp2->next->next;
						free(minim);
					}
					else {
						temp2->next = NULL;
						free(minim);
					}
				}
			}
		}
	}
	else {
		if (*ready) { 
			int min = 100;
			temp = *ready;
			temp2 = temp;
			Proces *minim;
			if (temp->timp < min) {
				minim = temp;
				min = temp->timp;
			}
			while (temp->next != NULL) {
				temp = temp->next;
				if (temp->timp < min) {
					minim = temp;
					min = temp->timp;
				}
			}
			enqueue(running, minim->nume, minim->timp, minim->prioritate, 0,0);
			if (temp2->timp == min) {
				dequeue(ready);
			}
			else { 
				while (temp2->next->timp != min) {
					temp2 = temp2->next;
				}
				if (temp2->next->next) {
					temp2->next = temp2->next->next;
					free(minim);
				}
				else {
					temp2->next = NULL;
					free(minim);
				}
			}
		}
	}
}

void tick3(Proces **running, Proces **ready, int cuanta, int *contor) {
	if (*running != NULL) {
		(*running)->timp --;
		(*contor) ++; //Contorizeaza timpul petrecut in running
		if ((*running)->timp == 0) {
			dequeue(running);
			if (*ready) { 
				enqueue(running, (*ready)->nume, (*ready)->timp, (*ready)->prioritate, 0, 0);
				dequeue(ready);
				*contor = 0;
			}
		}
		else {
			//Cand contorul ajunge sa fie egal cu cuanta
			if  ((*contor) == cuanta) {
				//Se trimite in ready procesul din running
				enqueue(ready, (*running)->nume, (*running)->timp, (*running)->prioritate, 0, 0);
				dequeue(running);
				//Se trimite in running primul proces din ready
				enqueue(running, (*ready)->nume, (*ready)->timp, (*ready)->prioritate, 0, 0);
				dequeue(ready);
				*contor = 0;
			}
		}
	}
	else {
		if (*ready) { 
			enqueue(running, (*ready)->nume, (*ready)->timp, (*ready)->prioritate, 0, 0);
			dequeue(ready);
			*contor = 0;
		}
	}
}

//Returneaza cea mai mare prioritate a nodurilor
int max_prioritate(Proces **ready) {
	int maxim = 1;
	Proces *temp;
	temp = *ready;
	if (*ready) { 
		if (temp->prioritate > maxim) {
			maxim = temp->prioritate;
		}
		while (temp->next != NULL) {
			temp = temp->next;
			if (temp->prioritate > maxim) {
				maxim = temp->prioritate;
			}
		}
	}
	return maxim;
}

//Returneaza primul nod cu cea mai mare prioritate
Proces* nod_maxim(Proces **ready, int maxim) {
	Proces *temp;
	temp = *ready;
	while (temp->prioritate != maxim) {
		temp = temp->next;
	}
	return temp;
}

void tick4(Proces **running, Proces **ready, int cuanta, int *contor) {
	Proces *temp, *nod_max;
	int maxim = 1;
	if (*running != NULL) {
		(*running)->timp --;
		(*contor) ++;
		if ((*running)->timp == 0) {
			dequeue(running);	
			if (*ready) {
				temp = *ready;
				maxim = max_prioritate(ready); 
				nod_max = nod_maxim(ready, maxim);
				//Se adauga in running nodul cu cea mai mare prioritate in cazul in care timpul ajunge la 0
				enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, 0, 0);
				if (temp == nod_max) {
					dequeue(ready);
				}
				else { 
					while (temp->next != nod_max && temp != NULL) {
						temp = temp->next;
					}
					if (temp->next->next) {
						temp->next = temp->next->next;
						free(nod_max);
					}
					else {
						temp->next = NULL;
						free(nod_max);
					}
				}
				*contor = 0;
			}
		}
		else {
			if ((*contor) == cuanta) {
				//Se trimite in ready procesul din running
				enqueue(ready, (*running)->nume, (*running)->timp, (*running)->prioritate, 0,0);
				dequeue(running);
				if (*ready) { 
					temp = *ready;
					maxim = max_prioritate(ready); 
					nod_max = nod_maxim(ready, maxim);
					//Se adauga in running procesul cu cea mai mare prioritate
					enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, 0,0);
					if (temp == nod_max) {
						dequeue(ready);
					}
					else { 
						while (temp->next != nod_max && temp != NULL) {
							temp = temp->next;
						}
						if (temp->next->next) {
							temp->next = temp->next->next;
							free(nod_max);
						}
						else {
							temp->next = NULL;
							free(nod_max);
						}
					}
					*contor = 0;
				}
			}
			else {
				if (*ready) { 
					temp = *ready;
					maxim = max_prioritate(ready);
					//In cazul in care s-a adaugat in ready un proces cu prioritatea mai mare decat a celui din running
					if (maxim > (*running)->prioritate) { 
						nod_max = nod_maxim(ready, maxim);
						enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, 0,0);
						dequeue(running);
						if (temp == nod_max) {
							dequeue(ready);
						}
						else { 
							while (temp->next != nod_max && temp != NULL) {
								temp = temp->next;
							}
							if (temp->next->next) {
								temp->next = temp->next->next;
								free(nod_max);
							}
							else {
								temp->next = NULL;
								free(nod_max);
							}
						}
						*contor = 0;
					}
				}
			}
		}
	}
	else {
		if (*ready) { 
			temp = *ready;
			maxim = max_prioritate(ready); 
			nod_max = nod_maxim(ready, maxim);
			enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, 0,0);
			if (temp == nod_max) {
				dequeue(ready);
			}
			else { 
				while (temp->next != nod_max && temp != NULL) {
					temp = temp->next;
				}
				if (temp->next->next) {
					temp->next = temp->next->next;
					free(nod_max);
				}
				else {
					temp->next = NULL;
					free(nod_max);
				}
			}
			*contor = 0;
		}
	}
}

void tick5(Proces **running, Proces **ready, int cuanta, int *contor) {
	Proces *temp, *nod_max;
	//Valoare initiala a prioritatii maxime este cea mai mica prioritate posibila
	int maxim = 1;
	if (*running != NULL) {
		(*running)->timp --;
		(*contor) ++;
		if ((*running)->timp == 0) {
			//La terminarea timpului, se scoate din running procesul
			dequeue(running);	
			if (*ready) {
				temp = *ready;
				//Se cauta prioritatea maxima in ready
				maxim = max_prioritate(ready); 
				//Se ia nodul din ready cu prioritatea maxima gasita
				nod_max = nod_maxim(ready, maxim);
				enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, nod_max->good, nod_max->bad);
				//In cazul in care nodul cu prioritatea maxima este primul, se scoate din ready
				if (temp == nod_max) {
					dequeue(ready);
				}
				else { 
					//Altfel, se parcurge lista pana este gasit
					while (temp->next != nod_max && temp != NULL) {
						temp = temp->next;
					}
					if (temp->next->next) {
						temp->next = temp->next->next;
						free(nod_max);
					}
					else {
						//Cazul in care nodul este ultimul
						temp->next = NULL;
						free(nod_max);
					}
				}
				*contor = 0;
			}
		}
		else {
			if ((*contor) == cuanta) {
				(*running)->bad ++;
				//Daca iese de 2 ori din running procesul
				if ((*running)->bad == 2){
					(*running)->prioritate --;
					(*running)->bad = 0;
				}
				enqueue(ready, (*running)->nume, (*running)->timp, (*running)->prioritate, (*running)->good, (*running)->bad);
				dequeue(running);
				if (*ready) { 
					temp = *ready;
					maxim = max_prioritate(ready); 
					nod_max = nod_maxim(ready, maxim);
					enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, nod_max->good, nod_max->bad);
					if (temp == nod_max) {
						dequeue(ready);
					}
					else { 
						while (temp->next != nod_max && temp != NULL) {
							temp = temp->next;
						}
						if (temp->next->next) {
							temp->next = temp->next->next;
							free(nod_max);
						}
						else {
							temp->next = NULL;
							free(nod_max);
						}
					}
					*contor = 0;
				}
			}
			else {
				if (*ready) { 
					temp = *ready;
					maxim = max_prioritate(ready);
					if (maxim > (*running)->prioritate) { 
						nod_max = nod_maxim(ready, maxim);
						(*running)->bad ++;
						//Daca iese de 2 ori din running
						if ((*running)->bad == 2) {
							(*running)->prioritate --;
							(*running)->bad = 0;
						}
						enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, nod_max->good, nod_max->bad);
						dequeue(running);
						if (temp == nod_max) {
							dequeue(ready);
						}
						else { 
							while (temp->next != nod_max && temp != NULL) {
								temp = temp->next;
							}
							if (temp->next->next) {
								temp->next = temp->next->next;
								free(nod_max);
							}
							else {
								temp->next = NULL;
								free(nod_max);
							}
						}
						*contor = 0;
					}
				}
			}
		}
	}
	else {
		if (*ready) { 
			temp = *ready;
			maxim = max_prioritate(ready); 
			nod_max = nod_maxim(ready, maxim);
			enqueue(running, nod_max->nume, nod_max->timp, nod_max->prioritate, nod_max->good, nod_max->bad);
			if (temp == nod_max) {
				dequeue(ready);
			}
			else { 
				while (temp->next != nod_max && temp != NULL) {
					temp = temp->next;
				}
				if (temp->next->next) {
					temp->next = temp->next->next;
					free(nod_max);
				}
				else {
					temp->next = NULL;
					free(nod_max);
				}
			}
			*contor = 0;
		}
	}
}

//Se creeaza un proces din comanda primita si se adauga in ready
void add(Proces **ready, char *string) {
	char nume[20];
	int timp, prioritate;
	//Se trece peste "a"
	string = string + 2;
	sscanf(string, "%s %d %d", nume, &timp, &prioritate);
	enqueue(ready, nume, timp, prioritate, 0, 0);
}

//Se creeaza mai multe procese din comanda primita adaugate in ready
void multipleadd(Proces **ready, char *string) {
	char nume[20];
	int timp, prioritate, nr_caractere;
	//Se desparte stringul primit in cuvinte
	//Se trece peste "ma"
	string = string + 3;
	while (*string != '\0') {
		sscanf(string, "%s %d %d %n", nume, &timp, &prioritate, &nr_caractere);
		enqueue(ready, nume, timp, prioritate, 0, 0);
		string = string + nr_caractere;
	}
}

//Se trimite in coada waiting procesul din running
void wait(Proces **running, Proces **waiting) {
	if (*running) {
		enqueue(waiting, (*running)->nume, (*running)->timp, (*running)->prioritate, (*running)->good, (*running)->bad);
		dequeue(running);
	}
}

void wait5(Proces **running, Proces **waiting) {
	if (*running) {
		(*running)->good ++;
		//Daca se duce de 2 ori in waiting procesul
		if ((*running)->good == 2) {
			(*running)->prioritate ++;
			(*running)->good = 0;
		}
		enqueue(waiting, (*running)->nume, (*running)->timp, (*running)->prioritate, (*running)->good, (*running)->bad);
		dequeue(running);

	}
}

//Se cauta procesul cu numele respectiv in waiting si se muta in ready
void event(Proces **waiting, Proces **ready, char *string) {
	char nume[20];
	sscanf(string, "%*s %s", nume);
	//Se pastreaza numele in variabila pentru a cauta procesul in lista
	Proces *temp;
	if (*waiting) {
		temp = *waiting;
		if (strcmp(temp->nume,nume) == 0) {
			Proces *aux;
			aux = *waiting;
			*waiting = (*waiting)->next;
			enqueue(ready, aux->nume, aux->timp, aux->prioritate, aux->good, aux->bad);
			free(aux);
		}
		else {
			while (temp->next != NULL && strcmp(temp->next->nume,nume) != 0) {
				temp = temp->next;
			}
			if (strcmp(temp->next->nume,nume) == 0) {
				Proces *aux;
				aux = temp->next;
				temp->next = temp->next->next;
				enqueue(ready, aux->nume, aux->timp, aux->prioritate, aux->good, aux->bad);
				free(aux);
			}
		}
	}
}

//Afisare 
void show(Proces **proces, FILE *f2) {
	if (*proces != NULL) {
		fprintf(f2, "%s %d\n", (*proces)->nume, (*proces)->timp);
	}
}

void FCFS(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running) {
	char comanda[100];
	while (fgets(comanda, 100, f1)) {
		if (comanda[strlen(comanda) -1] == '\n') {
			comanda[strlen(comanda) - 1] = '\0';
		}
		//In functie de prima litera din comanda, se alege ce se face cu procesul
		switch (comanda[0]) {
			case 'a':
				add(ready, comanda);
				tick(running, ready);
				break;
			case 't':
				tick(running, ready);
				break;
			case 'e':
				event(waiting, ready, comanda);
				tick(running, ready);
				break;
			case 'w':
				wait(running, waiting);
				tick(running, ready);
				break;
			case 's':
				if (*running == NULL) {
					fprintf(f2, "\n");
				}
				else {
					show(running, f2);
				}
				break;
			case 'm' :
				multipleadd(ready, comanda);
				tick(running, ready);
				break;
		}
	}
}

void SJF(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running) {
	char comanda[100];
	while (fgets(comanda, 100, f1)) {
		if (comanda[strlen(comanda) -1] == '\n'){
			comanda[strlen(comanda) - 1] = '\0';
		}
		switch (comanda[0]) {
			case 'a':
				add(ready, comanda);
				tick2(running, ready);
				break;
			case 't':
				tick2(running, ready);
				break;
			case 'e':
				event(waiting, ready, comanda);
				tick2(running, ready);
				break;
			case 'w':
				wait(running, waiting);
				tick2(running, ready);
				break;
			case 's':
				if (*running == NULL) {
					fprintf(f2, "\n");
				}
				else {
					show(running, f2);
				}
				break;
			case 'm' :
				multipleadd(ready, comanda);
				tick2(running, ready);
				break;
		}
	}
}

void RoundRobin(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running, int cuanta, int *contor) {
	char comanda[100];
	while (fgets(comanda, 100, f1)) {
		if (comanda[strlen(comanda) -1] == '\n') {
			comanda[strlen(comanda) - 1] = '\0';
		}
		switch (comanda[0]) {
			case 'a':
				add(ready, comanda);
				tick3(running, ready, cuanta, contor);
				break;
			case 't':
				tick3(running, ready, cuanta, contor);
				break;
			case 'e':
				event(waiting, ready, comanda);
				tick3(running, ready, cuanta, contor);
				break;
			case 'w':
				wait(running, waiting);
				tick3(running, ready, cuanta, contor);
				break;
			case 's':
				if (*running == NULL) {
					fprintf(f2, "\n");
				}
				else {
					show(running, f2);
				}
				break;
			case 'm' :
				multipleadd(ready, comanda);
				tick3(running, ready, cuanta, contor);
				break;
		}
	}
}

void Planificare_prioritati(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running, int cuanta, int *contor) {
	char comanda[100];
	while (fgets(comanda, 100, f1)) {
		if (comanda[strlen(comanda) -1] == '\n') {
			comanda[strlen(comanda) - 1] = '\0';
		}
		switch (comanda[0]) {
			case 'a':
				add(ready, comanda);
				tick4(running, ready, cuanta, contor);
				break;
			case 't':
				tick4(running, ready, cuanta, contor);
				break;
			case 'e':
				event(waiting, ready, comanda);
				tick4(running, ready, cuanta, contor);
				break;
			case 'w':
				wait(running, waiting);
				tick4(running, ready, cuanta, contor);
				break;
			case 's':
				if (*running == NULL) {
					fprintf(f2, "\n");
				}
				else {
					show(running, f2);
				}
				break;
			case 'm' :
				multipleadd(ready, comanda);
				tick4(running, ready, cuanta, contor);
				break;
		}
	}
}

void Planificare_dinamica(FILE *f1, FILE *f2, Proces **ready, Proces **waiting, Proces **running, int cuanta, int *contor) {
	char comanda[100];
	while (fgets(comanda, 100, f1)) {
		if (comanda[strlen(comanda) -1] == '\n') {
			comanda[strlen(comanda) - 1] = '\0';
		}
		switch (comanda[0]) {
			case 'a':
				add(ready, comanda);
				tick5(running, ready, cuanta, contor);
				break;
			case 't':
				tick5(running, ready, cuanta, contor);
				break;
			case 'e':
				event(waiting, ready, comanda);
				tick5(running, ready, cuanta, contor);
				break;
			case 'w':
				wait5(running, waiting);
				tick5(running, ready, cuanta, contor);
				break;
			case 's':
				if (*running == NULL) {
					fprintf(f2, "\n");
				}
				else {
					show(running, f2);
				}
				break;
			case 'm' :
				multipleadd(ready, comanda);
				tick5(running, ready, cuanta, contor);
				break;
		}
	}
}