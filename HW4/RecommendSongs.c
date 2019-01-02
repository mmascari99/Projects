/*
  Author: Michael mascari
  Email: mmascari2017@my.fit.edu
  Course: Algorithms & Data Structures
  Section: 2
  Description: hw4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct USER{
	double distance;
	char name[10];
	int ratings[10];
}USER;

typedef struct COMMAND{
	char name[30];
	char user[10];
	int songs[10];
	struct COMMAND* next;
}COMMAND;

typedef struct QNode{
	double distance;
	char name[10];
	struct QNode* next;
}QNode;

QNode* head = NULL;

COMMAND* CHead = NULL;
COMMAND* CPointer = NULL;

void read_user_file(USER* UList, int argc, char* argv[]); //reads the user file
void read_command_file(int argc, char* argv[]); //reads the command file
void make_commands(char name[], char user[], int songs[]); //inserts command into command linked list
void do_commands(USER* UList); //does the commands from the COMMAND list in order
void add_customer(USER* UList,  char name[], int ratings[]); //adds another user to the list alphabetically
void distance_ratings(USER* UList); //prints out the users aplhabetically with their distance ratings
void recommend_songs(USER* UList); //recommends songs to the important user @ Ulist[0]
void add_queue(USER* UList, char name[], int ratings[]); //adds a node to the queue
void CPrint(); //debug function
void QPrint(); //debug function
int make_heap(USER* UList, double help[]); //makes the heap
void min_heap(USER* UList, double help[], int length); //sorts the heap by minimum value
void closest(USER* UList, double help[], int length); //finds the closest customer using the ordered heap
int remove_min(USER* UList, double help[], int length, char name[]); //removes min then heapifies & removes user from queue

int main(int argc, char* argv[]){
	USER* UList = (USER*) malloc(101* sizeof(USER)); //101 to store the important user @ UList[0]
	read_user_file(UList, argc, argv);
	read_command_file(argc, argv);
	do_commands(UList);
	return 0;
}

//does the commands from the COMMAND list in order
void do_commands(USER* UList){
	CPointer = CHead;
	while(CPointer!= NULL){
		if(strcmp(CPointer->name, "AddCustomer")==0){			
			add_customer(UList, CPointer->user, CPointer->songs);
			printf("AddCustomer %s ", CPointer->user);
			for(int i = 0; i < 10; i++){
				printf("%d ", CPointer->songs[i]);
			}
			printf("\n");
			UList[0].ratings[0] = UList[0].ratings[0] + 1;
		}else if(strcmp(CPointer->name, "PrintCustomerDistanceRatings")==0){
			distance_ratings(UList);
		}else if(strcmp(CPointer->name, "RecommendSongs")==0){
			recommend_songs(UList);
		}
		CPointer = CPointer->next;
	}
	return;
}

//prints out the users aplhabetically with their distance ratings
void distance_ratings(USER* UList){
	printf("PrintCustomerDistanceRatings\n");
	printf("      %-10s ", UList[0].name);
	for(int j = 1; j < 11; j++){
		printf("%d ", UList[0].ratings[j]);
	}
	printf("\n");
	int i = 1;
	while(UList[i].name[0] != '\0'){
		if(UList[i].distance == -1){
			printf("----- ");
		}else{
			printf("%1.3f ", UList[i].distance);
		}
		 printf("%-10s ", UList[i].name);
		for(int j = 0; j < 10; j++){
			printf("%d ", UList[i].ratings[j]);
		}
		i++;
		printf("\n");
	}
	return;
}

//recommends songs to the important user @ Ulist[0]
void recommend_songs(USER* UList){
	printf("RecommendSongs");
	int length;
	double help[UList[0].ratings[0]];
	length = make_heap(UList, help);
	int depth = log(length)/log(2);
	for(int i = 0; i <= depth; i++){
		min_heap(UList, help, length);
	}
	closest(UList, help, length);
	
	return;
}

//finds closest costomer using sorted heap
void closest(USER* UList, double help[], int length){
	int i = 1;
	int four = 0;
	int useable = 0;
	char name[10];
	QNode* temp = head;
	while(temp->distance != help[0] && temp->distance > 0.0001){
		temp = temp->next;
	}
	while(strcmp(UList[i].name,temp->name)!=0){
		i++;
	}
	for(int j = 0; j < 10; j++){
		if(UList[0].ratings[j+1] == 0 && UList[i].ratings[j] > 4){
			four++;
		}
		if(UList[0].ratings[j+1] == 0 && UList[i].ratings[j] > 3){
			useable = 1;
		}
	}
	if(useable == 1){
		printf(" %s", UList[i].name);
		if(four > 0){
			for(int j = 0; j < 10; j++){
				if(UList[0].ratings[j+1] == 0 && UList[i].ratings[j] > 4){
					printf(" song%d 5", j);
				}
			}
		}else{
			for(int j = 0; j < 10; j++){
				if(UList[0].ratings[j+1] == 0 && UList[i].ratings[j] > 3){
					printf(" song%d 4", j);
				}
			}			
		}
		printf("\n");
	}else{		
		strcpy(name,UList[i].name);
		length = remove_min(UList, help, length, name);
		int depth = log(length)/log(2);
		for(int i = 0; i <= depth; i++){
			min_heap(UList, help, length);
		}
		closest(UList, help, length);
	}
	return;
}

int remove_min(USER* UList, double help[], int length, char name[]){
	QNode* temp = head;
	QNode* trailer = NULL;
	while(temp!= NULL && strcmp(name,temp->name)!=0){
		trailer = temp;
		temp = temp->next;
	}		
	if(strcmp(name,temp->name)==0){
		if(trailer == NULL){
			head = temp->next;
			printf("%s ", temp->name);
		}else{
			if(temp->next != NULL){
				trailer->next = temp->next;
				printf("%s -> %s", trailer->name, temp->next->name);
			}else{
				trailer->next = NULL;
			}	
		}
		free(temp);
		help[0] = help[length-1];
		length--;
		return length;
	}
}

//sets up the binary tree for the heap
int make_heap(USER* UList, double help[]){
	int total = 0;
	QNode* temp = head;
	for(int i = 0; i < UList[0].ratings[0]; i++){
		if(temp!=NULL){
			help[i] = temp->distance;
			temp = temp->next;
			total++;
		}
	}
	return total;
}

void min_heap(USER* UList, double help[], int length){
	int i = 0;
	double temp;
	while(2*i+2 <= length-1){
		if(help[2*i+1] < help[i]){
			temp = help[i];
			help[i] = help[2*i+1];
			help[2*i+1] = temp;
		}
		if(help[2*i+2] < help[i]){
			temp = help[i];
			help[i] = help[2*i+2];
			help[2*i+2] = temp;
		}
		i++;
	}
	if(2*i+1 <= length-1){
		if(help[2*i+1] < help[i]){
			temp = help[i];
			help[i] = help[2*i+1];
			help[2*i+1] = temp;
		}
	}
	return;
}

//adds each new user to the priority queue
void add_queue(USER* UList, char name[], int ratings[]){
	double distance;
	int total = 0;
	int user = 0;
	int S = 0;
	while(strcmp(name, UList[user].name)!=0){
		user++;
	}
	for(int i = 0; i < 10; i++){
		if(UList[0].ratings[i+1] > 0 && UList[user].ratings[i] > 0){
			S++;
			if(UList[0].ratings[i+1] - UList[user].ratings[i] > 0){
				total += UList[0].ratings[i+1] - UList[user].ratings[i];
			}else{
				total += UList[user].ratings[i] - UList[0].ratings[i+1];
			}
		}
	}
	distance = (1/(double)S) + ((1/(double)S)*(double)total);
	if(S == 0){
		UList[user].distance = -1; //updates UList distance for alphabetical printout
	}else{ //only makes a new node if the user has a distance
		QNode* NewQueue = (QNode*) malloc(sizeof(QNode));
		UList[user].distance = distance;
		NewQueue->distance = distance;
		strcpy(NewQueue->name,name);
		if(head == NULL){
			head = NewQueue;
			NewQueue->next = NULL;
		}else{
			QNode* temp = head;
			while(temp->next!=NULL){
				temp = temp->next;
			}
			temp->next = NewQueue;
		}
	}
	return;
}

//read in from the user input file
void read_user_file(USER* UList, int argc, char* argv[]){
	//starts @ 1 so important user can be stored @ UList[0]
	char name[10];
	int ratings[10];
	FILE * data_input = fopen(argv[1], "r");
	fscanf(data_input, "%s ", UList[0].name); //stores the important user @ UList[0] 
	UList[0].ratings[0] = 1;
	while(fscanf(data_input, "%s ", name)!= EOF){
		if(strcmp(name, UList[0].name)==0){
			for(int j = 0; j < 10; j++){
				fscanf(data_input, "%d", &ratings[j]);
				UList[0].ratings[j+1] = ratings[j];
			}
		}else{
			for(int j = 0; j < 10; j++){
				fscanf(data_input, "%d", &ratings[j]);
			}
			add_customer(UList, name, ratings);
			UList[0].ratings[0] = UList[0].ratings[0] + 1;
		}
	}
	return;
}

//adds a user to the user list alphabetically
void add_customer(USER* UList, char name[], int ratings[]){
	int j = UList[0].ratings[0];
	
	if(strcmp(name, UList[1].name)<0 || UList[0].ratings[0] == 1){
		while(j > 1){
			UList[j] = UList[j-1];
			j--;
		}
		strcpy(UList[1].name,name);
		for(int k = 0; k < 10; k++){
			UList[1].ratings[k] = ratings[k];
		}
	}else{
		int i = 1;
		while(strcmp(name, UList[i].name)>0 && i < UList[0].ratings[0]){
			i++;
		}
		if(strcmp(name, UList[i].name)<0){
			while(j > i){
				UList[j] = UList[j-1];
				j--;
			}
			strcpy(UList[i].name,name);
			for(int k = 0; k < 10; k++){
				UList[i].ratings[k] = ratings[k];
			}
			add_queue(UList, name, ratings); //adds the user to the Priority queue
			return;
		}
		strcpy(UList[UList[0].ratings[0]].name,name);
		for(int k = 0; k < 10; k++){
			UList[UList[0].ratings[0]].ratings[k] = ratings[k];
		}
	}
	add_queue(UList, name, ratings); //adds the user to the Priority queue
	return;
}

//read in from command input file
void read_command_file(int argc, char* argv[]){
	char name[30]; //name of the command
	char user[10]; //if AddCustomer, this is the user's name
	int songs[10]; //if AddCustomer, this is the user's ratings
	FILE * data_input = fopen(argv[2], "r");
	while(fscanf(data_input, "%s ", name)!= EOF){
		if(strcmp(name,"AddCustomer")==0){
			fscanf(data_input, "%s ", user);
			for(int i = 0; i < 10; i++){
				fscanf(data_input, "%d", &songs[i]);
			}
		}
		make_commands(name, user, songs); //inserts new data into command linked list
	}
	return;
}

//makes the command linekd list
void make_commands(char name[], char user[], int songs[]){
	COMMAND* NewCommand = (COMMAND*) malloc(sizeof(COMMAND));
	strcpy(NewCommand->name,name);
	strcpy(NewCommand->user,user);
	if(strcmp(name,"AddCustomer")==0){
		for(int i = 0; i < 10; i++){
			NewCommand->songs[i] = songs[i];
		}
	}
	NewCommand->next = NULL;
	if(CHead == NULL){
		CHead = NewCommand;
		CPointer = NewCommand;
	}else{
		CPointer->next = NewCommand;
		CPointer = NewCommand;
	}
	return;
}

//debug function to make sure the COMMAND list is in order
void CPrint(){
	COMMAND* temp = CHead;
	while(temp!= NULL){
		printf("%s", temp->name);
		if(strcmp(temp->name,"AddCustomer")==0){
			printf(" %s ", temp->user);
			for(int i = 0; i < 10; i++){
				printf("%d ", temp->songs[i]);
			}
		}
		printf("\n");
		temp = temp->next;
	}
	return;
}

//debug function to make sure the Priority Queue exists
void QPrint(){
	QNode* temp = head;
	while(temp!=NULL){
		printf("%1.3f %s\n", temp->distance, temp->name);
		temp = temp->next;
	}
	return;
}
