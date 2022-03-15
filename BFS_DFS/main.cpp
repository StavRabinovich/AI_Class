#include "glut.h"
#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

const int MSZ = 100; // maze size

const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int GRAY = 5;
const int PATH = 6;


int maze[MSZ][MSZ] = {0};
vector<Cell*> grays;
bool startBFS = false, startDFS = false;


void InitMaze();

void init()
{	//          Red Green Blue 
	glClearColor(0, 0, 0.4, 0);// color of window background
	
	// set the main axes
	glOrtho(0, MSZ, 0, MSZ ,- 1, 1); 

	srand(time(0));

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for(i=1;i<MSZ-1;i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) // mostly walls
			{
				if (rand() % 100 > 35)
					maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
			else // mostly spaces
			{
				if (rand() % 100 > 20)
					maze[i][j] = SPACE;
				else maze[i][j] = WALL;
			}
		}
	maze[MSZ / 2][MSZ / 2] = START;
	Cell* ps = new Cell(MSZ / 2, MSZ / 2, nullptr);
	grays.push_back(ps);

	maze[rand() % MSZ][rand() % MSZ] = TARGET;
}


void ShowMaze() 
{
	int i, j;

	for(i=0;i<MSZ;i++)
		for (j = 0; j < MSZ; j++)
		{
			// set color for cell (i,j)
			switch (maze[i][j]) 
			{
			case WALL:
				glColor3d(0.5, 0, 0); // dark red
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case START:
				glColor3d(0.5, 0.5, 1); // light blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case BLACK:
				glColor3d(1, 1, 0); // yellow
				break;
			case GRAY:
				glColor3d(1, 0, 1); // magenta
				break;
			case PATH:
				glColor3d(0, 0.7, 0); // green
				break;
			}// switch
			// now show the cell as plygon (square)
			glBegin(GL_POLYGON);
			glVertex2d(j, i); // left-bottom corner
			glVertex2d(j, i+1); // left-top corner
			glVertex2d(j+1, i+1); // right-top corner
			glVertex2d(j+1, i); // right-bottom corner
			glEnd();
		}// for
}

void RestorePath(Cell* ps)
{
	while (ps != nullptr)
	{
		maze[ps->getRow()][ps->getCol()] = PATH;
		ps = ps->getParent();
	}
}

// gets the pointer to the current Cell and the coordinates of its neighbor
void CheckNeighbor(Cell* pCurrent, int row, int col)
{
	// check if the niegbor is not a TARGET
	if(maze[row][col]==TARGET)
	{
		startBFS = false;
		startDFS = false;
		cout << "The solution has been found\n";
		RestorePath(pCurrent);
	}
	else // paint this neighbor GRAY
	{
		Cell* ps = new Cell(row, col, pCurrent);
		grays.push_back(ps);   // ADD neighbor to grays QUEUE or STACK
		maze[row][col] = GRAY;
	}
}

void BFSIteration() 
{
	Cell* pCurrent;
	int row, col; // current row and col
	// check the grays queue
	if (grays.empty()) // nothing to do
	{
		startBFS = false;
		cout << "There is no solution\n";
	}
	else
	{
		// pick and remove from grays QUEUE the first Cell and check its neighbors
		pCurrent = *(grays.begin());
		grays.erase(grays.begin());
		// paint it black
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		maze[row][col] = BLACK;
		// now scan all the white [or target] neighbors and add them (if it's not a target) to Grays
		// check UP
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
			CheckNeighbor(pCurrent, row + 1, col);
		// check DOWN
		if (startBFS && (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET))
			CheckNeighbor(pCurrent, row - 1, col);
		// check LEFT
		if (startBFS && (maze[row][col-1] == SPACE || maze[row ][col-1] == TARGET))
			CheckNeighbor(pCurrent, row , col-1);
		// check RIGHT
		if (startBFS && (maze[row ][col+1] == SPACE || maze[row ][col+ 1] == TARGET))
			CheckNeighbor(pCurrent, row , col+ 1);
	}
}


void DFSIteration()
{
	Cell* pCurrent;
	int row, col; // current row and col

	if(grays.empty()) // no solution
	{
		startDFS = false;
		cout << "There is no solution\n";
	}
	else
	{
		// pick and remove from grays STACK the first Cell and check its neighbors
		// actually it is the last element in grays (because we add a new element to grays by push_back)
		pCurrent = grays.back();
		grays.pop_back();
		// paint it black
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		maze[row][col] = BLACK;

		// check random direction
		int direction[4] = { -1,-1,-1,-1 };
		int index;
		// fill direction with 0,1,2,3 on random locations
		for (int i = 0; i < 4; i++) 
		{
			do {
				index = rand() % 4;
			} while (direction[index] != -1);
			direction[index] = i;
		}

		for (int i = 0; i < 4 && startDFS; i++)
		{
			switch (direction[i]) 
			{
			case 0:  // up
				if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
					CheckNeighbor(pCurrent, row + 1, col);
				break;
			case 1:  // down
				if ((maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET))
					CheckNeighbor(pCurrent, row - 1, col);
				break;
			case 2:   // left
				if ((maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET))
					CheckNeighbor(pCurrent, row, col - 1);
				break;
			case 3:   // right
				if ((maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET))
					CheckNeighbor(pCurrent, row, col + 1);
				break;

			} // switch
		} // for

	} // else

}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	
	ShowMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
	if (startBFS)
		BFSIteration();
	if (startDFS)
		DFSIteration();


	glutPostRedisplay(); // indirect call to refresh function (display)
}

void menu(int choice)
{
	if (choice == 1) // BFS
		startBFS = true;
	else if (choice == 2) // DFS
		startDFS = true;
}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // double buffering for animation
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Uninformed Search Example");

	glutDisplayFunc(display); // sets display function as window refresh function
	glutIdleFunc(idle); // runs all the time when nothing happens

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Run BFS", 1);
	glutAddMenuEntry("Run DFS", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();

	glutMainLoop(); // starts window queue of events
}