/*
	CSC D84 - Unit 1 - Search

	This file contains stubs for implementing the different search
	algorithms covered in the course. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E., Jul. 15
	Updated: F.J.E., Jan. 18
*/

/**********************************************************************
% COMPLETE THIS TEXT BOX:
%
% 1) Student Name: Kyle Lewis
% 2) Student Name:		
%
% 1) Student number: 1006113215
% 2) Student number:
% 
% 1) UtorID Lewisky2
% 2) UtorID
% 
% We hereby certify that the work contained here is our own
%
% Kyle Lewis		             _____________________
% (sign with your name)            (sign with your name)
***********************************************************************/

#include "AI_search.h"
#include "stdbool.h"
#define INT_MAX 2147483647;

/**
 * defining nodes and queues for linked lists
*/
typedef struct Node{
	int value;
	int priority;
	Node* next;
}Node;

typedef struct Queue{
	Node* first;
	Node* last;
} Queue;
/**
 * defining basic functions for queue functionality
*/
Queue* createQueue(){
	Queue* newQueue = (Queue*)calloc(1, sizeof(Queue));
	if(newQueue == NULL){
		printf("Not enough memory for queue");
		return NULL;
	}
	newQueue->first = NULL;
	newQueue->last = NULL;

	return newQueue;
}

Node* createNode(int value){
	Node* newNode = (Node*)calloc(1, sizeof(Node));
	if(newNode == NULL){
		printf("not enoguh memory for new node");
		return NULL;
	}
	newNode->value = value;
	newNode->next = NULL;
	newNode->priority = 0;

	return newNode;
}

Node* createPriorityNode(int value, int priority){
	Node* newNode = createNode(value);
	newNode->priority = priority;
	// printf("newNode priority %d\n",newNode->priority);
	return newNode;
}

void enQueue(Queue* queue, int value){
	Node* newNode = createNode(value);
	if(queue->first == NULL){
		queue->first = newNode;
		queue->last = newNode;
		// printf("queued %d\n",value);
		return;
	}
	queue->last->next = newNode;
	queue->last = newNode;
	// printf("queued %d\n",value);
	return;
}

//enqueue into priority queue by sorting each time something is inserted into the queue
//slow but i didn't want to implement something like a min heap
void priorityEnQueue(Queue* queue, int value, int priority){
	Node* newNode = createPriorityNode(value, priority);
	if(queue->first == NULL){
		queue->first = newNode;
		queue->last = newNode;
		return;
	}
	//otherwise insert into linked list and sort it

	if(newNode->priority <= queue->first->priority){
		newNode->next = queue->first;
		queue->first = newNode;
		return;
	}

	Node* head = queue->first;
	//note prev should never be null since we always check to see if it should be inserted
	//at head first
	Node* prev = NULL;
	while(head != NULL){
		if(newNode->priority <= head->priority){
			prev->next = newNode;
			newNode->next = head;
			return;
		}
		prev = head;
		head = head->next;
	}
	//if iterated through entire list then it is the new last element
	prev->next = newNode;
	queue->last = newNode;
	return;
}

int deQueue(Queue* queue){
	if(queue->first == NULL){
		printf("Nothing in queue");
		return -1;
	}
	Node* first = queue->first;
	queue->first = queue->first->next;
	if(queue->first == NULL){
		queue->last = NULL;
	}
	int val = first->value;
	//printf("dequeing priority %d\n",first->priority);
	free(first);
	return val;
}

Node* priDeQueue(Queue* queue){
	if(queue->first == NULL){
		printf("Nothing in queue");
		return NULL;
	}
	Node* first = queue->first;
	queue->first = queue->first->next;
	if(queue->first == NULL){
		queue->last = NULL;
	}
	return first;
}

void freeQueue(Queue* queue){
	Node* first = queue->first;
	Node* temp = NULL;
	while(first != NULL){
		temp = first->next;
		free(first);
		first = temp;
	}
	queue->last = NULL;
	free(queue);
	return;
}

void print_path(int (*path)[2]){
	for(int i = 0; i < graph_size; i++){
		if(path[i][0] == -1 || path[i][1] == -1){
			printf("i:%d\n",i);
			break;
		}
		printf("[%d,%d]->",path[i][0],path[i][1]);
	}
	return;
}

//helper function to get grid position from cords
int get_grid_position(int coords[2]){
	// printf("converting grid [%d,%d] to %d\n",coords[0],coords[1],(coords[0] + (size_X * coords[1])));
	return (coords[0] + (size_X * coords[1]));
}

