//Abagiu Ioan-Razvan 311CD

#include <stdio.h>
#include "WearhouseManager.h"

Package *create_package(long priority, const char* destination){

	Package *package = malloc(sizeof(Package));
	package->priority = priority;

	if(!package)
		return NULL;

	if(destination != NULL)
		package->destination = strdup(destination);

	else
		package->destination = NULL;

	return package;
}

void destroy_package(Package* package){

	//fail alloc
	if(!package)
		return;

	free(package->destination);
	free(package);
	return;
}

Manifest* create_manifest_node(void){

	Manifest *manifest_node = malloc(sizeof(Manifest));

	manifest_node->package = NULL;
	manifest_node->next = NULL;
	manifest_node->prev = NULL;


	return manifest_node;
}

void destroy_manifest_node(Manifest* manifest_node){

	destroy_package(manifest_node->package);
	manifest_node->next = NULL;
	manifest_node->prev = NULL;

	free(manifest_node);
	return;
}

Wearhouse* create_wearhouse(long capacity){

	if(capacity <= 0)
		return NULL;

	Wearhouse *w = malloc(sizeof(Wearhouse));

	//fail alloc
	if(!w)
		return NULL;

	w->capacity = capacity;
	w->size = 0;

	w->packages = malloc(capacity * sizeof(Package));
	
	return w;
}

Wearhouse *open_wearhouse(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Wearhouse *w = NULL;


	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	if((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		w = create_wearhouse(atol(token));

		free(line);
		line = NULL;
		len = 0;
	}

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		long priority = atol(token);
		token = strtok(NULL, ",\n ");
		Package *p = create_package(priority, token);
		w->packages[w->size++] = p;

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);
	fclose(fp);
	return w;
	file_open_exception:
	return NULL;
}

long wearhouse_is_empty(Wearhouse *w){

	if(w->size == 0)
		return 1;

	return 0;
}

long wearhouse_is_full(Wearhouse *w){

	if(w->size == w->capacity)
		return 1;

	return 0;
}

long wearhouse_max_package_priority(Wearhouse *w){

	long max = w->packages[0]->priority;

	for(long i = 1; i < w->size; i++) {

		if(max < w->packages[i]->priority)
			max = w->packages[i]->priority;
	}
	return max;
}

long wearhouse_min_package_priority(Wearhouse *w){

	long min = w->packages[0]->priority;

	for(long i = 1; i < w->size; i++) {

		if(min > w->packages[i]->priority)
			min = w->packages[i]->priority;
	}
	return min;
}


void wearhouse_print_packages_info(Wearhouse *w){
	for(long i = 0; i < w->size; i++){
		printf("P: %ld %s\n",
				w->packages[i]->priority,
				w->packages[i]->destination);
	}
	printf("\n");
}

void destroy_wearhouse(Wearhouse* wearhouse){

	for(long i = 0; i < wearhouse->capacity; i++)
		destroy_package(wearhouse->packages[i]);

	free(wearhouse->packages);
	free(wearhouse);
	return;
}


Robot* create_robot(long capacity){

	Robot *robot = malloc(sizeof(Robot));

	//fail alloc
	if(!robot)
		return NULL;

	robot->next = NULL;
	robot->size = 0;
	robot->capacity = capacity;
	robot->manifest = NULL;

	return robot;
}

int robot_is_full(Robot* robot){

	if(robot->size == robot->capacity)
		return 1;

	return 0;
}

int robot_is_empty(Robot* robot){

	if(robot->size == 0)
		return 1;

	return 0;
}

Package* robot_get_wearhouse_priority_package(Wearhouse *w, long priority){

	for(long i = 0; i < w->size; i++) {

		if(w->packages[i]->	priority == priority)
			return w->packages[i];
	}

	return NULL;
}

