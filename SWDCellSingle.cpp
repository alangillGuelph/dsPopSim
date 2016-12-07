#include "SWDCellSingle.h"

/*

	Implementation of the SWDSingleCell class methods.  Class definition and accessors are 
	included in the SWDCellSingle header file.  The overall class description is also included
	in the header file, right above the class definition.

*/

// Constructor taking a parameters object specifying the model parameters, and the
// initial temperature for the simulation
SWDCellSingle::SWDCellSingle(Parameters paramsNew, double newTemp) {
	dayCrossedMaxFruit = -1;
	ignoreFruit = true;
	ignoreDiapause = true;

	fruitQualities[0] = 0.05; // fruit quality starts at 0.05, at the beginning of the year
	currentFruitQ = 0.05; // default starting fruit quality of 0.05
	
	if (&paramsNew != NULL) {
		params = Parameters(paramsNew); // set Parameters datafield to a copy of the Parameters passed in
		temp = paramsNew.getParameter("constant temp"); // initial temperature of the cell
		population = SWDPopulation(params); // initialize population with parameters specified in the Parameters 
		resetCellParams(params, true); // set cell datafields to the values specified in the Parameters -> reset the fruit params
	}
	else {
		params = Parameters();
		temp = 15;
		population = SWDPopulation();
	}
	
	resetTime();

	killAllFruit = false; // fruit quality has not passed the cutoff yet
	
	for (int i = 0; i < 10; i ++) 
		thresholdPopDay[i] = -1;

	if (newTemp != 888) // 888 is the error state temperature
		temp = newTemp;
}

// method to change the value of a single specified parameter (specified by name) 
// a message is returned indicating success; or specifying the error if one occured
errormsg SWDCellSingle::setSingleParameter(std::string &param, double newVal) {
	return params.setParameter(param, newVal);
}

// method to get the value of a single specified parameter
double SWDCellSingle::getSpecificParameter(std::string &param) const {
	return params.getParameter(param);
}