//Helper function used to determine if a pair of coordinates are in an array
int coordsInArray(int coords ,int (*array)[2],int size){
	//printf("cords in array\n");
	for(int i = 0; i < size; i++){
		if (coords == get_grid_position(*(array+i))){
	//		printf("cords found\n");
			return i;
		}
	}
	//printf("cords not found\n");
	return -1;
}

// Helper function used to do bfs on graph
void bfs(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2]){
	//start a queue
	//while queue is not empty and they are still cheeses
	//continue god's work

	int cheese_index = -1;
	int mouse_grid = get_grid_position(mouse_loc[0]);
	int predecessor[graph_size];

	int iteration = 0;
	
	//initalizing the predecessor arr
	for(int i = 0; i < graph_size; i++){
		predecessor[i] = -1;
	}

	Queue* queue = createQueue();
	//queuing the current mouse position
	enQueue(queue,mouse_grid); 

	while(queue != NULL && queue->first != NULL && cheese_index < 0){
		//dequeue position
		//printf("deQueue\n");
		int curr_pos = deQueue(queue);
		//printf("see if cheese\n");
		cheese_index = coordsInArray(curr_pos,cheese_loc,cheeses);
		//marking off order visited
		visit_order[curr_pos % size_X][(int) curr_pos / size_Y] = iteration;

		//check various grid locations
		//printf("cats\n");
		// printf("curr_pos: %d\n",curr_pos);
		// printf("testing %d\n", (int) gr[curr_pos][0] == 1);
		// printf("will collide with cat %d\n",(int) ! (coordsInArray(curr_pos-size_X,cat_loc,cats) < 0));
		// printf("out of range %d\n",(int)curr_pos - size_X >= 0);
		// printf("visited %d\n",predecessor[curr_pos-size_X] == -1);
		if(gr[curr_pos][0] == 1 && coordsInArray(curr_pos-size_X,cat_loc,cats) < 0 && curr_pos - size_X >= 0 && predecessor[curr_pos-size_X] == -1){
			//if one higher is possible, then add current_postition up on (minus y) to queue
			predecessor[curr_pos-size_X] = curr_pos;
			enQueue(queue,curr_pos - size_X);
			// printf("something got queued");
		}
		if(gr[curr_pos][1] == 1 && coordsInArray(curr_pos+1,cat_loc,cats) < 0 && curr_pos+1 < graph_size  && predecessor[curr_pos+1] == -1){
			predecessor[curr_pos+1] = curr_pos;
			enQueue(queue,curr_pos + 1);
			// printf("something got queued");
		}
		if(gr[curr_pos][2] == 1 && coordsInArray(curr_pos+size_X,cat_loc,cats) < 0 && curr_pos + size_X < graph_size && predecessor[curr_pos+size_X] == -1){
			predecessor[curr_pos+size_X] = curr_pos;
			enQueue(queue,curr_pos + size_X);
			// printf("something got queued");
		}
		if(gr[curr_pos][3] == 1 && coordsInArray(curr_pos-1,cat_loc,cats) < 0 && curr_pos -1 >= 0  && predecessor[curr_pos-1] == -1){
			predecessor[curr_pos-1] = curr_pos;
			enQueue(queue, curr_pos - 1);
			// printf("something got queued");
		}
		//printf("iterate\n");
		iteration++;
	}

	//if didn't find cheese index just return
	if(cheese_index == -1){
		return;
	}
	//otherwise go through predessecors to get path in reverse order then reverse it
	int curr_cords = get_grid_position(cheese_loc[cheese_index]);
	int path_index = 0;
	while(curr_cords != mouse_grid){
		//add the current cords to the path (by operating on its grid value)
		//then go to its predessecor
		path[path_index][0] = curr_cords % size_X;
		path[path_index][1] = (int) curr_cords / size_Y;
		// printf("[%d,%d]\n",curr_cords % size_X,(int) curr_cords / size_Y);
		// printf("predessecor %d\n",predecessor[curr_cords]);

		curr_cords = predecessor[curr_cords];
		path_index++;
	}
	path[path_index][0] = mouse_loc[0][0];
	path[path_index][1] = mouse_loc[0][1];
	path_index++;
	// printf("before reversing, %d\n", path_index);
	// print_path(path);

	//first path_index+1 elements should have the path in reverse order, so just reverse those elements
	//path_index = len(path)

	for(int i = 0; i < (path_index) / 2; i++){
		int temp[2] = {path[i][0], path[i][1]};
		path[i][0] = path[path_index-1-i][0];
		path[i][1] = path[path_index-1-i][1];
		path[path_index-1-i][0] = temp[0];
		path[path_index-1-i][1] = temp[1];
	}
	// printf("test\n");
	// printf("[%d,%d]",path[0][0],path[0][1]);
	// print_path(path);
	freeQueue(queue);
	return;
}

