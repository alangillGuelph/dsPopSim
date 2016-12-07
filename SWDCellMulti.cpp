#include "SWDCellMulti.h"

/*

	Implementation of the SWDCellMulti class methods.  Class definition and accessors are 
	included in the SWDCellMulti header file.  The overall class description is also included
	in the header file, right above the class definition.

*/

// constructor which takes in the row and column values in the 2D grid of cells in the simulator
// and the parameters object specifying the cell parameters for the simulation
SWDCellMulti::SWDCellMulti(int r, int c, Parameters paramsNew) : SWDCellSingle(paramsNew), row(r), col(c) { }

// constructor taking no args - in this case, row and col are both set to 0 and 
// the default params are used
// this is not really used
SWDCellMulti::SWDCellMulti() : SWDCellSingle(Parameters()), row(0), col(0) { }
	
// method to reset the parameters of the cell to the params specified
// this doesn't reset the fruit params
// to reset the fruit params, the specific method for resetting fruit params (which 
// is in the SWDCellSingle file) is used
// this is b/c this method is called on all the cells in the grid when resetting the global
// params, since most are constant for the model i.e. across all the cells
// but the fruit params are cell specific (esp b/c of latitude), so it would not make 
// sense to reset them all. if being reset, these would be set per cell, using the 
// specific method provided
void SWDCellMulti::resetCellParams(Parameters paramsNew) {
	SWDCellSingle::resetCellParams(paramsNew, false); // don't reset fruit params!
}