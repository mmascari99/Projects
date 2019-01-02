/*
  Author: Michael Mascari
  Email: mmascari2017@my.fit.edu
  Course: Algorithms & Data Structures
  Section: 2
  Description: HW3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//tree struct
typedef struct TREE{
	char name[40]; //stores the name of the tree node
	struct TREE* parent; //points to the parent node
	struct TREE* children[100]; //array of children tree pointers to point to as many children nodes as possible
}TREE;

//command struct
typedef struct COMMAND{
	char name[25]; //stores the name
	char first[40]; //stores the first tree node name
	char second[40]; //can store a second tree node name
	struct COMMAND* next; //points to the next command
}COMMAND;

//all head, root, and pointer nodes
TREE* THead = NULL;
COMMAND* CHead = NULL;
COMMAND* CPointer = NULL;

void read_file(int argc, char* argv[]); //reads from the file
void make_tree(char parent[], char name[], TREE* TempNode, TREE* NewTree); //makes the new node on the tree
void tree_node(char parent[], char name[], TREE* TempNode); //sets up the new node for the tree
void read_commands(int argc, char* argv[]); //reads from the command file
void make_commands(char name[], char first[], char second[]); //sets up the commands linked list
void do_commands(); //runs through the command list and executes the commands
void direct_supervisor(TREE* temp); //fufills the directsupivisor command
void direct_subordinates(TREE* temp); //fufills the directsubordinates command
void all_supervisors(); //fufills the AllSupervisors command
void all_subordinates(COMMAND* help, TREE* temp); //fufills the AllSubordinates command
int num_supervisors(COMMAND* help, TREE* temp, int total); //fufills the NumberOfAllSupervisors command
int num_subordinates(COMMAND* help, TREE* temp, int total); //fufills the NumberOfAllSubordinates command
void is_supervisor(TREE* temp); //fufills the IsSupervisor command
void is_subordinate(TREE* temp); //fufills the IsSubordinate command
void closest_common_supervisor(COMMAND* help, TREE* temp); //fufills the ClosestCommonSupervisor command
int CCS_Supervisors(COMMAND* help, COMMAND* help2, TREE* temp, char common[], int done); //helps with closest common supervisor
int all_CCSsupervisors(COMMAND* help2, TREE* temp, char common[], int done); //helps with closest common supervisor
void CPrint(); //debug function 

int main(int argc, char* argv[]){
	read_file(argc, argv);
	read_commands(argc, argv);
	do_commands();
	return 0;
}

//executes each command from the command list
void do_commands(){
	int total, other; //to store totals found/compare totals found
	TREE* temp; //for a needed pointer
	COMMAND* help; //for a needed pointer
	CPointer = CHead; 
	while(CPointer!=NULL){
		//reassigns all variables to starting positions for each loop
		total = 0;
		other = 0;
		temp = THead;
		help = CPointer;
		//enters the command it was told to run. no need for further comments in this function, because that is alll this function does for a very long time
		if(strcmp(CPointer->name,"DirectSupervisor")==0){
			direct_supervisor(temp);
		}else if(strcmp(CPointer->name,"DirectSubordinates")==0){
			printf("DirectSubordinates %s ", CPointer->first);
			direct_subordinates(temp);
			printf("\n");
		}else if(strcmp(CPointer->name,"AllSupervisors")==0){
			printf("AllSupervisors %s ", CPointer->first);
			all_supervisors(help, temp);
			printf("\n");
		}else if(strcmp(CPointer->name,"AllSubordinates")==0){
			printf("AllSubordinates %s ", CPointer->first);
			all_subordinates(help, temp);
			printf("\n");
		}else if(strcmp(CPointer->name,"NumberOfAllSupervisors")==0){
			printf("NumberOfAllSupervisors %s ", CPointer->first);
			total = num_supervisors(help, temp, total);
			printf("%d\n", total);
		}else if(strcmp(CPointer->name,"NumberOfAllSubordinates")==0){
			printf("NumberOfAllSubordinates %s ", CPointer->first);
			total = num_subordinates(help, temp, total);
			printf("%d\n", total);
		}else if(strcmp(CPointer->name,"IsSupervisor")==0){
			printf("IsSupervisor %s %s", CPointer->first, CPointer->second);
			is_supervisor(temp);
		}else if(strcmp(CPointer->name,"IsSubordinate")==0){
			printf("IsSubordinate %s %s", CPointer->first, CPointer->second);
			is_subordinate(temp);
		}else if(strcmp(CPointer->name,"CompareRank")==0){
			printf("CompareRank %s %s ", CPointer->first, CPointer->second);
			total = num_supervisors(help, temp, total);
			strcpy(help->first, CPointer->second);
			temp = THead;
			other = num_supervisors(help, temp, other);
			//printf("%d %d\n", total, other);
			if(total > other){
				printf("lower\n");
			}else if(total < other){
				printf("higher\n");
			}else{
				printf("same\n");
			}
		}else if(strcmp(CPointer->name,"ClosestCommonSupervisor")==0){
			printf("ClosestCommonSupervisor %s %s", CPointer->first, CPointer->second);
			closest_common_supervisor(help, temp);
		}
		CPointer = CPointer->next;
	}
	return;
}

//main ccs function
void closest_common_supervisor(COMMAND* help, TREE* temp){
	int done = 0; //a flag for if it found the supervisor
	COMMAND* help2 = CPointer;
	strcpy(help2->first, CPointer->second);
	char common[100];
	done = CCS_Supervisors(help, help2, temp, common, done);
	
	return;
}

//outside nested loop function
int CCS_Supervisors(COMMAND* help, COMMAND* help2, TREE* temp, char common[], int done){
	int i = 0;
	
	//loops through each possible supervisor for each input. 
	//does so like a nested loop
	if(strcmp(temp->name,help->first)==0){
		strcpy(common, temp->parent->name);
		done += all_CCSsupervisors(help2, temp, common, done);
		if(strcmp(THead->name,temp->parent->name)!=0){
		strcpy(help->first, temp->parent->name);
		temp = THead;		
		done += CCS_Supervisors(help, help2, temp, common, done);
		}
		return 0;
	}
	while(temp->children[i] != NULL){
		done += CCS_Supervisors(help, help2, temp->children[i], common, done);
		i++;
	}
	return 0;
}

//inside nested loop funstion
int all_CCSsupervisors(COMMAND* help2, TREE* temp, char common[], int done){
	int i = 0;
	//here is the inside of the nestd loop. 
	//returns one if it find what it is looking for
	if(strcmp(temp->name,help2->first)==0){
		if(strcmp(common,temp->parent->name)==0 && done < 1){
			printf(" %s\n", common);
			return 1; //returns one because it found what it was looking for
		}
		if(strcmp(THead->name,temp->parent->name)!=0){
		strcpy(help2->first, temp->parent->name);
		temp = THead;		
		done += all_CCSsupervisors(help2, temp, common, done);
		}
		return 0;
	}
	while(temp->children[i] != NULL){
		done += all_CCSsupervisors(help2, temp->children[i], common, done);
		i++;
	}
	return 0;
}

//recursively finds the direct supervisor
void direct_supervisor(TREE* temp){
	int i = 0;
	while(temp->children[i]!=NULL){
		if(strcmp(temp->children[i]->name,CPointer->first)==0){
				printf("DirectSupervisor %s %s\n", CPointer->first, temp->children[i]->parent); //prints out if it finds what it is looking for then returns
				return;
			}
		i++;
	}
	i = i-1;
	while(i >= 0){
		direct_supervisor(temp->children[i]);//names a smaller tree out of the 'roots' children
		i--;
	}
	return;
}

//recursively finds the direct subordinates
void direct_subordinates(TREE* temp){
	int i = 0;
	if(strcmp(temp->name,CPointer->first)==0){
		while(temp->children[i] != 0){
			printf("%s ", temp->children[i]->name); //prints out f it finds what it is looking for
			i++;
		}
		return;
	}else{
		while(temp->children[i] != NULL){
			direct_subordinates(temp->children[i]); //makes a tree of its children if it does not find what it is looking for
			i++;
		}
	}
	return;
}

//recursevly finds all supervisors
void all_supervisors(COMMAND* help, TREE* temp){
	int i = 0;
	if(strcmp(temp->name,help->first)==0){
		printf("%s ", temp->parent->name); //prinds out the parent node
		if(strcmp(THead->name,temp->parent->name)!=0){
		strcpy(help->first, temp->parent->name);
		temp = THead;		
		all_supervisors(help, temp); //only ends if it finds the root node
		}
		return;
	}
	while(temp->children[i] != NULL){
		all_supervisors(help, temp->children[i]);
		i++;
	}
	return;
}

//prints out all subordinates
void all_subordinates(COMMAND* help, TREE* temp){
	int i = 0;
	if(strcmp(temp->name, help->first)==0){
		while(temp->children[i] != 0){
			printf("%s ", temp->children[i]->name); //prints if it finds a subordinate
			strcpy(help->first, temp->children[i]->name);
			all_subordinates(help, temp->children[i]); //recursive
			
			i++;
		}
		return;
	}else{
		while(temp->children[i] != NULL){
			all_subordinates(help, temp->children[i]); //recursive
			i++;
		}
	}
	return;
}

//finds the numebr of all supervisors
int num_supervisors(COMMAND* help, TREE* temp, int total){
	int i = 0;
	if(strcmp(temp->name,help->first)==0){
		
		if(temp->parent!=NULL){
			total++; //adds to total when it finds a parent
			if(strcmp(THead->name,temp->parent->name)!=0){
				strcpy(help->first, temp->parent->name);
				temp = THead;		
				total = num_supervisors(help, temp, total); //recursive
			}
		}
		return total;//ends when there are no supervisors left and returns total
	}
	while(temp->children[i] != NULL){
		total = num_supervisors(help, temp->children[i], total); //recursive
		i++;
	}
	return total; //ends when there are no supervisors left and returns total
}

//finds the number of all subordinates
int num_subordinates(COMMAND* help, TREE* temp, int total){
		int i = 0;
		int excess;
	if(strcmp(temp->name, help->first)==0){
		while(temp->children[i] != 0){
			total++; //adds one each time
			strcpy(help->first, temp->children[i]->name);
			total = num_subordinates(help, temp->children[i], total);
			
			i++;
		}
		return total; //ends when there are no subordinates left and returns total
	}else{
		while(temp->children[i] != NULL){
			total = num_subordinates(help, temp->children[i], total); //recursive
			i++;
		}
	}
	return total; //ends when there are no subordinates left and returns total
}

//recursevly finds if somethign is a supervisor of another
void is_supervisor(TREE* temp){
	int i = 0;
	if(strcmp(temp->name,CPointer->first)==0){
		if(strcmp(temp->parent->name,CPointer->second)==0){
			printf(" yes\n");
		}else{
			printf(" no\n");
		}
		return;
	}else{
		while(temp->children[i] != NULL){
			is_supervisor(temp->children[i]); //recursive
			i++;
		}
	}
	return;
}

//recursevly finds if somethign is a subordinate of another
void is_subordinate(TREE* temp){
	int i = 0;
	if(strcmp(temp->name,CPointer->first)==0){
		while(temp->children[i] != 0){
			if(strcmp(temp->children[i]->name,CPointer->second)==0){
				printf(" yes\n");
				return;
			}
				i++;
			}
		printf(" no\n");
		return;
	}else{
		while(temp->children[i] != NULL){
			is_subordinate(temp->children[i]); //recursive
			i++;
		}
	}
	return;
}

//read in from command input file
void read_commands(int argc, char* argv[]){
	char name[25];
	char first[40];
	char second[40];
	FILE * data_input = fopen(argv[2], "r");
	while(fscanf(data_input, "%s %s ", name, first)!= EOF){
		if(strcmp(name,"ClosestCommonSupervisor")==0 || strcmp(name,"IsSubordinate")==0 || strcmp(name,"CompareRank")==0 ||strcmp(name,"IsSupervisor")==0){
			fscanf(data_input, "%s ", second);
		}
		make_commands(name, first, second); //inserts new data into command linked list
		//printf("%s %s %s\n", name, first, second);
	}
	return;
}

//makes the command linekd list
void make_commands(char name[], char first[], char second[]){
	COMMAND* NewCommand = (COMMAND*) malloc(sizeof(COMMAND));
	strcpy(NewCommand->name,name);
	strcpy(NewCommand->first,first);
	if(strcmp(name,"ClosestCommonSupervisor")==0 || strcmp(name,"IsSubordinate")==0 || strcmp(name,"CompareRank")==0 ||strcmp(name,"IsSupervisor")==0){
		strcpy(NewCommand->second,second);
		//printf("%s\n", NewCommand->second);
	}
	NewCommand->next = NULL;
	if(CHead == NULL){
		CHead = NewCommand;
		CPointer = NewCommand;
	}else{
		CPointer->next = NewCommand;
		CPointer = NewCommand;
	}
	//printf("%s %s %s\n", CPointer->name, CPointer->first, CPointer->second);
	return;
}

//reads fromt the make tree input file
void read_file(int argc, char* argv[]){
	char word[40];
	char parent[40];
	FILE * data_input = fopen(argv[1], "r");
	fscanf(data_input, "%s ", word);
	TREE* root = (TREE*) malloc(sizeof(TREE));	
	root->parent = NULL;
	strcpy(root->name,word);
	//printf("%s\n", word);
	THead = root;
	TREE* TempNode;
	while(fscanf(data_input, "%s %s ", parent, word)!= EOF){
		TempNode = root;
		tree_node(parent, word, TempNode); //makes a new tree node with what it read in
	}
	return;
}

//simplifies the make_tree function
void tree_node(char parent[], char name[], TREE* TempNode){
	TREE* NewTree = (TREE*) malloc(sizeof(TREE));
	strcpy(NewTree->name,name);
	make_tree(parent, name, TempNode, NewTree);
	//NewTree->parent = TempNode;
	return;
}

//makes each tree node
void make_tree(char parent[], char name[], TREE* TempNode, TREE* NewTree){
	int i = 0;
	//printf("%s: ", TempNode->name);
	//searches for the parent name in a 2 depth tree, recursive if deeper than that.
	if(strcmp(TempNode->name,parent)==0){
		while(TempNode->children[i] != NULL){
			i++;
		}
			//printf("%d", i);
			TempNode->children[i] = NewTree;
			NewTree->parent = TempNode;
			printf("%s %s\n", NewTree->parent, TempNode->children[i]->name);
	}else{
		TREE* NewTemp;
		while(TempNode->children[i] != NULL){
			NewTemp = TempNode->children[i];
			//printf("%s ", NewTemp->name);	
			if(NewTree->parent == NULL){
				make_tree(parent, name, NewTemp, NewTree); //makes a tree of the roots children and tries again
			}
			i++;
		}
	}
	return;
}

//debug function
void CPrint(){
	CPointer = CHead;
	while(CPointer != NULL){
		printf("%s %s ",CPointer->name, CPointer->first);
		if(strcmp(CPointer->name,"ClosestCommonSupervisor")==0 || strcmp(CPointer->name,"IsSubordinate")==0 || strcmp(CPointer->name,"CompareRank")==0 ||strcmp(CPointer->name,"IsSupervisor")==0){
			//printf("entered");
			printf("%s ", CPointer->second);
		}
		printf("\n");
		CPointer = CPointer->next;
	}
	return;
}
