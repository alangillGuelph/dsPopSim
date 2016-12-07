#include "SWDSimulatorMulti.h"

/*

	Implementation of the SWDSimulatorMulti class and RunStruct struct methods.  Method declarations are 
	included in the SWDSimulatorMulti header file.  The overall class/struct descriptions are also included
	in the header file, right above the class/struct definition.

*/

// this method basically sets up and runs the thread wrapper for a cell
// each of the cells running concurrently is set up with one of these - the threadarg is a 
// reference to a RunStruct struct which contains all the arguments necessary to run the simulation
// the arguments are: numTimeSteps to run for, threadCells[i] the particular cell to simulate over, 
// 					  timeStep the timestep to start at, dt the integration step, ignoreFruit, ignoreDiapause submodel bools
// 					  startDay day to add the flies, temps the temperature array, hasNan bool signifying whether the cell has NaN temperatures 
// Note: if the temperatures are NaNs, the simulation is not run (since the results would be nonsensical, so there's 
//       no point wasting the computational time)
void* runThread(void *threadarg) {
	struct RunStruct *o;

    o = (struct RunStruct *) threadarg; // save the threadargs as a struct reference

	bool ignoreDiap = o->ignoreDiapause;
	int startDay = o->startDay;

	if (startDay >= 0)
		o->cell.setAddInitPop(true); // if valid startday, set variable to ensure that init pop isn't added before injection date, regardless of diapause

	int tempSize = o->temps.size(); // the number of temperature values

	for (double i = 0; round2Decimals(i) < o->numTimeSteps; i = i + o->dt) {

		if (o->hasNan || tempSize == 0) // don't simulate if there are NaNs
			break;

		double temp = o->temps[((int)i) % tempSize]; // constant temp!!!!! (for now)

		if ((int) o->timeStep == startDay && !o->injectFlies) {
			o->injectFlies = true;
			o->cell.readInitFlies(); // read in initial populations on the chosen date
		}

		o->cell.SWDCellSingle::stepForward(temp, o->ignoreFruit, ignoreDiap, o->dt, o->timeStep);
		o->timeStep += o->dt; // increase the current timestep accordingly
	}

	pthread_exit(NULL);
}

// method to do nothing when thread is executed (could be used for dud cells if necessary)
void* bogusRun(void* pls) {}

// constructor for the SWDSimulatorMulti
// takes in model parameters, number of rows/cols for the grid of cells, and the list of corresponding lat/lon pairs
SWDSimulatorMulti::SWDSimulatorMulti(std::string paramFile, int rows, int cols, std::vector<std::vector<double>> latitudes1) : numRows(rows), numCols(cols), latitudes(latitudes1) {
	dt = 0.05;
	numThreads = 2;
	timeStep = 0;
	
	Parameters newParams(paramFile);

	for (int r = 0; r < numThreads; r++){
		threadCells.push_back(SWDCellMulti(0, 0, newParams)); // default temps
	}
		

	resetTime();
}

// method to set the parameters of all cells to the default parameters
void SWDSimulatorMulti::setDefaultParams() {
	Parameters paramsNew; // default parameters
	
	for(int r = 0; r < numThreads; r++){
		threadCells[r].resetCellParams(paramsNew); // apply changes to each cell, reset parameters to those read in
	}
}

