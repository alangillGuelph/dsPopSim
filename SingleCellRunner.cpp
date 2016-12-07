#include "SWDSimulatorSingle.h"
#include "SWDSimulatorMulti.h"

/*

	This file is a runner for the singlecell simulator.
	It takes in command line arguments to run the simulator.
	Run the simulation as follows:

	First, compile the model.  Call it using:

	./singleSim useDefault paramFileName tempFileName ignoreFruit ignoreDiapause outputFileName addFliesDate startingFemPop

	useDefault = 
	paramFileName = the extended parameters list to run the simulation with.  Usually configparams.txt
	tempFileName = a file with 365 single line entries of temperatures
	ignoreFruit = [0|1] 0, use the fruit model, 1 ignore the fruit model
	ignoreDiapause = [0|1] 0, use the diapause model, 1 ignore the diapause model
	outputFileName = destination file for results
	addFliesDate = 0-364 for fly introduction date, -1 to use diapause termination date
	startingFemPop = number of initial fecund females (> 0)

	Note that: useDefault, ignoreFruit, ignoreDiapause are bools (0 = false)
	Note also that: addFliesDate as -1 specifies to add the flies on diapause cross date.

	If you have the incorrect number of arguments the code will exit with an error.
	
	Otherwise the output of the simulation will be printed to the output file specified.
	This output consists of: 
		-- daily population of every lifestage
		-- daily fruit quality
		-- total cumulative population for each stage
		-- peak population for each stage
		-- day of max fruit
		-- day of diapause crossed
		-- peak population date for each stage

*/

void printCellInfo( std::string outputFile, SWDCellSingle cell) {
	std::string names[] = {"eggs", "instar1", "instar2", "instar3", "pupae", "males", "females", "fruit"};
		
	XYSeries toPrint[8]; // array of data series for all lifestages, and fruit quality, vs time
	
	toPrint[0] = cell.getEggSeries();
	toPrint[1] = cell.getInst1Series();
	toPrint[2] = cell.getInst2Series();
	toPrint[3] = cell.getInst3Series();
	toPrint[4] = cell.getPupaeSeries();
	toPrint[5] = cell.getMalesSeries();
	toPrint[6] = cell.getFemalesSeries();
	toPrint[7] = cell.getFruitQualitySeries();
	
	std::ofstream fileOut;
	fileOut.open(outputFile);
	fileOut << "Time:" << "\t";
	
	// all the series have the same number of data points
	
	// print daily data
	for (int jk = 0; jk < 8; jk ++) { // print data labels
			fileOut << names[jk] << ":\t";
	}
	fileOut << "\n";
	for (int ik = 0; ik < toPrint[0].size(); ik +=20) { // += 20 so it prints every 20th datapoint (i.e. once per day)
	 	fileOut << toPrint[0][ik].getX() << "\t"; // print the timestep (same for all series)
	 	for (int jk = 0; jk < 8; jk ++) {
	 			fileOut << toPrint[jk][ik].getY() << "\t";
	
	 	}
	 	fileOut << "\n";
	}



	// total population (non-normalized), normalized pop (from the whole data set), diapause termination date, peak pop date, max fruit date


	// first, get diap term date
	double diapi = 0;
	for ( int i = 0; i < 365*20; i += 20) {
		if ( toPrint[6][i].getY() > 0) {
			diapi = i/20.0;
			break;
		}
	}


	// then, get fruit max date
	double diapf = 0;
	for ( int i = 0; i < 365*20; i += 20) {
		if ( toPrint[7][i].getY() >= 0.95 ) {
			diapf = i/20.0;
			break;
		}
	}

	double total =  cell.getTotEggs() + cell.getTotInst1() + cell.getTotInst2() + cell.getTotInst3() + cell.getTotPupae() + cell.getTotMales() + cell.getTotFemales();
	
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

	std::stringstream sstm3;
	sstm3 << "\n\nDiapause injection date: " << diapi << "\n\nFruit max date: " << diapf << "\n";
	fileOut << sstm3.str();

	fileOut.close();
	
}

// main method to run the code; this takes in the cmd line arguments
int main(int argc, char *argv[]) {

	if ( argc != 9)
	{
		printf( "Error, exiting now\nUsage: ./singleSim useDefault paramFileName tempFileName ignoreFruit ignoreDiapause outputFileName addFliesDate startingFemPop");
		printf( "\n\nNote that: useDefault, ignoreFruit, ignoreDiapause are bools (0 = false)\nNote also that: addFliesDate as -1 specifies to add the flies on diapause cross date.");
		exit( 0);
	}

	// read in all the arguments
	std::stringstream sstm;
	sstm << argv[1] << " " << argv[4] << " " << argv[ 5] << " " << " " << argv[ 7] << " " << argv[ 8]; // need all the numbers saved here

	int runTime = 360; // run simulation for a year
	double dt = 0.05;  // integration step

	std::string inputFileName = argv[ 2];
	std::string tempFileName = argv[ 3];
	std::string outputFileName = argv[ 6];

	int useDefault, ignoreFruit, ignoreDiapause, printArray, startDate, startFemPop;

	sstm >> useDefault;
	sstm >> ignoreFruit;
	sstm >> ignoreDiapause;
	sstm >> startDate;
	sstm >> startFemPop;

	if ( useDefault == 1)
	{
		inputFileName = ""; // set to null filename if not reading from a file
	}
                                                         
	// read temperatures in from file
	std::ifstream infile( tempFileName);
	std::vector< double> temperatures;

	double temp;
	while ( infile >> temp)
	{
		temperatures.push_back( temp);
	}

	infile.close();

	// set up and run the simulator
	SWDSimulatorSingle sim( dt, inputFileName);

	std::string param = "initial females1";
	sim.setSingleParameter( param, startFemPop);

	sim.run( temperatures, runTime, ignoreFruit, ignoreDiapause, startDate);

	printCellInfo( outputFileName, sim.getCell()); // print output to a file

	return 0;
}
