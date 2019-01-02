*
  Author: Michael Mascari
  Email: mmascari2017@my.fit.edu
  Course: CSE2010
  Section: 2
  Description: hw6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NODE{
	char value; //what can move from cell to cell in the game
	char meta_value; //if a monster lands on the end point, meta_value does not change
}NODE;

typedef struct ARENA{
	int height;
	int length;
	int player_pos;
	NODE* playing_field; //contains the NODES as an array once it is known how many
}ARENA;

typedef struct ZOMBIE{ //alphabetical list of the zombies
	char name;
	int place;
	struct ZOMBIE* next;
}ZOMBIE;

typedef struct QUEUE{ //queue containing unvisited positions
	int position;
	struct QUEUE* next;
}QUEUE;

typedef struct FOUND{ //list containing viited positions
	int position;
	struct FOUND* next;
}FOUND;

FOUND* Fhead = NULL;

QUEUE* Qhead = NULL;

ZOMBIE* head = NULL;
ZOMBIE* ZPointer = NULL;

void read_file(int argc, char* argv[], ARENA* game); //reads in the input
void APrint(ARENA* game); //prints out a map of the game
void make_zombie(int i, char name); //places a zombie in the linked list
void move_player(ARENA* game); //moves the player first
void move_zombies(ARENA* game); //moves the zombies in alphabetical order;
void enqueue(int position); //enqueues a position on the field
int dequeue(); //removes  a position from the queue
void found(int position); //every position removed from queue is "found" so we don't reemter that space
void bredth_first_search(int position, ARENA* game); //searches from the zombie to the player
int check_found(int position); //checks a position against the FOUND list
void print_path(ARENA* game, int Zposition, int distance); //prints the path the zombie wants to take to get to the player
void clean_up(ARENA* game); //removes all the parent and visited values;
int defound(); //assists clean_up(); works like dequeue on the FOUND list


int main(int argc, char* argv[]){
	ARENA game; //makes the game world
	read_file(argc, argv, &game); //reads from the input file
	APrint(&game); //does the first print of the game world
	move_player(&game); //asks the player what they want to do, then moves the player
	APrint(&game); //prints the game world for the second time
	move_zombies(&game); //prints out each zombies path they wish to take
	return 0;
}

//moves each zombie in alphabetical order
void move_zombies(ARENA* game){
	ZPointer = head;
	while(ZPointer != NULL){ //loops through the Zombie list
		enqueue(ZPointer->place); ///queues where the zombie is standing
		bredth_first_search(ZPointer->place, game);	//BFS 	
		ZPointer = ZPointer->next;
	}
	return;
}