//helper function used to do dfs on a graph
bool dfs(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_grid, int visited[graph_size], int depth, int* iteration){
	

	//if location is out of bounds or lines up with a cat, return false
	if(mouse_grid < 0 || mouse_grid >= graph_size || coordsInArray(mouse_grid,cat_loc,cats) >= 0 || visited[mouse_grid] || *iteration >= graph_size){
		return false;
	}
	// printf("iteration %d\n",*iteration);
	
	//add node to path
	int mouse_x = (int) mouse_grid % size_X;
	int mouse_y = (int) mouse_grid / size_Y;
	path[depth][0] = mouse_x;
	path[depth][1] = mouse_y;
	visit_order[mouse_x][mouse_y] = *iteration;
	*iteration += 1;
	visited[mouse_grid] = true;

	//if current location is at cheese, then return True
	if(coordsInArray(mouse_grid,cheese_loc,cheeses) >= 0){
		printf("found cheese\n");
		return true;
	}

	//check all around it clockwise and call functions on those
	
	if(gr[mouse_grid][0] == 1 &&  dfs(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_grid-size_X,visited,depth+1,iteration)){
		return true;
	}
	if(gr[mouse_grid][1] == 1 && dfs(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_grid+1,visited,depth+1,iteration)){
		return true;
	}
	if(gr[mouse_grid][2] == 1 && dfs(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_grid+size_X,visited,depth+1,iteration)){
		return true;
	}
	if(gr[mouse_grid][3] == 1 && dfs(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_grid-1,visited,depth+1,iteration)){
		return true;
	}

	//if none of its functions lead to a cheese, return false because this node doesn't
	path[depth][0] = -1;
	path[depth][1] = -1;
	return false;
}