void robot_remove_wearhouse_package(Wearhouse *w, Package* package){
	//nu il distruge ci il sterge din packages
	long pos;
	for(long i = 0; i < w->size; i++) {

		//s-a gasit package de eliminat pe pozitia i
		if(w->packages[i] == package) {

			pos = i;
			break;
		}
	}

	for(long i = pos; i < w->size-1; i++) {
		w->packages[i] = w->packages[i+1];
	}

	w->size--;
	return;
}

void robot_load_one_package(Robot* robot, Package* package){
	if(robot_is_full(robot))
		return;

	if(robot->manifest == NULL) {

		robot->manifest = create_manifest_node();
		robot->manifest->package = create_package(package->priority, package->destination);
		robot->size++; return;

	}

	else if(robot->manifest != NULL) {

		Manifest *newNode = create_manifest_node();
		newNode->package = create_package(package->priority, package->destination);

		//daca se insereaza la inceput(primul element)
		if(package->priority > robot->manifest->package->priority) {

			newNode->prev = NULL;
			newNode->next = robot->manifest;
			robot->manifest->prev = newNode;
			robot->manifest = newNode;
			robot->size++; return;
		}

		//prioritati egale , inserare la inceput
		else if(package->priority == robot->manifest->package->priority) {

			if(strcmp(package->destination, robot->manifest->package->destination) < 0) {

				newNode->prev = NULL;
				newNode->next = robot->manifest;
				robot->manifest->prev = newNode;
				robot->manifest = newNode;
				robot->size++; return;
			}
		}
		//daca se insereaza la mijloc sau final, inserare dupa current
		else {
			Manifest *current = robot->manifest;

			while(current->next != NULL && package->priority < current->next->package->priority)
				current = current->next;

			//daca e la mijloc
			if(current->next != NULL) {

				newNode->prev = current;
				newNode->next = current->next;
				current->next->prev = newNode;
				current->next = newNode;
				robot->size++; return;
			}

			//daca e la final
			else if(current->next == NULL) {

				newNode->next = NULL;
				newNode->prev = current;
				current->next = newNode;
				robot->size++; return;
			}
		}
		Manifest *current = robot->manifest;
		//daca nu a ajuns in nici un caz => 
		//prioritatile devin egale => sortare dupa destination 
		
		//inserare DUPA current; current ajunge la priority egale
		while(current->next != NULL && package->priority != current->next->package->priority)
			current = current->next;

		//current se muta in functie de destination
		while(current->next != NULL && package->priority == current->next->package->priority &&
			strcmp(package->destination, current->next->package->destination) >= 0)
			current = current->next;

			// * mijloc sau final *

			//daca e undeva la mijloc
		if(current->next != NULL) {

			if(strcmp(package->destination, current->next->package->destination) < 0) {
					
				newNode->prev = current;
				newNode->next = current->next;
				current->next->prev = newNode;
				current->next = newNode;
				robot->size++; return;
			}
		}

		//daca e la final
		if(current->next == NULL) {

			newNode->next = NULL;
			newNode->prev = current;
			current->next = newNode;
			robot->size++; return;
		}
	}
}

long robot_load_packages(Wearhouse* wearhouse, Robot* robot){
//se ia pachetul din wearhouse, se pune in robot apoi se da remove din wearhouse.

	while(robot_is_full(robot) == 0) {

		long priority = wearhouse_max_package_priority(wearhouse);
		Package *package = robot_get_wearhouse_priority_package(wearhouse, priority);
		robot_load_one_package(robot, package);
		robot_remove_wearhouse_package(wearhouse, package);
	}
	return robot->size;
}

Package* robot_get_destination_highest_priority_package(Robot* robot, const char* destination){
	//pentru destinatia data intoarce pachetul cu cea mai mare prioritate
	if(robot_is_empty(robot))
		return NULL;

	Package *aux;
	long max = -1;

	Manifest *current = robot->manifest;

	while(current != NULL) {

		if(strcmp(destination, current->package->destination) == 0) {

			if(max < current->package->priority) {

				max = current->package->priority;
				aux = current->package;

			}
		}
		current = current->next;
	}
	return aux;
}