//searches from the zombie to the player
void bredth_first_search(int position, ARENA* game){
	printf("Creature %c:", ZPointer->name);
	position = dequeue();
	int queuer;
	int distance = 1; //starts at 1 because it increments the farther out it stretches and if it doesnt have to stretch, the length would be 1
	while(1){
		//4 blocks of code because slight differences between directions. I will explain the top one.
		if(game->playing_field[position+game->length].value != '#'){ //makes sure it is not a wall
			if(game->playing_field[position+game->length].value < 'A' || game->playing_field[position+game->length].value > 'Z'){ //makes sure it is not another zombie
				if(game->playing_field[position+game->length].value == '0'){ //if it is the player, BFS is finished, so it will do it's finishing lines
					game->playing_field[position+game->length].meta_value = position; //place this position as the parent node of player_pos
					while(position != ZPointer->place){ //work backward to zombie position, to find the distance
						distance++;
						position = game->playing_field[position].meta_value;
					}
					
					print_path(game, ZPointer->place, distance);
					return;
				}else{ //if not the player, it is another empty zone
					if(game->playing_field[position+game->length].meta_value == ' '){ //makes sure it has not been visited yet
						game->playing_field[position+game->length].meta_value = position; 
						enqueue(position+game->length); //queues it
					}
				}
			}   
		}
		//second direction
		if(game->playing_field[position-game->length].value != '#'){
			if(game->playing_field[position-game->length].value < 'A' || game->playing_field[position-game->length].value > 'Z'){
				if(game->playing_field[position-game->length].value == '0'){
					game->playing_field[position-game->length].meta_value = position; 
					while(position != ZPointer->place){
						distance++;
						position = game->playing_field[position].meta_value;
					}
					
					print_path(game, ZPointer->place, distance);
					return;
				}else{
					if(game->playing_field[position-game->length].meta_value == ' '){
						game->playing_field[position-game->length].meta_value = position; 
						enqueue(position-game->length);
					}
				}
			}   
		}
		//third direction
		if(game->playing_field[position+1].value != '#'){
			if(game->playing_field[position+1].value < 'A' || game->playing_field[position+1].value > 'Z'){
				if(game->playing_field[position+1].value == '0'){
					game->playing_field[position+1].meta_value = position; 
					while(position != ZPointer->place){
						distance++;
						position = game->playing_field[position].meta_value;
					}
					
					print_path(game, ZPointer->place, distance);
					return;
				}else{
					if(game->playing_field[position+1].meta_value == ' '){
						game->playing_field[position+1].meta_value = position; 
						enqueue(position+1);
					}
				}
			}   
		}
		//fourth direction
		if(game->playing_field[position-1].value != '#'){
			if(game->playing_field[position-1].value < 'A' || game->playing_field[position-1].value > 'Z'){
				if(game->playing_field[position-1].value == '0'){
					game->playing_field[position-1].meta_value = position; 
					while(position != ZPointer->place){
						distance++;
						position = game->playing_field[position].meta_value;
					}
					
					print_path(game, ZPointer->place, distance);
					return;
				}else{
					if(game->playing_field[position-1].meta_value == ' '){
						game->playing_field[position-1].meta_value = position; 
						enqueue(position-1);
					}
				}
			}   
		}	
		found(position); //puts the used position in the FOUND list
		queuer = 0;
		while(queuer == 0){ //queuer is 0 until position is not something from the FOUND list
			position = dequeue();
			queuer = check_found(position);
		}
		
	}
	return;
}

//prints the path the zombie wants to take to get to the player
void print_path(ARENA* game, int Zposition, int distance){
	int coords[2*(distance+1)];
	int position = game->player_pos; //starts from the players position and wrks backwards beacuse parent values are stored in meta_value
	for(int i = distance+1; i > 0; i--){
		coords[(2*i) -1] = position % game->length; //returns x coord
		coords[(2*i) - 2] = position/game->length; //returns y coord
		position = game->playing_field[position].meta_value; //enters the parent node 		
	}
	//determines direction based on how the coords interact with each other
	if(coords[2] > coords[0]){
		printf(" d %d ", distance);
		for(int i = 0; i < 2*(distance+1); i = i+2){
			printf("(%d,%d) ",coords[i], coords[i+1]);
		}
	}else if(coords[2] < coords[0]){
		printf(" u %d ", distance);
		for(int i = 0; i < 2*(distance+1); i = i+2){
			printf("(%d,%d) ",coords[i], coords[i+1]);
		}
	}else if(coords[3] < coords[1]){
		printf(" l %d ", distance);
		for(int i = 0; i < 2*(distance+1); i = i+2){
			printf("(%d,%d) ",coords[i], coords[i+1]);
		}
	}else if(coords[3] > coords[1]){
		printf(" r %d ", distance);
		for(int i = 0; i < 2*(distance+1); i = i+2){
			printf("(%d,%d) ",coords[i], coords[i+1]);
		}
	}
	printf("\n");
	clean_up(game); //resets the game world and lists
	return;
}

//checks a positio against the FOUND lsit
int check_found(int position){
	FOUND* temp = Fhead;
	while(temp!= NULL){
		if(position == temp->position){ //if the position was already found, try the next thing in the queue
			return 0;
		}
		temp = temp->next;
	}
	return 1;
}

//enqueues a position from the field
void enqueue(int position){
	//simple queue function
	QUEUE* new = (QUEUE*) malloc(sizeof(QUEUE));
	new->position = position;
	new->next = NULL;
	if(Qhead == NULL){ //if queue is empty
		Qhead = new;
	}else{ //if queue is not empty
		QUEUE* temp = Qhead;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = new;
	}
	return;
}