// method to move the cell forward one timestep
// the temperature of the timestep is specified, and whether or not to ignore diapause and fruit submodels,
// and the integration step (dt -- the length of one timestep), and the current timestep 
void SWDCellSingle::stepForward(double temperature, bool ignoreFruitNew, bool ignoreDiapauseNew, double dt, double timeStep) {	
	// reset datafields to those passed in
	ignoreFruit = ignoreFruitNew;
	ignoreDiapause = ignoreDiapauseNew;
	temp = temperature;
	
	// get fruit parameters for the calculations
	double fruitBaseTemp = params.getParameter("fruit base temp");
	double fruitTimeLag = params.getParameter("fruit time lag");
	double fruitHarvestCutoff = params.getParameter("fruit harvest cutoff");
	
	double gt = getGT(fruitBaseTemp, temperature);
	
	int index = ((int) timeStep) % 365;
	double fruitQLag = 0.05;
	if (index - fruitTimeLag > 0) { // if timelag timesteps have passed in the year
		fruitQLag = fruitQualities[(int) (index - fruitTimeLag)]; // get the fruit quality timelag timesteps ago
		if (fruitQLag > fruitHarvestCutoff) // if it was greater than the cutoff, fruit quality begins to decrease
			killAllFruit = true;
	} else // otherwise, the year has restarted and fruit quality can increase again
		killAllFruit = false;
	if (index == 0) { // if it's the beginning of the year, fruit quality restarts at 0.05
		currentFruitQ = 0.05;
	}
	
	if (killAllFruit)
		fruitQLag = 1; // this is so that the fruit quality does not start increasing again during the year, after quality has reached the cutoff
	
	currentFruitQ = obtainFruitQuality(gt, currentFruitQ, fruitQLag, dt, params); // calculate current fruit quality
	
	if (round2Decimals(currentFruitQ) == 1 && dayCrossedMaxFruit == -1)
		dayCrossedMaxFruit = timeStep;
	
	fruitQualities[((int) timeStep) % 365] = currentFruitQ; // store the fruit quality for the current timestep in the array
	// Note: only one fruit quality is stored per timestep (i.e. not one value per dt)
	
	population.computePopulation(temperature, currentFruitQ, params, ignoreFruit, ignoreDiapause, dt, timeStep); // update the population
	
	// update the stage-specific population data series
	eggSeries.push_back(XYPair(timeStep, getEggs()));
	inst1Series.push_back(XYPair(timeStep, getInst1()));
	inst2Series.push_back(XYPair(timeStep, getInst2()));
	inst3Series.push_back(XYPair(timeStep, getInst3()));
	pupaeSeries.push_back(XYPair(timeStep, getPupae()));
	malesSeries.push_back(XYPair(timeStep, getMales()));
	femalesSeries.push_back(XYPair(timeStep, getFemales()));
	
	const double* femStagePopulation = getFemStages();
	for (int i = 0; i < 7; i ++)
		femaleStageSeries[i].push_back(XYPair(timeStep, femStagePopulation[i]));

	fruitQualitySeries.push_back(XYPair(timeStep, currentFruitQ)); // update fruit quality data series
	
	// update cumulative totals and max populations/days

	totEggs += getEggs() * dt;
	totInst1 += getInst1() * dt;
	totInst2 += getInst2() * dt;
	totInst3 += getInst3() * dt;
	totPupae += getPupae() * dt;
	totMales += getMales() * dt;
	totFemales += getFemales() * dt;
	
	if (maxEggs < getEggs()) {
		maxEggs = getEggs();
		maxEggsDay = timeStep;
	}
	
	if (maxInst1 < getInst1()) {
		maxInst1 = getInst1();
		maxInst1Day = timeStep;
	}
	
	if (maxInst2 < getInst2()) {
		maxInst2 = getInst2();
		maxInst2Day = timeStep;
	}
	
	if (maxInst3 < getInst3()) {
		maxInst3 = getInst3();
		maxInst3Day = timeStep;
	}
	
	if (maxPupae < getPupae()) {
		maxPupae = getPupae();
		maxPupaeDay = timeStep;
	}
	
	if (maxMales < getMales()) {
		maxMales = getMales();
		maxMalesDay = timeStep;
	}
	
	if (maxFemales < getFemales()) {
		maxFemales = getFemales();
		maxFemalesDay = timeStep;
	}
	
	// this isn't used in the current model, but i'm leaving it in b/c it 
	// could come in handy later both for debugging and for tracing population
	for (int i = 0; i < 10; i ++) { // currently thresholdPop.length == 10
		if (getFemales() >= thresholdPop[i] && thresholdPopDay[i] < 0) {
			thresholdPopDay[i] = timeStep;
		}
	}
	
}


// method to reset the simulation to timestep 0
void SWDCellSingle::resetTime() {
	population.resetPopulation(); // reset the population
	
	dayCrossedMaxFruit = -1; // haven't reached max fruit date yet
	
	for (int i = 0; i < 10; i ++)
		thresholdPopDay[i] = -1;
	
	// reset the data series
	eggSeries.clear();
	inst1Series.clear();
	inst2Series.clear();
	inst3Series.clear();
	pupaeSeries.clear();
	malesSeries.clear();
	femalesSeries.clear();
	
	for (int i = 0; i < 7; i ++)
		femaleStageSeries[i].clear();
	
	fruitQualitySeries.clear();
	
	// now, reset max populations to 0, since they're all 0 at timestep 0
	maxEggs = 0;
	maxInst1 = 0;
	maxInst2 = 0;
	maxInst3 = 0;
	maxPupae = 0;
	maxMales = 0;
	maxFemales = 0;
	
	// reset the maximum population date to timestep 0
	maxEggsDay = 0;
	maxInst1Day = 0;
	maxInst2Day = 0;
	maxInst3Day = 0;
	maxPupaeDay = 0;
	maxMalesDay = 0;
	maxFemalesDay = 0;
	
	totEggs = 0;
	totInst1 = 0;
	totInst2 = 0;
	totInst3 = 0;
	totPupae = 0;
	totMales = 0;
	totFemales = 0;
}

// method to reset the cell parameters to new values specified by the parameters
// object passed in
// can specify with the boolean whether or not to reset the fruit parameters too
void SWDCellSingle::resetCellParams(Parameters &paramsNew, bool resetFruitParams) {
	params.setCopyParams(paramsNew, resetFruitParams);
}

// method to reset only the fruit parameters
errormsg SWDCellSingle::resetFruitParams(std::map<std::string, double> fruitMap) {
	return params.resetFruitParams(fruitMap);
}
