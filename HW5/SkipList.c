/*
  Author: Michael Mascari
  Email: mmascari2017@my.fit.edu
  Course: Algorithms & Data Structures
  Section: 2
  Description: hw5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//command struct to help with debugging 
//holds the input commands in order for processing
typedef struct COMMAND{
	char name[30];
	int date1;
	int date2;
	char event[101];
	struct COMMAND* next;
}COMMAND;

//command pointers
COMMAND* head = NULL;
COMMAND* pcommand = NULL;

//quad node according to the course slides
typedef struct NODE{
    int key;
    char event[100];
    struct NODE* next;
	struct NODE* prev;
	struct NODE* up;
	struct NODE* down;
}NODE;

//skiplist struct so height can be stored with it
typedef struct SKIPLIST{
	int size;
	NODE* first;
}SKIPLIST;

extern int getRandHeight(); //Dr Chan's function
void init_list(SKIPLIST* list); //sets up the skiplist
void add_height(SKIPLIST* list); //adds another height to the skiplist
void read_file(int argc, char*argv[]); //reads the input and stores in the COMMAND list
void add_command(char name[], int date1, int date2, char event[]); //adds a command to the command linked list
void do_commands(SKIPLIST list); //does the COMMAND list in order
void display_event(SKIPLIST* list, int date); //prints out the date at that date value
void add_event(SKIPLIST* list, int date, char event[]); //adds an event at the date value
void add_list(SKIPLIST* list); //adds a new skiplist level
void delete_event(SKIPLIST* list, int date); //deletes the event at the date value
void display_between_dates(SKIPLIST* list, int date1, int date2); //displayes every event between two dates
void display_from_start_date(SKIPLIST* list, int date); //displays every event until the date value
void display_to_end_date(SKIPLIST* list, int date); //displays every event after the date value
void display_all_events(SKIPLIST* list); //displayes every event at every date value
void print_skip_list(SKIPLIST* list); //prints every line of the skip list
void CPrint(); //debug function

int main(int argc, char* argv[]){
	SKIPLIST list; //skiplist created
	init_list(&list); //first level of skiplist created, beacsue it is unique to other times a level is inserted to the list.
	read_file(argc, argv); //reads in the commands from the input file, stores the commands in the COMMAND linked list
	do_commands(list); //does the COMMANDS in order
	return 0;
}

//does the COMAND list in order
void do_commands(SKIPLIST list){
	pcommand = head; //resets pcommand beacause it is done being used for add_command();
	while(pcommand!=NULL){ //enters the correct function according to pcommand->name
		if(strcmp(pcommand->name,"DisplayEvent")==0){
			display_event(&list, pcommand->date1);
		}else if(strcmp(pcommand->name,"AddEvent")==0){
			add_event(&list, pcommand->date1, pcommand->event);
		}else if(strcmp(pcommand->name,"DeleteEvent")==0){
			delete_event(&list, pcommand->date1);
		}else if(strcmp(pcommand->name,"DisplayEventsBetweenDates")==0){
			display_between_dates(&list, pcommand->date1, pcommand->date2);
		}else if(strcmp(pcommand->name,"DisplayEventsFromStartDate")==0){
			display_from_start_date(&list, pcommand->date1);
		}else if(strcmp(pcommand->name,"DisplayEventsToEndDate")==0){
			display_to_end_date(&list, pcommand->date1);
		}else if(strcmp(pcommand->name,"DisplayAllEvents")==0){
			display_all_events(&list);
		}else if(strcmp(pcommand->name,"PrintSkipList")==0){
			print_skip_list(&list);
		}
		pcommand = pcommand->next; //pcommand goes to the next COMMAND
	}
	return;
}

//adds another height to the skiplist
void add_height(SKIPLIST* list){
	list->size++; //adjusts the height variable because another level is being added
	NODE* temp = list->first;
	NODE* new_front = (NODE*) malloc(sizeof(NODE)); //new starting node for the level created
	new_front->key = -1;
	temp->up = new_front;
	new_front->down = temp;
	NODE* new_end = (NODE*) malloc(sizeof(NODE)); //new ending node for the level created
	new_end->key = 10000;
	new_front->next = new_end;
	while(temp->key!=10000){ //moves temp until it is at the current levels last value
		temp = temp->next;
	}
	
	//reassigns a large amount of pointers 
	temp->up = new_end;
	new_end->down = temp;
	new_end->prev = new_front;
	new_front->up = NULL;
	new_front->prev = NULL;
	new_end->next = NULL;
	new_end->up = NULL;
	list->first = new_front;
	return;
}

 //prints out the date at that date value
void display_event(SKIPLIST* list, int date){
	NODE* temp = list->first; //starts at the front of the skiplist
	for(int i = 0; i < list->size; i++){ //for every height value
		while(date >= temp->next->key){ // while date is greater than the next nodes key, check the next node
			temp = temp->next;
		}
		if(temp->key == date){ // if date equals the nodes key, break from the for loop
			break;
		}
		if(date < temp->next->key){ //if date is less than the next nodes key, drop to the next level
			temp = temp->down;
		}
	}
	while(date >= temp->next->key){ //checks on the last level. is not in the for loop or else it could drop past the skiplist
		temp = temp->next;
	}
	
	//prints out according to how the date was found or if it was found
	if(date == temp->key){
		if(date > 999){
			printf("DisplayEvent %d %s\n", date, temp->event);
		}else{
			printf("DisplayEvent 0%d %s\n", date, temp->event);
		}
	}else{
		if(date > 999){
			printf("DisplayEvent %d none\n", date);
		}else{
			printf("DisplayEvent 0%d none\n", date);
		}
	}
	return;
}

//adds an event at the date value
void add_event(SKIPLIST* list, int date, char event[]){
	int height = getRandHeight(); //gets a height for the value from Dr chan's function
	if(height >= list->size){ //if the height found is too large for the current skiplist, adjust the skiplist
		add_height(list);
	}
	NODE* other_temp = list->first; //NODE starts at front of list
	while(other_temp->down!=NULL){ //drops down
		other_temp = other_temp->down;
	}
	NODE* temp = other_temp;
	while(temp->key < date){ //finds spot for date according to key
		temp = temp->next;
		if(temp->key == date){ //if a key matches date, it cannot be placed so the error is returned
			printf("AddEvent");
			if(date > 999){
				printf(" %d ", date);
			}else{
				printf(" 0%d " , date);
			}
			printf("%s replacingExistingEvent\n", event);
			return;
		}
	}
	if(height >= 0){ //according to the height given, a certain amount fo nodes needs to be made
		NODE* new = (NODE*) malloc(sizeof(NODE)); //NODE made then values and pointers adjusted
		new->key = date;
		strcpy(new->event, event);
		temp->prev->next = new;
		new->prev = temp->prev;
		new->next = temp;
		temp->prev = new;
		new->up = NULL;
		new->down = NULL;
		if(height >= 1){ //according to the height given, a certain amount fo nodes needs to be made
			other_temp = other_temp->up; //goes to the above level. 
			temp = other_temp;
			while(temp->key < date){ //finds the correct spot at the new level
				temp = temp->next;
			}
			NODE* new1 = (NODE*) malloc(sizeof(NODE)); //NODE made then values and pointers adjusted
			new1->key = date;
			strcpy(new1->event, event);
			temp->prev->next = new1;
			new1->prev = temp->prev;
			new1->next = temp;
			temp->prev = new1;
			new1->up = NULL;
			new1->down = new;
			new->up = new1;
			if(height >= 2){ //according to the height given, a certain amount fo nodes needs to be made
				other_temp = other_temp->up; //goes to the above level. 
				temp = other_temp;
				while(temp->key < date){ //finds the correct spot at the new level
					temp = temp->next;
				}
				NODE* new2 = (NODE*) malloc(sizeof(NODE)); //NODE made then values and pointers adjusted
				new2->key = date;
				strcpy(new2->event, event);
				temp->prev->next = new2;
				new2->prev = temp->prev;
				new2->next = temp;
				temp->prev = new2;
				new2->down = new1;
				new2->up = NULL;
				if(height >= 3){ //according to the height given, a certain amount fo nodes needs to be made
					other_temp = other_temp->up;//goes to the above level. 
					temp = other_temp;
					while(temp->key < date){ //finds the correct spot at the new level
						temp = temp->next;
					}
					NODE* new3 = (NODE*) malloc(sizeof(NODE)); //NODE made then values and pointers adjusted
					new3->key = date;
					strcpy(new3->event, event);
					temp->prev->next = new3;
					new3->prev = temp->prev;
					new3->next = temp;
					temp->prev = new3;
					new3->up = NULL;
					new3->down = new2;
					new2->up = new3;
					if(height >= 4){ //according to the height given, a certain amount fo nodes needs to be made
						other_temp = other_temp->up;//goes to the above level. 
						temp = other_temp;
						while(temp->key < date){ //finds the correct spot at the new level
							temp = temp->next;
						}
						NODE* new4 = (NODE*) malloc(sizeof(NODE)); //NODE made then values and pointers adjusted
						new4->key = date;
						strcpy(new4->event, event);
						temp->prev->next = new4;
						new4->prev = temp->prev;
						new4->next = temp;
						temp->prev = new4;
						new4->up = NULL;
						new4->down = new3;
						new3->up = new4;
					}
				}
			}
		}
		//prints out that an event was added
		printf("AddEvent ");
		if(new->key >999){
			printf("%d ", new->key);	
		}else{
			printf("0%d ",new->key);
		}
		printf("%s success\n", event);
		new->down = NULL;
	}
	return;
}

//deletes the event at the date value
void delete_event(SKIPLIST* list, int date){
	int dropped = 0; //dropped keeps track of levels fallen. used for recursion.
	NODE* temp = list->first; //start at the top of the list
	for(int i = 0; i < list->size; i++){ //loops through for the total height
		while(date >= temp->next->key){ // while date >= next node key, check next node
			temp = temp->next;
		}
		if(temp->key == date){ //if date equals NODE key, break the for loop, means dropped != total height of list, because it did not fall all the way
			break;
		}
		if(date < temp->next->key){ // if date < next NODE key, incriment dropped, fall to the next list.
			dropped++;
			temp = temp->down;
		}
	}
	while(date >= temp->next->key){ //checks one one time outside of list. does it this way so it cant drop outside of the whole structure
		temp = temp->next;
	}
	if(dropped == list->size){ //if dropped equals total height, it was the last time the value had to be deleted
		//prints out what happened according to if the value existed
		if(date == temp->key){ 
			if(date > 999){
				printf("DeleteEvent %d success\n", date);
			}else{
				printf("DeleteEvent 0%d success\n", date);
			}
		}else{
			if(date > 999){
				printf("DeleteEvent %d noDateError\n", date);
			}else{
				printf("DeleteEvent 0%d noDateError\n", date);
			}
		}
	}
	if(temp->key == date){ //readjusts pointers and frees the temp NODE
		temp->next->prev = temp->prev;
		temp->prev->next = temp->next;
		free(temp);
	}
	if(dropped != list->size){ //will == list->size when the pointer had to drop the whole skiplist, meaning it removed from the bottom list
		delete_event(list,date); //recursive until they are all gone
	}
	return;
}

//displayes every event between two dates
void display_between_dates(SKIPLIST* list, int date1, int date2){
	if(date1 > 999){
		printf("DisplayEventsBetweenDates %d %d ", date1 ,date2);
	}else{
		if(date2 > 999){
			printf("DisplayEventsBetweenDates 0%d %d ", date1, date2);
		}else{
			printf("DisplayEventsBetweenDates 0%d 0%d ", date1, date2);
		}
	}
	NODE* temp = list->first;
	for(int i = 0; i < list->size; i++){ //for every level
		while(date1 >= temp->next->key){ //while first date is greater or equal to next NODE key, NODE equals next NODE
			temp = temp->next;
		}
		if(temp->key == date1){ //if first date is found, break
			break;
		}
		if(date1 < temp->next->key){ //if first date is less than next NODE key, drop to next level
			temp = temp->down;
		}
	}
	if(temp->key == date1){
		while(temp->down!=NULL){ //falls to the bootom level so every date gets printed
			temp = temp->down;
		}
	}else{
		while(date1 >= temp->key){ //loops through the bootm level
			temp = temp->next;
		}
	}
	while(temp->key < date2){ //while key is less than second date
		if(temp->key > 999){
			printf("%d:%s ",temp->key, temp->event);
		}else{
			printf("0%d:%s ",temp->key, temp->event);
		}
		temp = temp->next;
	}
	printf("\n");
	return;
}

//displays every event until the date value
void display_from_start_date(SKIPLIST* list, int date){
	if(date > 999){
		printf("DisplayEventsFromStartDate %d ", date);
	}else{
		printf("DisplayEventsFromStartDate 0%d ", date);
	}
	NODE* temp = list->first; //start at front of skiplist
	for(int i = 0; i < list->size; i++){ //for every height of list
		while(date >= temp->next->key){ //while date is greater or equal to next NODE key, NODE equal next NODE
			temp = temp->next;
		}
		if(temp->key == date){ // if NODE key equals date, break
			break;
		}
		if(date < temp->next->key){ //if date is less than NODE next key, dropdown
			temp = temp->down;
		}
	}
	if(temp->key == date){ //if the key was found, drop to the lowest level with that key
		while(temp->down!=NULL){
			temp = temp->down;
		}
	}else{ //else, it is known we are on the lowest level so loop through the values until the correct one is found, or NULL
		while(date >= temp->key){
			temp = temp->next;
		}
	}
	while(temp->key != 10000){ //print if key does not equal the end key
		if(temp->key > 999){
			printf("%d:%s ",temp->key, temp->event);
		}else{
			printf("0%d:%s ",temp->key, temp->event);
		}
		temp = temp->next;
	}
	printf("\n");
	return;
}

//displays every event after the date value
void display_to_end_date(SKIPLIST* list, int date){
	printf("DisplayEventsToEndDate ");
	NODE* temp = list->first;
	while(temp->down!=NULL){ //drops to the bottom list because it wants to print from the start to the end date. same Big(O) time as binary search for front, but no comprisons needed.
		temp = temp->down;
	}
	temp = temp->next;
	while(temp->key <= date){//prints out every item until date is larger than the key for the current item
		if(temp->key >999){
			printf("%d:%s ", temp->key, temp->event);
		}else{
			printf("0%d:%s ", temp->key, temp->event);
		}
		temp = temp->next;
	}
	printf("\n");
	return;
} 

//displayes every event at every date value
void display_all_events(SKIPLIST* list){
	printf("DisplayAllEvents ");
	NODE* temp = list->first;
	while(temp->down!=NULL){ //drops to the bottom list because it wants to print from the start to the end date. same Big(O) time as binary search for front, but no comprisons needed.
		temp = temp->down;
	}
	temp = temp->next;
	while(temp->key!=10000){ //prints until the end date
		if(temp->key >999){
			printf("%d:%s ", temp->key, temp->event);
		}else{
			printf("0%d:%s ", temp->key, temp->event);
		}
		temp = temp->next;
	}
	printf("\n");
	return;
}

//prints every line of the skip list
void print_skip_list(SKIPLIST* list){	
	printf("PrintSkipList\n");
	int i = list->size;
	NODE* temp = list->first;
	NODE* other_temp;
	while(temp!=NULL){
		other_temp = temp;
		printf("(S%d) ", i);
		if(other_temp->next->key == 10000){ //if the first cell points to the last cell we know it is empty
			printf("empty"); //can have more than one empty if something in S3 gets removed. Not a rule of skipists to keep all but one list occupied.
		}else{
			while(other_temp!=NULL){ //loops through the list
				if(other_temp->key!=-1 && other_temp->key!=10000){ //doesn't drint the first or last cell
					if(other_temp->key>999){
						printf("%d", other_temp->key);
					}else{
						printf("0%d", other_temp->key);
					}
					printf(":%s ", other_temp->event);
				}
				other_temp = other_temp->next;
			}
		}
		i--;
		printf("\n");
		temp = temp->down; //drops to the next list down;
	}

	return;
}

//sets up the skiplist
void init_list(SKIPLIST* list){
	list->size = 0; //first time list is used so height equals 0;
	NODE* start = (NODE*) malloc(sizeof(NODE)); //names starting node
	start->key = -1; // -1 is less than any date
	list->first = start; //assigns start to the begining of the skiplist
	NODE* end = (NODE*) malloc(sizeof(NODE)); //names ending node
	end->key = 10000; //10000 is greater than any date
	//assigns many pointers
	end->prev = start;
	start->next = end;
	start->prev = NULL;
	start->up = NULL;
	start->down = NULL;
	end->next = NULL;
	end->up = NULL;
	end->down = NULL;
}

void read_file(int argc, char*argv[]){
	//these four variables are for the stroing the command
	char name[30];
	int date1;
	int date2;
	char event[101];
	FILE * data_input = fopen(argv[1], "r"); //opens the input file
	while(fscanf(data_input, "%s ", name)!= EOF){ //scans in the correct amount of fields accoring to the command
			if(strcmp(name,"DisplayEvent")==0){
				fscanf(data_input, "%d ", &date1);
			}else if(strcmp(name,"AddEvent")==0){
				fscanf(data_input, "%d %s ", &date1, event);
			}else if(strcmp(name,"DeleteEvent")==0){
				fscanf(data_input, "%d ", &date1);
			}else if(strcmp(name,"DisplayEventsBetweenDates")==0){
				fscanf(data_input, "%d %d ", &date1, &date2);
			}else if(strcmp(name,"DisplayEventsFromStartDate")==0){
				fscanf(data_input, "%d ", &date1);
			}else if(strcmp(name,"DisplayEventsToEndDate")==0){
				fscanf(data_input, "%d ", &date1);
			}
			add_command(name, date1, date2, event); //adds all four fields to each command, but do_commands(); will only do the correct fields
	}
	return;
}

//adds commadn to COMMAND linked list
void add_command(char name[], int date1, int date2, char event[]){
	COMMAND* newcommand = (COMMAND*) malloc(sizeof(COMMAND)); //mallocs the new command to place in the list
	//assigns the fields of the COMMAND struct
	newcommand->date1 = date1;
	newcommand->date2 = date2;
	strcpy(newcommand->name,name);
	strcpy(newcommand->event,event);
	newcommand->next = NULL;
	//places the COMMAND in the correct place
	if(head == NULL){
		head = newcommand;
		pcommand = newcommand;
	}else{
		pcommand->next = newcommand;
		pcommand = newcommand;
	}
	return;
}

//debug function
void CPrint(){
	pcommand = head;
	while(pcommand != NULL){
		printf("%s ", pcommand->name);
		if(strcmp(pcommand->name,"DisplayEvent")==0){
			if(pcommand->date1 < 1000){
				printf("0");
			}
			printf("%d\n", pcommand->date1);
		}else if(strcmp(pcommand->name,"AddEvent")==0){
			if(pcommand->date1 < 1000){
				printf("0");
			}
			printf("%d %s\n", pcommand->date1, pcommand->event);
		}else if(strcmp(pcommand->name,"DeleteEvent")==0){
			if(pcommand->date1 < 1000){
				printf("0");
			}
			printf("%d\n", pcommand->date1);
		}else if(strcmp(pcommand->name,"DisplayEventsBetweenDates")==0){
			if(pcommand->date1 < 1000){
				printf("0");
			}
			printf("%d ", pcommand->date1);
			if(pcommand->date2 < 1000){
				printf("0");
			}
			printf("%d\n", pcommand->date2);
		}else if(strcmp(pcommand->name,"DisplayEventsFromStartDate")==0){
			if(pcommand->date1 < 1000){
				printf("0");
			}			
			printf("%d\n", pcommand->date1);
		}else if(strcmp(pcommand->name,"DisplayEventsToEndDate")==0){
			if(pcommand->date1 < 1000){
				printf("0");
			}			
			printf("%d\n", pcommand->date1);
		}else if(strcmp(pcommand->name,"DisplayAllEvents")==0){
			printf("\n");
		}else if(strcmp(pcommand->name,"PrintSkipList")==0){
			printf("\n");
		}
		pcommand = pcommand->next;
	}
}