//removes a position from the queue
int dequeue(){
	int answer = -1; //to return the node and place in FOUND list, -1 if queue is empty
	QUEUE* temp = Qhead;
	if(temp != NULL){
		if(temp->next!= NULL){
			Qhead = Qhead->next;
		}else{
			Qhead = NULL;
		}
		answer = temp->position; //sets the variable to be returned
		free(temp); //frees the dequed node
		
	}
	return answer; //returns answer 
}

//if the position is removed from queue, it is "found" so it isn't reentered
void found(int position){
	FOUND* new = (FOUND*) malloc(sizeof(FOUND));
	new->position = position;
	new->next = NULL;
	if(Fhead == NULL){
		Fhead = new;
	}else{
		FOUND* temp = Fhead;
		while(temp->position != position && temp->next != NULL){
			temp = temp->next;
		}
		if(temp->position == position){
			return;
		}else{
			temp->next = new;
		}
	}
	return;
}

//places a zombie in the ZOMBIE linked list
void make_zombie(int i, char name){
	//simple linked list function
	ZOMBIE* new = (ZOMBIE*) malloc(sizeof(ZOMBIE));
	new->name = name;
	new->place = i;
	if(head == NULL){ //if head is NULL
		head = new;
		new->next = NULL;
	}else{
		ZOMBIE* temp = head; 
		if(new->name < temp->name){ //if placing at front of list
			new->next = head;
			head = new;
		}else{			
			while(temp->next != NULL){
				if(temp->next->name < new->name){
					temp = temp->next;
				}else{
					new->next = temp->next; //placing at middle of list
					temp->next = new;
					return;
				}
			}
			temp->next = new; //placing at end of list
			new->next = NULL;
		}
	}
	return;
}

//moves the player through a scanf input
void move_player(ARENA* game){
	char move;
	printf("Player 0, please enter your move [u(p), d(own), l(eft), or r(ight)]:");	
	scanf(" %c", &move);
	while(move!= 'u' && move != 'd' && move!= 'l' && move!= 'r'){ //input checking
		printf("Player 0, please enter your move [u(p), d(own), l(eft), or r(ight)]:");	
		scanf(" %c", &move);
	}
	//repeated for all four directions so I will only say for top one, only thing that changes is which part of the game world is affected
	if(move == 'r'){ //players input is checked
		//moves the players character if possible
		if(game->playing_field[game->player_pos+1].value == ' '){
			game->playing_field[game->player_pos].value = ' ';
			game->playing_field[game->player_pos].meta_value = ' ';
			game->playing_field[game->player_pos+1].value = '0';
			game->playing_field[game->player_pos+1].meta_value = '0';
			game->player_pos = game->player_pos+1;
		}else{
			//otherwise repeat the function because it's a wall and we want user input again
			if(game->playing_field[game->player_pos+1].value == '#'){
				move_player(game);
				return;
			}else if(game->playing_field[game->player_pos+1].meta_value == '*'){//or the player won or walked into a zombie
				printf("Player 0 beats the hungry creatures!\n");
			}else{
				printf("One creature is not hungry any more!\n");
			}
		}
	}else if(move == 'u'){
		if(game->playing_field[game->player_pos-(game->length)].value == ' '){
			game->playing_field[game->player_pos].value = ' ';
			game->playing_field[game->player_pos].meta_value = ' ';
			game->playing_field[game->player_pos-(game->length)].value = '0';
			game->playing_field[game->player_pos-(game->length)].meta_value = '0';
			game->player_pos = game->player_pos-(game->length);
		}else{
			if(game->playing_field[game->player_pos-(game->length)].value == '#'){
				move_player(game);
				return;
			}else if(game->playing_field[game->player_pos-(game->length)].meta_value == '*'){
				printf("Player 0 beats the hungry creatures!\n");
			}else{
				printf("One creature is not hungry any more!\n");
			}
		}
	}else if(move == 'd'){
		if(game->playing_field[game->player_pos+(game->length)].value == ' '){
			game->playing_field[game->player_pos].value = ' ';
			game->playing_field[game->player_pos].meta_value = ' ';
			game->playing_field[game->player_pos+(game->length)].value = '0';
			game->playing_field[game->player_pos+(game->length)].meta_value = '0';
			game->player_pos = game->player_pos+(game->length);
		}else{
			if(game->playing_field[game->player_pos+(game->length)].value == '#'){
				move_player(game);
				return;
			}else if(game->playing_field[game->player_pos+(game->length)].meta_value == '*'){
				printf("Player 0 beats the hungry creatures!\n");
			}else{
				printf("One creature is not hungry any more!\n");
			}
		}
	}else if(move == 'l'){
		if(game->playing_field[game->player_pos-1].value == ' '){
			game->playing_field[game->player_pos].value = ' ';
			game->playing_field[game->player_pos].meta_value = ' ';
			game->playing_field[game->player_pos-1].value = '0';
			game->playing_field[game->player_pos-1].meta_value = '0';
			game->player_pos = game->player_pos-1;
		}else{
			if(game->playing_field[game->player_pos-1].value == '#'){
				move_player(game);
				return;
			}else if(game->playing_field[game->player_pos-1].meta_value == '*'){
				printf("Player 0 beats the hungry creatures!\n");
			}else{
				printf("One creature is not hungry any more!\n");
			}
		}
	}
	return;
}

