#ifndef SWD_SIMULATOR_MULTI_H
#define SWD_SIMULATOR_MULTI_H

#include "SWDCellMulti.h"
#include <pthread.h>

/*

	Header file for the SWDSimulatorMulti.cpp.  Contains the SWDSimulatorMulti class definition,
	described below.  
	Method definitions are included in the cpp file, along with descriptions of the code.

*/


/*
	This struct is basically a wrapper for a SWDCellMulti (i.e. one of the grid cells in the 2D grid
	of cells running in the MultiCell simulation).  This struct exists only to allow the multicell simulation
	to be threaded.  Since currently there is no intercell interference, any of the cells can be run in parallel.
	The threading code is included in the cpp file, along with explanations of how it works.
*/
struct RunStruct {

	double numTimeSteps; // timesteps to run the sim for
	SWDCellMulti cell;
	double timeStep, dt; // current timestep and integration step respectively
	bool ignoreFruit;
	bool ignoreDiapause;
	int startDay; // day to add the flies at
	std::vector<double> temps; // array of per-day temperature values

	bool hasNan; 
	bool injectFlies;

	RunStruct(double numTimeSteps1, SWDCellMulti cell1, double timeStep1, double dt1, bool ignoreFruit1, bool ignoreDiapause1, int startDay1, std::vector<double> newTemps, bool newHasNan) : numTimeSteps(numTimeSteps1), cell(cell1), timeStep(timeStep1), dt(dt1), ignoreFruit(ignoreFruit1), ignoreDiapause(ignoreDiapause1), startDay(startDay1), temps(newTemps), hasNan(newHasNan), injectFlies(false) {}
	RunStruct() : numTimeSteps(0), cell(SWDCellMulti()), timeStep(0), dt(0), ignoreFruit(true), ignoreDiapause(true), startDay(0), temps(std::vector<double>()), hasNan(false), injectFlies(false) {}

};


/*
	This class describes an SWDSimulatorMulti object, which is a simulator to run 
	its array of cells under various specified conditions.  
	There are accessors for all the cell's accessors (to access the cells' and their
	populations' parameters).  
	There are also methods to reset the parameters (i.e. the parameter map) either 
	with arguments or by reading in from a file).
	There are also mutators to run the cells for a specified time interval under 
	specified temperature conditions.
*/
class SWDSimulatorMulti {

	std::vector<SWDCellMulti> threadCells; // list of all the cells in the simulation (it is a 2D grid, but represented as a 1D array)
	int numRows, numCols; // total rows and cols in the grid of cells
	std::vector<std::vector<double>> latitudes; // 2d array of lat/lon pairs for the cells

	// pop and location of cell with max population
	double maxCellPopulation;
	int maxCellCoords[2];

	double timeStep, dt;

	int numThreads; // number of threads available to run concurrently (i.e. how many cells can run simultaneously)

public:

	// methods explained in the implementation file
	SWDSimulatorMulti(std::string paramFile, int rows, int cols, std::vector<std::vector<double>> latitudes1 = std::vector<std::vector<double>>());
	void setDefaultParams();
	void run(int numTimeSteps, bool ignoreFruit, bool ignoreDiapause, int startDay, std::vector<std::vector<std::string>> fileNames, std::vector<std::vector<std::string>> summaryFiles, std::vector<std::vector<std::string>> tempsFiles = std::vector<std::vector<std::string>>());
	void run(int numTimeSteps, bool ignoreFruit, bool ignoreDiapause, int startDay);

	void printCellInfo(int i, std::string dataFile, std::string summaryFile);

	errormsg loadParams(std::string configFile);

	// accessors 
	int getNumRows() const { return numRows; }
	int getNumCols() const { return numCols; }
	double getTimeStep() const { return timeStep; }

	void resetTime();
	
};

#endif