// method to run the simulation over all gridcells for the specified number of timesteps with the specified parameters
// fileNames : a 2D grid of filenames, each the name of the file to print per-day population values in for the corresponding grid cell, post sim
// summaryFiles : a 2D grid of filenames, each the name of the file to print sim summary values in for the corresponding grid cell, post sim
// tempsFiles : a 2D grid of filenames, each the name of the file to read temperature values in for the corresponding grid cell, for the sim
void SWDSimulatorMulti::run(int numTimeSteps, bool ignoreFruit, bool ignoreDiapause, int startDay, std::vector<std::vector<std::string>> fileNames, std::vector<std::vector<std::string>> summaryFiles, std::vector<std::vector<std::string>> tempsFiles) {
	maxCellPopulation = 0;

	int rc;

	int r = 0, c = 0;

	for (int k = 0; k < numRows*numCols; k += numThreads) { // go up by the number of threads available, since this is the number of cells run at a time
		
		// set up the list of threads and RunStructs (for the simulation parameters of each thread)
		pthread_t tSims[4];
		struct RunStruct rn[4];
   		
   		int cc = c;
   		int cr = r;
   		for (int i = 0; i < numThreads; i ++) { // for each thread
			
			// set up each thread that corresponds to a valid gridcell coordinate
			if (cr < numRows && cc < numCols) {
				threadCells[i].resetTime();
				std::vector<double> temps;
				bool hasNan = false;
				if (tempsFiles.size() > 0) { // read in temperature values for the cell, if there are any
					std::ifstream fileIn;
					fileIn.open(tempsFiles[cr][cc]);

					double v;
					while (fileIn >> v) {
						temps.push_back(v);
						if (v != v)
							hasNan = true;
					}

					fileIn.close();

				} else
					temps.push_back(15); // if no temps file, run at a constant 15 degrees (the sim can be run with no temp file if you really want)

				if (latitudes.size() > 0) {
					std::string param = "latitude";
					threadCells[i].setSingleParameter(param, latitudes[cr][cc]); // set latitude of the cell (needed for daylight hours calculations)
				}

				// create the list of thread running parameters
				rn[i] = RunStruct(numTimeSteps, threadCells[i], timeStep, dt, ignoreFruit, ignoreDiapause, startDay, temps, hasNan);
				rc = pthread_create(&tSims[i], NULL, runThread, (void *)&rn[i]); // run the thread (with the runThread method above)
			}
			// else {
			// 	rn[i] = RunStruct(0, threadCells[0], 0, 0, true);
			// 	rc = pthread_create(&tSims[i], NULL, bogusRun, (void *)&rn[i]);
			// }


			if (rc) {
		        std::cout << "Error:unable to create thread," << rc << std::endl;
		        exit(-1);
	    	}

	    	// update values (wrap around to col 0 if passed max column)
			cc ++;
			if (cc >= numCols) {
				cc = 0;
				cr ++;
			}
			
		}

		// join all sim threads to main thread, i.e. main thread doesn't continue until all sim threads are done
		cc = c; cr = r;
		for (int i = 0; i < numThreads; i ++) {
			
			if (cr < numRows && cc < numCols)	{
				pthread_join(tSims[i], NULL);
			}
			
			cc ++;
			if (cc >= numCols) {
				cc = 0;
				cr ++;
			}
		}

		// after all the threads have finished their simulations, the main thread regains control
		// and this section fo the code is reached
		for (int i = 0; i < numThreads; i ++) {
			
			// print info for the cells just simulated over
			if (fileNames[r][c].compare("") != 0 && summaryFiles[r][c].compare("") != 0) {
				if (r < numRows && c < numCols) {
					
					threadCells[i] = rn[i].cell;
					printCellInfo(i, fileNames[r][c], summaryFiles[r][c]);	
				}
			}
			if (r < numRows && c < numCols && threadCells[i].getMaxFemales() > maxCellPopulation) { // update the max population
				maxCellPopulation = threadCells[i].getMaxFemales();
				maxCellCoords[0] = r;
				maxCellCoords[1] = c;
			}

			c ++;
			if (c >= numCols) {
				c = 0;
				r ++;
			}
			if (r >= numRows) {
				break;
			}
		}
			

	}

	timeStep += numTimeSteps; // update the current timestep of the simulation over all cells

}

// run the simulation but with no specified temp files (this isn't really used)
void SWDSimulatorMulti::run(int numTimeSteps, bool ignoreFruit, bool ignoreDiapause, int startDay) {
	std::vector<std::vector<std::string>> tempNoLen;
	run(numTimeSteps, ignoreFruit, ignoreDiapause, startDay, tempNoLen, tempNoLen);
}

