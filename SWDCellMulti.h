#ifndef SWD_CELL_MULTI_H
#define SWD_CELL_MULTI_H

#include "SWDCellSingle.h"

/*

	SWDCellMulti is an extension of the SWDCellSingle class that modifies it for use with the multi-cell simulator.
	In addition to the methods applicable to the SWDCellSingle super-class, the SWDCellMulti also has datafields 
	for coordinates in the grid (i.e. its place in the 2D grid of cells in its multicell simulator).
	Also, the general resetCellParams does not reset the fruit parameters (since fruit parameters are specific to 
	the individual cell in the grid, while other parameters are constant across the grid).  There is a separate 
	method to only reset the fruit parameters (this is in the SWDCellSingle superclass).

	Methods described in-code, in the implementation file SWDCellMulti.cpp.

*/

class SWDCellMulti : public SWDCellSingle {
	int row, col;

public: 

	SWDCellMulti(int r, int c, Parameters paramsNew);
	SWDCellMulti();
	
	void resetCellParams(Parameters paramsNew);

	int getRow() const { return row; }
	int getCol() const { return col; }

};

#endif