void destroy_robot(Robot* robot){

	Manifest *aux;

	while(robot->manifest != NULL) {

		destroy_package(robot->manifest->package);		
		aux = robot->manifest;
		free(aux);
		robot->manifest = robot->manifest->next;

	}
	free(robot->manifest);
	free(robot);
	return;
}

void robot_unload_packages(Truck* truck, Robot* robot){ 

	//daca tir e plin nu se mai poate adauga

	if(truck->capacity == 0)
		return;

	Manifest *currentM = robot->manifest;

	while(currentM != NULL) {

		if(truck_is_full(truck) == 1 || robot_is_empty(robot) == 1)
			return;

		if(robot->manifest == NULL)
			return;

		//s-a gasit pachet cu destinatia tirului aferent
		if(strcmp(currentM->package->destination, truck->destination) == 0) {

			//aloc noul nod de manifest si incarc pachetul in el
			Package *auxP = currentM->package;
			Manifest *newNode = create_manifest_node();
			newNode->package = create_package(auxP->priority, auxP->destination);

			if(truck->manifest == NULL) {

				truck->manifest = newNode;
				truck->manifest->prev = truck->manifest->next = NULL;
				truck->size++;
			}
			//adaug mereu in stanga
			else {

			newNode->prev = NULL;
			newNode->next = truck->manifest;
			truck->manifest->prev = newNode;
			truck->manifest = newNode;
			truck->size++;

			}
			//sterg packetul din manifestul robotului
			//daca e primul
			if(currentM->prev == NULL) {

				if(currentM->next != NULL) {

					currentM->next->prev = NULL;
					robot->manifest = currentM->next;
					robot->size--;
				}
				//daca e singurul
				else if(currentM->next == NULL) {

					robot->size--;
					return;
				}
			}
			//daca e la mijloc sau ultimul
			else if(currentM->prev != NULL)	{

				//mijloc
				if(currentM->next != NULL) {

					currentM->prev->next = currentM->next;
					currentM->next->prev = currentM->prev;
					robot->size--;
				}
				else if(currentM->next == NULL) {

					//ultimul
					currentM->prev->next = NULL;
					robot->size--;
				}
			}
		}
		currentM = currentM->next;
	}
	return;
}

// Attach to specific truck
int robot_attach_find_truck(Robot* robot, Parkinglot *parkinglot){
	int found_truck = 0;
	long size = 0;
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	Manifest* m_iterator = robot->manifest;


	while(m_iterator != NULL){
		while(arrived_iterator != parkinglot->arrived_trucks){
			size  = truck_destination_robots_unloading_size(arrived_iterator);
			if(strncmp(m_iterator->package->destination, arrived_iterator->destination, MAX_DESTINATION_NAME_LEN) == 0 &&
					size < (arrived_iterator->capacity-arrived_iterator->size)){
				found_truck = 1;
				break;
			}

			arrived_iterator = arrived_iterator->next;
		}

		if(found_truck)
			break;
		m_iterator = m_iterator->next;
	}

	if(found_truck == 0)
		return 0;


	Robot* prevr_iterator = NULL;
	Robot* r_iterator = arrived_iterator->unloading_robots;
	while(r_iterator != NULL){
		Package *pkg = robot_get_destination_highest_priority_package(r_iterator, m_iterator->package->destination);
		if(m_iterator->package->priority >= pkg->priority)
			break;
		prevr_iterator = r_iterator;
		r_iterator = r_iterator->next;
	}

	robot->next = r_iterator;
	if(prevr_iterator == NULL)
		arrived_iterator->unloading_robots = robot;
	else
		prevr_iterator->next = robot;

	return 1;
}

void robot_print_manifest_info(Robot* robot){
	Manifest *iterator = robot->manifest;
	while(iterator != NULL){
		printf(" R->P: %s %ld\n", iterator->package->destination, iterator->package->priority);
		iterator = iterator->next;
	}

	printf("\n");
}