//want to do ucs (need priority queue)
void heuristic_search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])){

	//initalize priority queue
	//add starting node to priority queue
	printf("creating priority queue\n");
	Queue* priorityQueue = createQueue();
	int mouse_grid = get_grid_position(mouse_loc[0]);
	printf("inital %d\n",mouse_grid);
	priorityEnQueue(priorityQueue,mouse_grid,1);

	printf("testing queue value: %d, priority: %d, next is null %d\n",priorityQueue->first->value,priorityQueue->first->priority,priorityQueue->first->next == NULL);

	//while priority queue has an entry
	int cheese_index = -1;
	int iteration = 0;

	int predecessor[graph_size];

	//initalizing the predecessor arr
	for(int i = 0; i < graph_size; i++){
		predecessor[i] = -1;
	}

	while(priorityQueue->first != NULL){

		//pop off from priority queue
		Node* curr = priDeQueue(priorityQueue);
		int curr_pos = curr->value;
		int curr_pri = curr->priority;
		free(curr);
		// printf("dequeued %d\n",curr_pos);
		int curr_x = curr_pos % size_X;
		int curr_y = curr_pos / size_Y;
		visit_order[curr_x][curr_y] = iteration;
		//if its a cheese then you've found the cheese
		cheese_index = coordsInArray(curr_pos,cheese_loc,cheeses);
		if(cheese_index >= 0){
			printf("cheese has been found!\n");
			break;
		}

		
		//otherwise see if other nodes are valid and put them in the priority queue based off their heuristic

		if(gr[curr_pos][0] == 1 && coordsInArray(curr_pos-size_X,cat_loc,cats) < 0 && curr_pos - size_X >= 0 && predecessor[curr_pos-size_X] == -1){
			int new_pos = curr_pos-size_X;
			predecessor[new_pos] = curr_pos;
			priorityEnQueue(priorityQueue, new_pos, curr_pri + heuristic(new_pos % size_X,(int)new_pos / size_Y,cat_loc,cheese_loc,mouse_loc,cats,cheeses,gr));
		}
		if(gr[curr_pos][1] == 1 && coordsInArray(curr_pos+1,cat_loc,cats) < 0 && curr_pos+1 < graph_size  && predecessor[curr_pos+1] == -1){
			int new_pos = curr_pos+1;
			predecessor[new_pos] = curr_pos;
			priorityEnQueue(priorityQueue, new_pos, curr_pri + heuristic(new_pos % size_X,(int)new_pos / size_Y,cat_loc,cheese_loc,mouse_loc,cats,cheeses,gr));
		}
		if(gr[curr_pos][2] == 1 && coordsInArray(curr_pos+size_X,cat_loc,cats) < 0 && curr_pos + size_X < graph_size && predecessor[curr_pos+size_X] == -1){
			int new_pos = curr_pos+size_X;
			predecessor[new_pos] = curr_pos;
			priorityEnQueue(priorityQueue, new_pos, curr_pri + heuristic(new_pos % size_X,(int)new_pos / size_Y,cat_loc,cheese_loc,mouse_loc,cats,cheeses,gr));
		}
		if(gr[curr_pos][3] == 1 && coordsInArray(curr_pos-1,cat_loc,cats) < 0 && curr_pos -1 >= 0  && predecessor[curr_pos-1] == -1){
			int new_pos = curr_pos-1;
			predecessor[new_pos] = curr_pos;
			priorityEnQueue(priorityQueue, new_pos, curr_pri + heuristic(new_pos % size_X,(int)new_pos / size_Y,cat_loc,cheese_loc,mouse_loc,cats,cheeses,gr));
		}

		iteration++;
		
	}
	//if don't have a cheese index then path wasn't found
	if (cheese_index < 0){
		printf("no cheese found\n");
		return;
	}
	
	//otherwise need to backtrack the path

	int curr_cords = get_grid_position(cheese_loc[cheese_index]);
	int path_index = 0;
	while(curr_cords != mouse_grid){
		//add the current cords to the path (by operating on its grid value)
		//then go to its predessecor
		path[path_index][0] = curr_cords % size_X;
		path[path_index][1] = (int) curr_cords / size_Y;
		// printf("[%d,%d]\n",curr_cords % size_X,(int) curr_cords / size_Y);
		// printf("predessecor %d\n",predecessor[curr_cords]);

		curr_cords = predecessor[curr_cords];
		path_index++;
	}
	path[path_index][0] = mouse_loc[0][0];
	path[path_index][1] = mouse_loc[0][1];
	path_index++;
	// printf("before reversing, %d\n", path_index);
	// print_path(path);

	//first path_index+1 elements should have the path in reverse order, so just reverse those elements
	//path_index = len(path)

	for(int i = 0; i < (path_index) / 2; i++){
		int temp[2] = {path[i][0], path[i][1]};
		path[i][0] = path[path_index-1-i][0];
		path[i][1] = path[path_index-1-i][1];
		path[path_index-1-i][0] = temp[0];
		path[path_index-1-i][1] = temp[1];
	}
	//free the queue

	freeQueue(priorityQueue);
	return;

}

int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
 /*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. Which cheese is up to you.
	Whatever you code here, your heuristic must be admissible.

	Input arguments:

		x,y - Location for which this function will compute a heuristic search cost
		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		gr - The graph's adjacency list for the maze

		These arguments are as described in the search() function above
 */

	/**
	 * have each cost be manhattan distance to closest cheese (can figure out better algo later)
	*/
	// printf("heurisitc called\n");
	int min_distance = INT_MAX;
	for(int i = 0; i < cheeses; i++){
		int new_dist = (abs(x - cheese_loc[i][0]) + abs(y - cheese_loc[i][1]));
		if (new_dist < min_distance){
			min_distance = new_dist;
		}
	}
	return min_distance;
}