// method to print all info from a simulation
// this includes daily per-stage population values (printed to dataFile), and the summary
// of the simulation (overall total pop, day of max pop, etc) (printed to summaryFile)
void SWDSimulatorMulti::printCellInfo(int i, std::string dataFile, std::string summaryFile) {
	std::string names[] = {"eggs", "instar1", "instar2", "instar3", "pupae", "males", "females"};
		
	XYSeries toPrint[8]; // array of data series for all lifestages, and fruit quality, vs time
	
	SWDCellMulti& cell = threadCells[i];
	
	toPrint[0] = cell.getEggSeries();
	toPrint[1] = cell.getInst1Series();
	toPrint[2] = cell.getInst2Series();
	toPrint[3] = cell.getInst3Series();
	toPrint[4] = cell.getPupaeSeries();
	toPrint[5] = cell.getMalesSeries();
	toPrint[6] = cell.getFemalesSeries();
	toPrint[7] = cell.getFruitQualitySeries();
	
	std::ofstream fileOut;
	fileOut.open(dataFile);
	fileOut << "Time:" << "\t";
	
	// all the series have the same number of data points
	
	// print daily data
	for (int jk = 0; jk < 7; jk ++) { // print data labels
			fileOut << names[jk] << ":\t";
	}
	fileOut << "\n";
	for (int ik = 0; ik < toPrint[0].size(); ik +=20) { // += 20 so it prints every 20th datapoint (i.e. once per day)
		fileOut << toPrint[0][ik].getX() << "\t"; // print the timestep (same for all series)
		for (int jk = 0; jk < 7; jk ++) {
				std::stringstream sstm;
				sstm << toPrint[jk][ik].getY();
				fileOut << sstm.str() << "\t"; // print the corresponding value for the selected series
		}
		fileOut << "\n";
	}
	
	fileOut.close();
	
	fileOut.open(summaryFile);
	
	// print overall data
	fileOut << "\n\nTotal Cumulative Populations\n";
	std::stringstream sstm;
	sstm << "\n\t" << cell.getTotEggs() << "\t" << cell.getTotInst1() << "\t" << cell.getTotInst2() << 
					"\t" << cell.getTotInst3() << "\t" << cell.getTotPupae() << "\t" << cell.getTotMales() << 
					"\t" << cell.getTotFemales();;
				
	fileOut << sstm.str();
	fileOut << "\n\nPeak Populations\n";

	std::stringstream sstm1;
	sstm1 << "\n\t" << cell.getMaxEggs() << "\t" << cell.getMaxInst1() << "\t" << cell.getMaxInst2() << 
			"\t" << cell.getMaxInst3() << "\t" << cell.getMaxPupae() << "\t" << cell.getMaxMales() << 
			"\t" << cell.getMaxFemales();

	fileOut << sstm1.str();

	fileOut << "\n\nPeak Populations Day\n";

	std::stringstream sstm2;
	sstm2 << "\n\t" << cell.getDayMaxEggs() << "\t" << cell.getDayMaxInst1() << "\t" << cell.getDayMaxInst2() << 
			"\t" << cell.getDayMaxInst3() << "\t" << cell.getDayMaxPupae() << "\t" << cell.getDayMaxMales() << 
			"\t" << cell.getDayMaxFemales();
	
	fileOut << sstm2.str();

	fileOut.close();
	
}

// method to load simulation parameters from a specified file
// error message is returned if there is an error, otherwise if all is well "Success!" is returned
errormsg SWDSimulatorMulti::loadParams(std::string configFile) {
	Parameters params = threadCells[0].getParams(); // current parameters (recall: the same for all cells in the grid)
	errormsg status = params.setConfigParams(configFile); // read in from the file
	
	if (! (status.compare("Success!") == 0) ) // it there was an error, let the user know
		return status;

	for (int r = 0; r < numThreads; r++){
		threadCells[r].resetCellParams(params); // apply changes to each cell, reset parameters to those read in (not the fruit parameters)
	}	
	
	return status;
}

// method to reset the simulation to timestep 0
void SWDSimulatorMulti::resetTime() {
	maxCellPopulation = 0;
	maxCellCoords[0] = 0;
	maxCellCoords[1] = 0;
	timeStep = 0;
	for (int r = 0; r < numThreads; r++){
		threadCells[r].resetTime(); // reset time for the cell
	}
}