Truck* create_truck(const char* destination, long capacity, long transit_time, long departure_time){

	Truck *truck = malloc(sizeof(Truck));

	truck->transit_end_time = transit_time;
	truck->departure_time = departure_time;
	truck->capacity = capacity;
	truck->size = 0;
	truck->manifest = NULL;
	truck->unloading_robots = NULL;
	truck->next = NULL;

	if(destination != NULL)
		truck->destination = strdup(destination);

	else truck->destination = NULL;

	return truck;
}

int truck_is_full(Truck *truck){

	if(truck->size == truck->capacity)
		return 1;

	else return 0;
}

int truck_is_empty(Truck *truck){

	if(truck->size == 0)
		return 1;

	else return 0;
}

long truck_destination_robots_unloading_size(Truck* truck){
	/*returneaza cate pachete au acelasi destination cu truckul,
	pentru fiecare robot se parcurge manifestul iar cand un pachet
	din manifest are aceeasi dest cu truck dest se incrementeaza*/

	if(truck->unloading_robots == NULL)
		return 0;

	long total = 0;

	Robot *currentR = truck->unloading_robots;

	while(currentR != NULL) {

	Manifest *currentM = currentR->manifest;

		while(currentM != NULL) {

			if(strcmp(currentM->package->destination, truck->destination) == 0) {
				total +=  currentR->size;
				break;
			}
			currentM = currentM->next;
		}
		currentR = currentR->next;
	}
	return total;
}

void truck_print_info(Truck* truck){
	printf("T: %s %ld %ld %ld %ld %ld\n", truck->destination, truck->size, truck->capacity,
			truck->in_transit_time, truck->transit_end_time, truck->departure_time);

	Manifest* m_iterator = truck->manifest;
	while(m_iterator != NULL){
		printf(" T->P: %s %ld\n", m_iterator->package->destination, m_iterator->package->priority);
		m_iterator = m_iterator->next;
	}

	Robot* r_iterator = truck->unloading_robots;
	while(r_iterator != NULL){
		printf(" T->R: %ld %ld\n", r_iterator->size, r_iterator->capacity);
		robot_print_manifest_info(r_iterator);
		r_iterator = r_iterator->next;
	}
}

void destroy_truck(Truck* truck){

	if(!truck)
		return;

	if(truck->destination != NULL)
		free(truck->destination);

	Manifest *auxM;
	Manifest *currentM = truck->manifest;

	for(long i = 0; i < truck->size; i++) {

		auxM = currentM;
		free(auxM);
		currentM = currentM->next;
	}

	free(truck->manifest);

	Robot *auxR;
	Robot *currentR = truck->unloading_robots;

	while(currentR != NULL) {

		auxR = currentR;
		destroy_robot(auxR);
		currentR = currentR->next;
	}

	free(truck->unloading_robots);
	free(truck);

	return;
}

Parkinglot* create_parkinglot(void){
	// Allocate parking lot

	Parkinglot *parkinglot = malloc(sizeof(Parkinglot));

	parkinglot->arrived_trucks = calloc(1, sizeof(Truck));
	parkinglot->departed_trucks = calloc(1, sizeof(Truck));
	parkinglot->pending_robots = calloc(1, sizeof(Robot));
	parkinglot->standby_robots = calloc(1, sizeof(Robot));

	parkinglot->arrived_trucks->next = parkinglot->arrived_trucks;
	parkinglot->departed_trucks->next = parkinglot->departed_trucks;
	parkinglot->pending_robots->next = parkinglot->pending_robots;
	parkinglot->standby_robots->next = parkinglot->standby_robots;

	return parkinglot;
}

