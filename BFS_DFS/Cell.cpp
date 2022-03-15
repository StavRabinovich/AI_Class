#include "Cell.h"



Cell::Cell()
{
}

Cell::Cell(int r, int c, Cell * p)
{
	row = r;
	col = c;
	parent = p;
}


Cell::~Cell()
{
}