int predict_cat_cost(double gr[graph_size][4],int cat_loc[1][2], int mouse_loc[1][2]){
	//start a queue
	//while queue is not empty and they are still cheeses
	//continue god's work

	int mouse_index = -1;
	int cat_grid = get_grid_position(mouse_loc[0]);
	int mouse_grid = get_grid_position(mouse_loc[0]);
	int curr_pos = cat_grid;
	int predecessor[graph_size];

	int iteration = 0;
	
	//initalizing the predecessor arr
	for(int i = 0; i < graph_size; i++){
		predecessor[i] = -1;
	}

	Queue* queue = createQueue();
	//queuing the current mouse position
	enQueue(queue,cat_grid); 

	while(queue != NULL && queue->first != NULL && curr_pos != mouse_grid){
		//dequeue position
		//printf("deQueue\n");
		curr_pos = deQueue(queue);

		if(gr[curr_pos][0] == 1 && curr_pos - size_X >= 0 && predecessor[curr_pos-size_X] == -1){
			//if one higher is possible, then add current_postition up on (minus y) to queue
			predecessor[curr_pos-size_X] = curr_pos;
			enQueue(queue,curr_pos - size_X);
			// printf("something got queued");
		}
		if(gr[curr_pos][1] == 1 && curr_pos+1 < graph_size  && predecessor[curr_pos+1] == -1){
			predecessor[curr_pos+1] = curr_pos;
			enQueue(queue,curr_pos + 1);
			// printf("something got queued");
		}
		if(gr[curr_pos][2] == 1 && curr_pos + size_X < graph_size && predecessor[curr_pos+size_X] == -1){
			predecessor[curr_pos+size_X] = curr_pos;
			enQueue(queue,curr_pos + size_X);
			// printf("something got queued");
		}
		if(gr[curr_pos][3] == 1 && curr_pos -1 >= 0  && predecessor[curr_pos-1] == -1){
			predecessor[curr_pos-1] = curr_pos;
			enQueue(queue, curr_pos - 1);
			// printf("something got queued");
		}
		//printf("iterate\n");
		iteration++;
	}

	//if didn't find cheese index just return
	if(mouse_index == -1){
		return INT_MAX;
	}
	int curr_cords = predecessor[curr_pos];
	//otherwise go through predessecors to get path in reverse order then reverse it
	int path_length = 1;
	while(curr_cords != cat_grid){
		path_length ++;
	}
	freeQueue(queue);
	return path_length;
}

int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
 /*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. 

	However - this time you want your heuristic function to help the mouse avoid being eaten.
	Therefore: You have to somehow incorporate knowledge of the cats' locations into your
	heuristic cost estimate. How well you do this will determine how well your mouse behaves
	and how good it is at escaping kitties.

	This heuristic *does not have to* be admissible.

	Input arguments have the same meaning as in the H_cost() function above.
 */

	//want to maximize amounts of moves it takes cats to get to mouse
	//calculate sum of amount of moves it takes to get to mouse however, have them exponentially
	//more expensive as cats get closer to mouse
	//if going to collide with a cat, make it as expensive as possible

	//get length of each path from each cat to mouse
	//sum lengths of divide cheese cost by length of path

	//get heursitics for shortest path but add onto it the cost of the kitties;
	float sum = 0;
	int cheese_cost = H_cost(x,y,cat_loc,cheese_loc,mouse_loc,cats,cheeses,gr);
	for(int i = 0; i<cats; i++){
		//get manhattan distance
		int manhattan_dist = abs(x-cat_loc[i][0]) + abs(y-cat_loc[i][1]);
		//want it to be exponentially more expensive the closer the cat is and also want it
		//to outweight the fastest path
		//cost to cheese / manhattan distance to cat
		int cat_cost =  predict_cat_cost(gr,(cat_loc+i),mouse_loc);
		
		sum += cheese_cost + (cat_cost * cat_cost);

	}
	return (int) sum;
}