Parkinglot* open_parckinglot(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Parkinglot *parkinglot = create_parkinglot();

	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		// destination, capacitym transit_time, departure_time, arrived
		if(token[0] == 'T'){
			token = strtok(NULL, ",\n ");
			char *destination = token;

			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			token = strtok(NULL, ",\n ");
			long transit_time = atol(token);

			token = strtok(NULL, ",\n ");
			long departure_time = atol(token);

			token = strtok(NULL, ",\n ");
			int arrived = atoi(token);

			Truck *truck = create_truck(destination, capacity, transit_time, departure_time);

			if(arrived)
				truck_arrived(parkinglot, truck);
			else
				truck_departed(parkinglot, truck);

		}else if(token[0] == 'R'){
			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			Robot *robot = create_robot(capacity);
			parkinglot_add_robot(parkinglot, robot);

		}

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);

	fclose(fp);
	return parkinglot;

	file_open_exception:
	return NULL;
}

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){
/* parcurg lista de roboti, daca size = 0 il pun in standby robots
si sortez dupa capacitate, iar daca size != 0 il pun in pending robots
sortati dupa size
*/
	if(parkinglot == NULL)
		return;

	if(robot == NULL)
		return;

	Robot *addMe = robot;

	if(robot->size == 0) {

		//in standby robots sortat dupa capacitate
		Robot *head = parkinglot->standby_robots->next;
		Robot *current = head;
		
		//daca standby robots e goala
		if(current == NULL) {

			addMe->next = addMe;
			head = addMe;
			return;
		}

		//trebuie adaugat primul element
		else if(robot->capacity > current->capacity) {

			//ma duc la final sa actualizez head ul
			while(current->next != head)
				current = current->next;

			current->next = addMe;
			addMe->next = head;
			head = addMe;
			return;
		}

		//daca e undeva la mijloc
		else {

			while(current->next != head &&
				robot->capacity < current->next->capacity)
				current = current->next;

			addMe->next = current->next;
			current->next = addMe;
			return;
		}

	}

	else if(robot->size != 0) {

		//in standby robots sortat dupa capacitate
		Robot *head = parkinglot->pending_robots->next;
		Robot *current = head;

		//daca standby robots e goala
		if(current == NULL) {

			addMe->next = addMe;
			head = addMe;
			return;
		}

		//trebuie adaugat primul element
		else if(robot->capacity > current->capacity) {

			//ma duc la final sa actualizez head ul
			while(current->next != head)
				current = current->next;

			current->next = addMe;
			addMe->next = head;
			head = addMe;
			return;
		}

		//daca e undeva la mijloc
		else {

			while(current->next != head &&
				robot->capacity < current->next->capacity)
				current = current->next;

			addMe->next = current->next;
			current->next = addMe;
			return;
		}
	}
}

void parkinglot_remove_robot(Parkinglot *parkinglot, Robot* robot){

	/*daca robot->size == 0 il caut in standby dupa capacitate si il sterg iar
	daca robot->size != 0 il caut in pending dupa size si il sterg
	*/

	if(parkinglot == NULL)
		return;

	Robot *headS = parkinglot->standby_robots->next;
	Robot *currentS = headS;
	Robot *headP = parkinglot->pending_robots->next;
	Robot *currentP = headP;
	Robot *prevS, *prevP;

	if(robot->size == 0) {

		if(currentS == NULL)
			return;

		//se afla in standby robots
		while(currentS->next != headS && currentS->next->capacity != 0) {

			prevS = currentS;
			currentS = currentS->next;
		}

		//daca e singurul
		if(currentS->next == headS) {

			headS = NULL;
			parkinglot->standby_robots->next = parkinglot->standby_robots;
			return;
		}

		//daca e primul element
		else if(currentS == headS) {

			prevS = headS;
			//ma duc la final sa actualizez head ul
			while(prevS->next != headS)
				prevS = prevS->next;

			headS = currentS->next;
			prevS->next = headS;
			return;
		}

		//daca e ultimul element
		else if(currentS->next == headS) {

			prevS->next = headS;
			return;				
		}

		//daca e undeva la mijloc
		else {

			prevS->next = currentS->next;
			return;
		}
	}

	if(robot->size != 0) {

		if(currentP == NULL)
			return;

		//se afla in pending robots
		while(currentP->next != headP && currentP->next->size == 0) {

			prevP = currentP;
			currentP = currentP->next;
		}

		//daca e singurul
		if(currentP->next == headP) {

			headP = NULL;
			parkinglot->pending_robots->next = parkinglot->pending_robots;
			return;
		}

		//daca e primul element
		if(currentP == headP) {

			prevP = headP;
			//ma duc la final sa actualizez head ul
			while(prevP->next != headP)
				prevP = prevP->next;

			headP = currentP->next;
			prevP->next = headP;
			return;
		}

		//daca e ultimul element
		else if(currentP->next == headP) {

			prevP->next = headP;
			return;				
		}

		//daca e undeva la mijloc
		else {

			prevP->next = currentP->next;
			return;
		}
	}
}

