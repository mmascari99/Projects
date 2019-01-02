/*
  Author: Michael Mascari
  Email: mmascari2017@my.fit.edu
  Course: 2010
  Section: 2
  Description: HW1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//representative struct. keeps track of their name, if they are helping someone and the next represenative
typedef struct REP{
	char name[6];
	int busy;//1 for in call. 0 for not busy
	struct REP* next;
}REP;

//costomer struct. keeps track of their name, who is helping them, time they want to be helped for, and next in line
typedef struct COSTOMER{
	char name;
	int time; //time called
	char helped[6]; //name of helping rep
	int duration; //length of call
	int waiting; //time spent waiting
	struct COSTOMER* next;
}COSTOMER;

typedef struct COMMAND{
	struct COMMAND* previos;
	char command[23]; //command name
	int time; //time command happens
	char costomer;
	int chatLength;
	char waitORlater[6]; //wait or later field
	struct COMMAND* next;
}COMMAND;

//heads of the lists
REP* Rhead = NULL;
COSTOMER* Chead = NULL;
COMMAND* COMMANDhead = NULL;
int maxTime = 0;

void do_commands(); //removes oldest command and excecutes it
void check_calls(int time); //checks how long all calls have gone for and if they should end.
void check_rep(int time);//assigns people from on hold to a call
void clean_up(); //cleans up after COMMAND list is done
void end_call(char name); //ends calls that go too long
void make_rep_list(); //orders the representatives according to HW instructions
void make_command(char command[23], int time, char costomer, int chatLength, char waitORlater[6]); //makes a new node in the command linked list
void chat_request(char name, int time, int duration, char waitORlater[6]);
void available_rep(); //prints out all the available representatives
int quit_on_hold(char costomer, int time);
void max_wait_time(int time); //finds the return value of the PrintMaxWaitTime function
int make_minutes(int time); //converts HHMM format into minutes
void make_hours(int time); //converts minute format into HHMM
void Rprint(); //debug function
void COMMANDprint(); //debug function
void Cprint();//debug function

int main(int argc, char* argv[]){
	for(int i = 0; i < 5; i++){ //for loop 5 times because 5 representatives
		make_rep_list();
	}	
	char command[23];//command given to the code to run
	int time; //timestamp on the input
	char costomer; //who is making the command
	int chatLength; //how long the costomer will talk for
	char waitORlater[6];//wait or later input
	FILE * data_input = fopen(argv[1], "r");
	
	//scans in the correct amount of fields for each command
	while(fscanf(data_input, "%s ", command)!= EOF){
		if(strcmp(command, "ChatRequest")==0){
			fscanf(data_input, "%d %c %d %s ", &time, &costomer, &chatLength, waitORlater);
		} else 	if(strcmp(command, "QuitOnHold")==0){
			fscanf(data_input, "%d %c ", &time, &costomer);
		} else 	if(strcmp(command, "PrintAvailableRepList")==0){
			fscanf(data_input, "%d ", &time);
		}  else if(strcmp(command, "PrintMaxWaitTime")==0){
			fscanf(data_input, "%d ", &time);
		}
		//printf("%s ", command);
		time = make_minutes(time);
		make_command(command, time, costomer, chatLength, waitORlater);
	}
	fclose(data_input);
	do_commands();	
	while(Chead != NULL){
		clean_up();
	}
	return 0;
}

//function for deciding what to do with each file instruction
void do_commands(){
	int last; //time for current command
	int duration; //time for duration of a node
	COMMAND* COMMANDp = COMMANDhead;
	while(COMMANDp->next != NULL){
		last = COMMANDp->time;
		check_calls(last); //makes sure ended calls are over before seeing if people want to make more
		check_rep(last); //checks if there are people waiting before doing a command
		
		//does each command according to its name
		if(strcmp(COMMANDp->command,"ChatRequest")==0){
			printf("ChatRequest ");
			make_hours(COMMANDp->time);
			printf("%c %d %s\n", COMMANDp->costomer, COMMANDp->chatLength, COMMANDp->waitORlater);
			chat_request(COMMANDp->costomer, COMMANDp->time, COMMANDp->chatLength, COMMANDp->waitORlater);
		}else if(strcmp(COMMANDp->command,"QuitOnHold")==0){
			printf("QuitOnHold ");
			make_hours(COMMANDp->time);
			printf("%c\n", COMMANDp->costomer);
			last = quit_on_hold(COMMANDp->costomer, last);
			if(last > maxTime){
				maxTime = last;
			}
		}else if(strcmp(COMMANDp->command,"PrintAvailableRepList")==0){
			last = COMMANDp->time;
			printf("PrintAvailableRepList ");
			make_hours(last);
			available_rep();		
		}else if(strcmp(COMMANDp->command,"PrintMaxWaitTime")==0){
			last = COMMANDp->time;
			printf("MaxWaitTime ");
			make_hours(last);
			max_wait_time(last);
			printf("%d\n", maxTime);
		}
		COMMANDp = COMMANDp->next;
			
	}

		//don't mind the large chunk of code that is the same as above. Just meant to do the final command in the list. nicer ways? yes. Easier ways? no.
		last = COMMANDp->time;
		check_calls(last);
		check_rep(last);
		if(strcmp(COMMANDp->command,"ChatRequest")==0){
			printf("ChatRequest ");
			make_hours(COMMANDp->time);
			printf("%c %d %s\n", COMMANDp->costomer, COMMANDp->chatLength, COMMANDp->waitORlater);
			chat_request(COMMANDp->costomer, COMMANDp->time, COMMANDp->chatLength, COMMANDp->waitORlater);
		}else if(strcmp(COMMANDp->command,"QuitOnHold")==0){
			printf("QuitOnHold ");
			make_hours(COMMANDp->time);
			printf("%c\n", COMMANDp->costomer);
			last = quit_on_hold(COMMANDp->costomer, last);
			if(last > maxTime){
				maxTime = last;
			}
		}else if(strcmp(COMMANDp->command,"PrintAvailableRepList")==0){
			last = COMMANDp->time;
			printf("PrintAvailableRepList ");
			make_hours(last);
			available_rep();
		}else if(strcmp(COMMANDp->command,"PrintMaxWaitTime")==0){
			last = COMMANDp->time;
			printf("MaxWaitTime ");
			make_hours(last);
			max_wait_time(last);
			printf("%d\n", maxTime);
		}
	return;
}

//cleans up after command list is empty
void clean_up(){
	int first; //red herring varable that if changed means it found something
	char name; //tempcostomers name
	COSTOMER* TempCostomer;
	
	//finds the first call to end after all commands have finnished
	while(Chead->next !=NULL){
		first = 10000;
		TempCostomer = Chead;
		while(TempCostomer->next != NULL){
			if(strcmp(TempCostomer->helped,"Alice")==0 ||strcmp(TempCostomer->helped,"Bob")==0 ||strcmp(TempCostomer->helped,"Carol")==0 ||strcmp(TempCostomer->helped,"David")==0 ||strcmp(TempCostomer->helped,"Emily")==0){
				if(TempCostomer->time + TempCostomer->duration < first){
					first = TempCostomer->time + TempCostomer->duration;
					name = TempCostomer->name;
					TempCostomer = TempCostomer->next;
				}else{
					TempCostomer = TempCostomer->next;
				}
			}else{
				TempCostomer = TempCostomer->next;
			}
		}
		//needs to end the call it found plus check for a new costomer
		end_call(name);
		check_rep(first);
	}
	
	//makes sure the last costomer hangs up
	TempCostomer = Chead;
	end_call(TempCostomer->name);
	return;
}

//assigns people from on hold to a call
void check_rep(int time){
	COSTOMER* TempCostomer;
	REP* TempRep = Rhead;
	if(Chead != NULL){
		TempCostomer = Chead;
		//finds an open rep
		while(TempRep->busy == 1 && TempRep->next !=NULL){
			TempRep = TempRep->next;
		}
		if(TempRep->next ==NULL && TempRep->busy == 1){
			return;
		}
		
		//finds an open cosotmer
		while((strcmp(TempCostomer->helped,"Alice")==0 ||strcmp(TempCostomer->helped,"Bob")==0 ||strcmp(TempCostomer->helped,"Carol")==0 ||strcmp(TempCostomer->helped,"David")==0 ||strcmp(TempCostomer->helped,"Emily")==0)&&TempCostomer->next !=NULL){
			TempCostomer = TempCostomer->next;
		}
		if(TempCostomer->next ==NULL && (strcmp(TempCostomer->helped,"Alice")==0 ||strcmp(TempCostomer->helped,"Bob")==0 ||strcmp(TempCostomer->helped,"Carol")==0 ||strcmp(TempCostomer->helped,"David")==0 ||strcmp(TempCostomer->helped,"Emily")==0)){
			return;
		}
		
		//puts them on a call together
		TempRep->busy = 1;
		strcpy(TempCostomer->helped,TempRep->name);
		TempCostomer->waiting = time - TempCostomer->time;
		TempCostomer->time = time;
		printf("RepAssignment %c %s ", TempCostomer->name, TempRep->name);
		make_hours(time);
		printf("\n");
		check_rep(time);
	}
	
	return;
}

//makes sure people hung up on time
void check_calls(int time){
	COSTOMER* TempCostomer;
	int first; //red  herring again
	char name;
	if(Chead != NULL){
		TempCostomer = Chead;
		first = 10000; //any time value will be below this so any call will proc
		while(TempCostomer->next != NULL){
			if(strcmp(TempCostomer->helped,"Alice")==0 ||strcmp(TempCostomer->helped,"Bob")==0 ||strcmp(TempCostomer->helped,"Carol")==0 ||strcmp(TempCostomer->helped,"David")==0 ||strcmp(TempCostomer->helped,"Emily")==0){
				if(TempCostomer->time + TempCostomer->duration < first && TempCostomer->time + TempCostomer->duration < time){
					first = TempCostomer->time + TempCostomer->duration;
					name = TempCostomer->name;
					TempCostomer = TempCostomer->next;
				}else{
					TempCostomer = TempCostomer->next;
				}
			}else{
				TempCostomer = TempCostomer->next;
			}
		}
		
		//makes sure a call proced
		if(first != 10000){
			end_call(name);
			check_rep(first);
			check_calls(time);
		}
	}
	return;
}

//like it could be worse. Just saying
void end_call(char name){
	COSTOMER* TempCostomer;
	COSTOMER* PrevCostomer;
	REP* PrevRep;
	REP* TempRep;
	if(Chead != NULL){
		TempCostomer = Chead;
		TempRep = Rhead;
		
		if(TempCostomer->name == name){
			//first possibility, if they are both at the front of their list
			if(strcmp(TempRep->name,TempCostomer->helped)==0){
				Rhead = Rhead->next;
				TempRep->busy = 0;
				PrevRep = Rhead;
				while(PrevRep->next != NULL){
					PrevRep = PrevRep->next;
				}
				PrevRep->next = TempRep;
				TempRep->next = NULL;				
			}else{
				//second probability, if the rep is not at the front, but costomer is
				while(strcmp(TempRep->name,TempCostomer->helped)!=0&& TempRep->next != NULL){
					PrevRep = TempRep;
					TempRep = TempRep->next;
				}
				TempRep->busy = 0;
				PrevRep->next = TempRep->next;
				while(PrevRep->next != NULL){
					PrevRep = PrevRep->next;
				}
				PrevRep->next = TempRep;
				TempRep->next = NULL;
				//Rprint();
			}
			printf("ChatEnded %c %s ", TempCostomer->name, TempCostomer->helped);
			make_hours((TempCostomer->time + TempCostomer->duration));
			printf("\n");
			Chead = TempCostomer->next;
			check_rep((TempCostomer->time + TempCostomer->duration));
			if(TempCostomer->waiting > maxTime){
				maxTime = TempCostomer->waiting;
			}
			free(TempCostomer);
			return;
			
		//where the third possibility can be found
		}if(TempCostomer->name != name){
			while(TempCostomer->next != NULL){
				if(TempCostomer->name == name){
					while(strcmp(TempRep->name,TempCostomer->helped)!=0 && TempRep->next !=NULL){
						PrevRep = TempRep;
						TempRep = TempRep->next;
					}
					if(strcmp(TempRep->name,TempCostomer->helped)==0){
						TempRep->busy = 0;
						PrevRep->next = TempRep->next;
						while(PrevRep->next != NULL){
							PrevRep = PrevRep->next;
							
						}
						//we need to comment every five lines? but these 20 lines do the same thing.
						PrevRep->next = TempRep;
						TempRep->next = NULL;
						printf("ChatEnded %c %s ", TempCostomer->name, TempCostomer->helped);
						make_hours((TempCostomer->time + TempCostomer->duration));
						printf("\n");
						PrevCostomer->next = TempCostomer->next;
						check_rep((TempCostomer->time + TempCostomer->duration));
						if(TempCostomer->waiting > maxTime){
							maxTime = TempCostomer->waiting;
						}
						free(TempCostomer);
						return;
					}
				}
				PrevCostomer = TempCostomer;
				TempCostomer = TempCostomer->next;
			}
		}
	}
	return;
}
//runs the ChatRequest processes
void chat_request(char name, int time, int duration, char waitORlater[6]){
	REP* TempRep = Rhead; //haha
	COSTOMER* NewCostomer = (COSTOMER*) malloc(sizeof(COSTOMER));
	COSTOMER* TempCostomer;
	
	//fills in all the fields for a new costomer node
	NewCostomer->next = NULL;			
	NewCostomer->name = name;
	NewCostomer->time = time;
	NewCostomer->duration = duration;
	
	//figures out where to put it
	if(Chead == NULL){
		Chead = NewCostomer;
	}else{
		TempCostomer = Chead;
		while(TempCostomer->next != NULL){
			TempCostomer = TempCostomer->next;
		}
		TempCostomer->next = NewCostomer;
	}
	
	//finds a rep to give the costomer
	while(TempRep !=NULL){
		if(TempRep->busy == 1){
			TempRep = TempRep->next;
		}else{
			TempRep->busy = 1;
			printf("RepAssignment %c %s ", name, TempRep->name);
			make_hours(time);
			printf("\n");
			strcpy(NewCostomer->helped,TempRep->name);
			return;
		}
	}
	
	//if there are no reps left, refer to waitORlater field
	if(strcmp(waitORlater,"later")==0){
		COSTOMER* PrevCostomer;
		PrevCostomer = Chead;
		while(PrevCostomer->next != NewCostomer){
			PrevCostomer = PrevCostomer->next;
		}
		PrevCostomer->next = NULL;
		printf("TryLater %c ", name);
		make_hours(time);
		free(NewCostomer);
		printf("\n");
	}else{
		TempCostomer->next = NewCostomer;
		NewCostomer->next = NULL;
		strcpy(NewCostomer->helped,"NO");
		printf("PutOnHold %c ", name);
		make_hours(time);
		printf("\n");
	}
	return;
}

//Runs the QuitOnHold processes
int quit_on_hold(char costomer, int time){
	COSTOMER* TempCostomer;
	COSTOMER* PrevCostomer;
	int waiting = 0;
	if(Chead != NULL){
		TempCostomer = Chead;
		if(TempCostomer->name == costomer){
			Chead = TempCostomer->next;
			waiting = (time - TempCostomer->time); 
			free(TempCostomer);
			return waiting;
		}
		while(TempCostomer->name != costomer && TempCostomer->next!=NULL){
			PrevCostomer = TempCostomer;
			TempCostomer = TempCostomer->next;
		}
		if(TempCostomer->name == costomer){
			waiting = (time - TempCostomer->time); 
			if(TempCostomer->next != NULL){
			PrevCostomer->next = TempCostomer->next;
			free(TempCostomer);
			}else{
				PrevCostomer->next = NULL;
				free(TempCostomer);
			}
		}
	}
	return waiting;
}

//excecutes PrintMaxWaitTime instructions
void max_wait_time(int time){

//nothing to see here, y'all aren't doing actual max wait times and im pissed cause I wrote a whole funtion to keep track of everything
	return;
}

//converts HHMM format into minutes
int make_minutes(int time){
	int startingMinutes;
	startingMinutes = time % 100;
	time = (time/100)*60;
	time = time + startingMinutes;
	return time;
}

//converts minutes into HHMM format
void make_hours(int time){
	int minutes = time % 60;
	time = (time - minutes)/60;
	if(time < 10 && minutes < 10){
		printf("0%d0%d ", time, minutes);
	}else if(time >= 10 && minutes < 10){
		printf("%d0%d ", time, minutes);
	}else if(time < 10 && minutes >= 10){
		printf("0%d%d ", time, minutes);
	}else if(time >= 10 && minutes >= 10){
		printf("%d%d ", time, minutes);
	}
	return;
}

//debug function
void COMMANDprint(){
	COMMAND* TempCommand;
		if(COMMANDhead != NULL){
		TempCommand = COMMANDhead;
		while(TempCommand != NULL){
			printf("%s ", TempCommand->command);
			if(TempCommand->time < 1000){ //conforms the time printf to the HW requirements
				printf("0%d ", TempCommand->time);
			}else{
				printf("%d ", TempCommand->time);
			}
			if(strcmp(TempCommand->command,"ChatRequest")==0){
				printf("%c %d %s\n", TempCommand->costomer, TempCommand->chatLength, TempCommand->waitORlater);
			}else if(strcmp(TempCommand->command,"QuitOnHold")==0){
				printf("%c\n", TempCommand->costomer);
			} else{
				printf("\n");
			}
			TempCommand = TempCommand->next;
		}
	}
	return;
}

//makes a new node in the command linked list
void make_command(char command[23], int time, char costomer, int chatLength, char waitORlater[6]){ 
	COMMAND* NewCommand = (COMMAND*) malloc(sizeof(COMMAND));
	COMMAND* TempCommand;
	NewCommand->next = NULL;
	strcpy(NewCommand->command,command);
	NewCommand->time = time;
	NewCommand->costomer = costomer;
	NewCommand->chatLength = chatLength;
	strcpy(NewCommand->waitORlater,waitORlater);	
	if(COMMANDhead == NULL){
		COMMANDhead = NewCommand;
		NewCommand->previos = NULL;
	}else{
		TempCommand = COMMANDhead;
		while(TempCommand->next != NULL){
			TempCommand = TempCommand->next;
		}
		TempCommand->next = NewCommand;
		NewCommand->previos = TempCommand;
	}
}

//places the representatives in their preknown order in their linked list.
void make_rep_list(int rep){ 
	REP* NewRep = (REP*) malloc(sizeof(REP));
	REP* TempRep; //haha
	NewRep->next = NULL;
	NewRep->busy = 0;
	if(Rhead == NULL){
		strcpy(NewRep->name, "Alice"); //Alice goes first down the rabbit hole
		Rhead = NewRep;
	}else{
		TempRep = Rhead;
		while(TempRep->next != NULL){ //goes to the end of the list so the NewRep can go next
			TempRep = TempRep->next;
			
		}
		if(strcmp(TempRep->name,"Alice") == 0){ //places the remaining reepresentatives in their correct order
			strcpy(NewRep->name,"Bob");
		} else if(strcmp(TempRep->name,"Bob") == 0){
			strcpy(NewRep->name,"Carol");
		} else if(strcmp(TempRep->name,"Carol") == 0){
			strcpy(NewRep->name,"David");
		} else if(strcmp(TempRep->name,"David") == 0){
			strcpy(NewRep->name,"Emily");
		}
		TempRep->next = NewRep; 
	}
	return;
}

//runs through the representatives linked list and prints the names of everyone not working
void available_rep(){
	REP* tempnode;
		tempnode = Rhead;
		for(int i = 0; i < 5; i++){//loops 5 times because 5 representatives
			if(tempnode->busy == 0){ //checks their busy element. 1 for busy, 0 for not.
				printf("%s ", tempnode->name);
			}
			tempnode = tempnode->next;
		}
		printf("\n");
	return;
}

//debug function to make sure represenative list is working well
void Rprint(){
	REP* tempnode;
	if(Rhead != NULL){
		tempnode = Rhead;
		for(int i = 0; i < 5; i++){//loops 5 times because 5 representatives
				printf("%s:%d:", tempnode->name, tempnode->busy);
				if(tempnode->next !=NULL){
					printf(":%s		", tempnode->next->name);
				}
			tempnode = tempnode->next;
		}
		printf("\n");
	}
	return;
}

//debug function that prints the customer list
void Cprint(){
	COSTOMER* TempCostomer;
	if(Chead != NULL){
		TempCostomer = Chead;
		while(TempCostomer != NULL){
			printf("%c:%d:%s:%d", TempCostomer->name, TempCostomer->time, TempCostomer->helped, TempCostomer->duration);
			if(TempCostomer->next!=NULL){
				printf(":%c ", TempCostomer->next->name);
			}
			TempCostomer = TempCostomer->next;
		}
		printf("\n");
	}
	return;
}