void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]))
{
 /*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in AI_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:
	
	The game takes place on a grid of size 32x32, the file board_layout.h specifies the size and defines two
	constants 'size_X' and 'size_Y' for the horizontal and vertical size of the board, respectively. For our
	purposes, the grid of locations is represented by a graph with one node per grid location, so given
	the 32x32 cells, the graph has 1024 nodes. 

	To create a maze, we connect cell locations in the grid in such a way that a) there is a path from any
	grid location to any other grid location (i.e. there are no disconnected subsets of nodes in the graph),
	and b) there are loops.

	Since each node represents a grid location, each node can be connected to up to 4 neighbours in the
	top, right, bottom, and left directions respectively:

			node at (i,j-1)
				^
				|
	(node at i-1, j) <- node at (i,j) -> node at (i+1, j)
				|
				v
			node at (i,j+1)

	The graph is theredore stored as an adjacency list with size 1024 x 4, with one row per node in the
	graph, and 4 columns corresponding to the weight of an edge linking the node with each of its 4
	possible neighbours in the order towp, right, bottom, left (clockwise from top).

	Since all we care is whether nodes are connected. Weights will be either 0 or 1, if the weight is
	1, then the neighbouring nodes are connected, if the weight is 0, they are not. For example, if

	graph[i][0] = 0 
	graph[i][1] = 1
	graph[i][2] = 0
	graph[i][3] = 1

	then node i is connected to the right and left neighbours, but not to top or bottom.

	The index in the graph for the node corresponding to grid location (x,y) is

	index = x + (y*size_X) 		or in this case		index = x + (y*32)

	Conversely, if you have the index and want to figure out the grid location,

	x = index % size_X		or in this case		x = index % 32
	y = index / size_Y		or in this case		y = index / 32

	(all of the above are *integer* operations!)

	A path is a sequence of (x,y) grid locations. We store it using an array of size	
	1024 x 2 (since there are 1024 locations, this is the maximum length of any
	path that visits locations only once).

	Agent locations are coordinate pairs (x,y)	

   Arguments:
		gr[graph_size][4]   - This is an adjacency list for the maze
		path[graph_size][2] - An initially empty path for your code to fill.
				      In this case, empty means all entries are initially -1
		visit_order[size_X][size_Y]	- An array in which your code will store the
						  *order* in which grid locations were 
						  visited during search. For example, while
						  doing BFS, the initial location is the
						  start location, it's visit order is 1.
						  Then the search would expand the immediate
						  neighbours of the initial node in some order,
						  these would get a visit order of 2, 3, 4, and
						  5 respectively, and so on.
					
						  This array will be used to display the search
						  pattern generated by each search method.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- BFS
					mode = 1	- DFS
					mode = 2	- A*

		(*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[10][2], int cats, int cheeses)
				- This is a pointer to one of the heuristic functions you will implement, either H_cost()
				  or H_cost_nokitty(). The driver in AI_search_core_GL will pass the appropriate pointer
				  depending on what search the user wants to run.
			
				  If the mode is 0 or 1, this pointer is NULL

				  * How to call the heuristic function from within this function : *
					- Like any other function:
						h = heuristic( x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses);

   Return values:
		Your search code will directly update data passed-in as arguments:

		- path[graph_size][2]	: Your search code will update this array to contain the path from
					  the mouse to one of the cheese chunks. The order matters, so
					  path[0][:] must be the mouse's current location, path[1][:]
					  is the next move for the mouse. Each successive row will contain
					  the next move toward the cheese, and the path ends at a location
					  whose coordinates correspond to one of the cheese chunks.
					  Any entries beyond that must remain set to -1
		- visit_order[size_X][size_Y] 	:  Your search code will update this array to contain the
						   order in which each location in the grid was expanded
						   during search. This means, when that particular location
						   was checked for being a goal, and if not a goal, had its
						   neighbours added as candidates for future expansion (in
						   whatever order is dictated by the search mode).

						   Note that since there are 1024 locations, the values in 
						   this array must always be in [0, 1023]. The driver code
						   will then display search order as a yellow-colored
						   brightness map where nodes expanded earlier will look
						   brighter.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

 /********************************************************************************************************
 * 
 * TO DO:	Implement code to carry out the different types of search depending on the
 *		mode.
 *
 *		You can do this by writing code within this single function (and being clever about it!)
 *		Or, you can use this function as a wrapper that calls a different search function
 *		 (BFS, DFS, A*) depending on the mode. Note that in the latter case, you will have
 *	    	  to inform your A* function somehow of what heuristic it's supposed to use.
 *
 *		Visiting Order: When adding the neighbours of a node to your list of candidates for
 *				expansion, do so in the order top, right, bottom, left.
 *
 *		NOTE: Your search functions should be smart enough to not choose a path that goes
 *		      through a cat! this is easily done without any heuristics.
 * 
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

 // Stub so that the code compiles/runs - The code below will be removed and replaced by your code!

	if(mode == 0){
		bfs(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_loc);
	}else if(mode == 1){
		int visited[graph_size] = {false};
		int iteration = 0;
		dfs(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,get_grid_position(mouse_loc[0]),visited,0,&iteration);
		print_path(path);
	}else if(mode == 2){
		heuristic_search(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_loc,&H_cost);
	}else if(mode == 3){
		heuristic_search(gr,path,visit_order,cat_loc,cats,cheese_loc,cheeses,mouse_loc,&H_cost_nokitty);
	}else{
		path[0][0]=mouse_loc[0][0];
		path[0][1]=mouse_loc[0][1];
		path[1][0]=mouse_loc[0][0];
		path[1][1]=mouse_loc[0][1];
	}
	//print_path(path);

 return;
}