int parckinglot_are_robots_peding(Parkinglot* parkinglot){

	if(parkinglot == NULL)
		return 0;

	Robot *head = parkinglot->pending_robots->next;
	Robot *current = head;

	while(current->next != head)
		current = current->next;

	if(current == head)
		return 0;

	else return 1;
}

int parkinglot_are_arrived_trucks_empty(Parkinglot* parkinglot){
	
	if(parkinglot == NULL)
		return 0;

	int nrTrucks = 0;
	int emptyTrucks = 0;

	Truck *head = parkinglot->arrived_trucks->next;
	Truck *current = head;

	while(current->next != head) {

		nrTrucks++;
		if(truck_is_empty(current))
			emptyTrucks++;

		current = current->next;
	}

	if(nrTrucks == emptyTrucks)
		return 1;

	else return 0;
}

int parkinglot_are_trucks_in_transit(Parkinglot* parkinglot){

	if(parkinglot == NULL)
		return 0;

	Truck *head = parkinglot->departed_trucks->next;
	Truck *current = head;

	while(current->next != head) {

		current = current->next;
	}

	if(current != head)
		return 1;

	else return 0;
}

void destroy_parkinglot(Parkinglot* parkinglot){

	if(parkinglot == NULL)
		return;

	free(parkinglot->arrived_trucks);
	free(parkinglot->departed_trucks);
	free(parkinglot->pending_robots);
	free(parkinglot->standby_robots);
	free(parkinglot);
}

void parkinglot_print_arrived_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->arrived_trucks->next;
	while(iterator != parkinglot->arrived_trucks){

		truck_print_info(iterator);
		iterator = iterator->next;
	}

	printf("\n");
}

void parkinglot_print_departed_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->departed_trucks->next;
	while(iterator != parkinglot->departed_trucks){
		truck_print_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");
}

void parkinglot_print_pending_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->pending_robots->next;
	while(iterator != parkinglot->pending_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");
}

void parkinglot_print_standby_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->standby_robots->next;
	while(iterator != parkinglot->standby_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");
}


void truck_departed(Parkinglot *parkinglot, Truck* truck){
	/* tirul din parametru il adauga la parking->departed in functie de departure time, 
	si ii da remove din parking->arrived
	*/

	//STERGERE
	Truck *head = parkinglot->arrived_trucks;
	Truck *current = head;

	while(current->next != head && current->next->departure_time != truck->departure_time) {

		current = current->next;
	}

	if(current->next->departure_time == truck->departure_time)
		current->next = current->next->next;


	//ADAUGARE
	head = parkinglot->departed_trucks;
	current = head;
	Truck *addMe = truck;

	//lista este goala
	if(head->next == NULL) {

		addMe->next = head;
		head->next = addMe;	
	}

	//adaugare in functie de time, inserare dupa current
	while(current->next != head && truck->departure_time > current->next->departure_time)
		current = current->next;

	addMe->next = current->next;
	current->next = addMe;
	return;	
}