//cleans up the arena for the next zombei to do it's search
void clean_up(ARENA* game){
	for(int i = 0; i < game->length * game->height; i++){ //reassigns an empty meta value for everything that should have one. 
		//some spaces had their meta value used as a parent value to find the path between nodes 
		if(game->playing_field[i].meta_value != '*' && game->playing_field[i].meta_value != '#'){
			game->playing_field[i].meta_value = ' ';
		}
	}
	//both functions return -1 when their lists are emptied;
	int flag = dequeue();
	while(flag != -1){
		flag = dequeue();
		
	}
	flag = defound();
	while(flag != -1){
		flag = defound();
	}
	//makes sure the lists are not pointing at something in oblivion. since I check against NULL, I assign NULL.
	Qhead = NULL;
	Fhead = NULL;
	return;
}

//removes from the FOUND list to reset it
int defound(){
	int answer = -1; //to return the node, -1 if FOUND is empty
	FOUND* temp = Fhead;
	if(temp != NULL){
		if(temp->next!= NULL){
			Fhead = Fhead->next;
		}else{
			Fhead = NULL;
		}
		answer = temp->position;
		free(temp);
		
	}
	if(temp == NULL){
		return -1;
	}
	return answer;
}

//reads in the input
void read_file(int argc, char* argv[], ARENA* game){
	int height, length;
	FILE * data_input = fopen(argv[1], "r"); //opens the input file
	fscanf(data_input, "%d %d ", &height, &length);
	NODE* field = (NODE*) malloc(height*length*sizeof(NODE)); //because the dimensions of the game world are now known, stored in a one-d array
	game->playing_field = field;
	game->height = height;
	game->length = length;
	char NODE_value[length];
	int i = 0;
	int j;
	while(fgets(NODE_value, length+1, data_input)!=NULL){ //scans in each line, one at a time
		j = 0;
		while(j < length && NODE_value[j] != '\0' && NODE_value[j] != '\r' && NODE_value[j] != '\n'){ //removes gross hidden characters		
			field[i].value = NODE_value[j];
			if(NODE_value[j] == '*' || NODE_value[j] == '#'){ //meta_value is for the tile, not what is on it. unmoving things get a meta value
				field[i].meta_value = NODE_value[j];
			}else{
				field[i].meta_value = ' '; //means that the tile can be walked on
			}
			if(field[i].value == '0'){
				game->player_pos = i; //makes the palyers position more easily reachable later
			}
			if(field[i].value >= 'A' && field[i].value <= 'Z'){
				make_zombie(i, field[i].value); //places a new zombie in the ZOMBIE list
			}
			i++;
			j++;
		}
	}
	return;
}

//prints out the game world
void APrint(ARENA* game){
	printf(" "); //beacuse the proper formatting requires a space in the top left corner
	for(int i = 0; i < game->length; i++){
		printf("%d", i); //to print the top coordinates
	}
	for(int i = 0; i < game->length * game->height; i++){
		if(i % game->length == 0){ 
			printf("\n%d", i/game->length); //to rpint out the side coordinates
		}
		printf("%c", game->playing_field[i].value); //prints the value in the correct spot

	}
	printf("\n");	
	return;
}
