#include "SWDSimulatorSingle.h"
#include "SWDSimulatorMulti.h"

/*

	This file is a runner for the multicell simulator.
	Here, it's hardcoded to run over a 3x2 cell grid, for temperature files just to demonstrate
	how to set up the multicell simulator.
	The filenames for the temperature files and output files are hardocded for the directory
	structure of where I was running the simulations (on the leecs server), but could be changed
	if anyone wanted to reuse this file for their own grid file structure.



	Things you need to change if you want to get this to work with your own setup:
	-- change the dimensions of the grid (latitude/longitude)
	-- change the latitude you're starting with, and how it is incremented
	-- change the names of the file you're reading in from and printing to
	-- and, make sure it matches to your directory structure


	Note: if you have DATA/fileNamePrintTo.txt as your file to print to, the DATA directory
	      must already exist.  If the directory does not exist no output file will be created.
	      Make sure your file names match the existing directory structure.

*/

int main(int argc, char *argv[]) {

	int rows = 3; // latitude
	int cols = 2; // longitude

	int runTime = 365; // run simulation for a year
	// don't ignore either diapause or fruit submodel
	bool ignoreFruit = false;
	bool ignoreDiapause = false;
	int startDay = -1; // add on diap crossing threshold
	
	std::vector<std::vector<std::string>> fileNames;
	std::vector<std::vector<std::string>> summaryFiles;
	std::vector<std::vector<std::string>> tempsFiles;
	std::vector<std::vector<double>> latitudes;

	double lat = 24.5; // starting lat, gets updated each time
					   // you'd need to change this depending on which locations you're using
	
	for (int i = 0; i < rows; i ++) {

		fileNames.push_back(std::vector<std::string>());
		summaryFiles.push_back(std::vector<std::string>());
		tempsFiles.push_back(std::vector<std::string>());
		latitudes.push_back(std::vector<double>());
		// set up the filenames
		// this is hardcoded to the directory structure of where i was running the sims 
		// but this is easily changed to adapt for your own directory structure
		for (int j = 0; j < cols; j ++) {
			std::stringstream sstm1;
			sstm1 << "randomTemp_lat" << (i + 1) << "_lon" << (j + 1) << ".txt";

			fileNames[i].push_back("DATA/output_" + sstm1.str()); // daily output
			summaryFiles[i].push_back("DATA/summary_" + sstm1.str()); // summary output
			
			tempsFiles[i].push_back("TemperatureFiles/" + sstm1.str());

			latitudes[i].push_back(lat);

		}
		lat ++; // update lat for next row
	}
	
	SWDSimulatorMulti sim("", rows, cols, latitudes); // set up the simulator with the running parameters
	
	sim.run(runTime, ignoreFruit, ignoreDiapause, startDay, fileNames, summaryFiles, tempsFiles); // run the simulator (this also prints the output)
	
	return 0;
}