void truck_arrived(Parkinglot *parkinglot, Truck* truck){	
	if(parkinglot == NULL || truck == NULL) return;

	// Search through departed list, if exists node is found remove it
	// Note: this must remove the node not deallocate it

	//STERGERE
	Truck *head = parkinglot->departed_trucks;
	Truck *current = head;

	while(current->next != head && current->next != truck)
		current = current->next;

	if(current->next->departure_time == truck->departure_time)
		current->next = current->next->next;

	//ADAUGARE in fct de destination si apoi crescator departure time
	head = parkinglot->arrived_trucks;
	current = head;

	Truck *addMe = truck;
	addMe->size = 0;
	addMe->in_transit_time = 0;
	addMe->manifest = NULL;

	//lista este goala
	if(head->next == head) {

		head->next = addMe;
		addMe->next = head;
		return;
	}

	//adaugare in functie de dest, inserare dupa current
	while(current->next != head && strcmp(truck->destination, current->next->destination) > 0)
		current = current->next;

	if(current != head) {

		if(strcmp(truck->destination, current->destination) > 0) {

			addMe->next = current->next;
			current->next = addMe;
		}
	}

	//departure time
	else if(strcmp(truck->destination, current->next->destination) == 0) {

		if(truck->departure_time < current->next->departure_time) {

			addMe->next = current->next;
			current->next = addMe;
		}

		else if(truck->departure_time > current->next->departure_time) {

			current = current->next;
			addMe->next = current->next;
			current->next = addMe;
		}
	}
	return;
}

void truck_transfer_unloading_robots(Parkinglot* parkinglot, Truck* truck){

	Robot *current = truck->unloading_robots;
	Robot *addMe;

	while(current != NULL) {

		addMe = current;
		current = current->next;
		parkinglot_add_robot(parkinglot, addMe);
	}

	truck->unloading_robots = NULL;
}

// Depends on parking_turck_departed
void truck_update_depatures(Parkinglot* parkinglot, long day_hour){

	Truck *head = parkinglot->arrived_trucks;
	Truck *current = head;

	while(current->next != head) {

		if(day_hour == current->next->departure_time) {

			Truck *update = current->next;
			truck_transfer_unloading_robots(parkinglot, update);
			truck_departed(parkinglot, update);
		}

		current = current->next;
	}
}

// Depends on parking_turck_arrived
void truck_update_transit_times(Parkinglot* parkinglot){

	/*pt toate camioanele din departed plusez transit iar daca e
	egal cu end time il trec in arrived*/

	Truck *head = parkinglot->departed_trucks;
	Truck *current = head;

	while(current->next != head) {

		current->next->in_transit_time++;
		if(current->next->in_transit_time == current->next->transit_end_time) {

			truck_arrived(parkinglot, current->next);
			current->next->in_transit_time = 0;
		}
		current = current->next;
	}
}

void robot_swarm_collect(Wearhouse *wearhouse, Parkinglot *parkinglot){
	Robot *head_robot = parkinglot->standby_robots;
	Robot *current_robot = parkinglot->standby_robots->next;
	while(current_robot != parkinglot->standby_robots){

		// Load packages from wearhouse if possible
		if(!robot_load_packages(wearhouse, current_robot)){
			break;
		}

		// Remove robot from standby list
		Robot *aux = current_robot;
		head_robot->next = current_robot->next;
		current_robot = current_robot->next;

		// Add robot to the
		parkinglot_add_robot(parkinglot, aux);
	}
}


void robot_swarm_assign_to_trucks(Parkinglot *parkinglot){

	Robot *current_robot = parkinglot->pending_robots->next;

	while(current_robot != parkinglot->pending_robots){
		Robot* aux = current_robot;
		current_robot = current_robot->next;
		parkinglot_remove_robot(parkinglot, aux);
		int attach_succeded = robot_attach_find_truck(aux, parkinglot);
		if(!attach_succeded)
			parkinglot_add_robot(parkinglot, aux);
	}
}

void robot_swarm_deposit(Parkinglot* parkinglot){
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	while(arrived_iterator != parkinglot->arrived_trucks){
		Robot *current_robot = arrived_iterator->unloading_robots;
		while(current_robot != NULL){
			robot_unload_packages(arrived_iterator, current_robot);
			Robot *aux = current_robot;
			current_robot = current_robot->next;
			arrived_iterator->unloading_robots = current_robot;
			parkinglot_add_robot(parkinglot, aux);
		}
		arrived_iterator = arrived_iterator->next;
	}
}