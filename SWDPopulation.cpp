/*
This file is part of the dsPopSim software and is subject to the license distributed
with the software (see LICENSE.txt and CITATION.txt).  
Copyright (c) 2016, Aaron B. Langille, Ellen M. Arteca, Jonathan A. Newman
All rights reserved.
*/

#include "SWDPopulation.h"

/*

	Implementation of the SWDPopulation class methods.  Class definition and accessors are 
	included in the SWDPopulation header file.  The overall class description is also included
	in the header file, right above the class definition.

*/

// constructor setting to the original parameters
// note: although the params object passed in is not used, the parameters for 
// population are set later in the readPopulation method
// and here, the params are passed in to differentiate between this and the no-arg 
// constructor below
SWDPopulation::SWDPopulation(const Parameters &params) {
	resetPopulation();
}

// no argument constructor -- this only sets to default parameters, and sets
// the initial population to one egg
SWDPopulation::SWDPopulation() {
	resetPopulation();

	currentEggs = 1;
}

// return total females across all stages, at the current timestep
double SWDPopulation::getFemales() const { 
	return sumDoubleArray(currentFemaleStages, 7); 
} 

// method to read in the population from the params object (i.e. to set the
// initial population to those specified in the parameters)
// this is called when the population is first added (either on diapause crossing, 
// or at the particular timestep specified)
void SWDPopulation::readPopulation(const Parameters &params) {
	addInitPop = true;
		 
	currentEggs = params.getParameter("initial eggs");
	currentInst1 = params.getParameter("initial instar1");
	currentInst2 = params.getParameter("initial instar2");
	currentInst3 = params.getParameter("initial instar3");
	currentPupae = params.getParameter("initial pupae");
	currentMales = params.getParameter("initial males");
	 
	for (int i = 0; i < 7; i ++) {
		std::string stageStr = "initial females";
		int val = i + 1;
		std::stringstream sstm;
		sstm << val;
		stageStr += sstm.str();
		currentFemaleStages[i] = params.getParameter(stageStr);
	}
}

// method to reset the population to simulation step 0 
void SWDPopulation::resetPopulation() {
	currentEggs = 0;
	currentInst1 = 0;
	currentInst2 = 0;
	currentInst3 = 0;
	currentPupae = 0;
	currentMales = 0;

	for (int i = 0; i < 7; i ++)
		currentFemaleStages[i] = 0;

	s1 = 0;
	s2 = 0;

	crossedDiapause = false;
	addInitPop = false;

	crossedDiapDay = -1;
}

// method to move the compute the update of the population over one timestep with the specified parameters
// the temperature of the timestep is specified, and whether or not to ignore diapause and fruit submodels,
// and the integration step (dt -- the length of one timestep), and the current timestep 
// also the parameters of the simulation (as the Parameters object), and the current fruit quality of the model
void SWDPopulation::computePopulation(double temperature, double fruitQuality, const Parameters &params, bool ignoreFruit, bool ignoreDiapause, double dt, double timeStep) { 
		 
	// note: in order of indices: 0-eggs, 1-instar1, 2-instar2, 3-instar3, 4-pupae, 5-males, 6-females
	 
	// no negative populations
	if ( currentMales < 0)
                currentMales = 0;
    if ( currentPupae < 0)
            currentPupae = 0;
    if ( currentInst3 < 0)
            currentInst3 = 0;
    if ( currentEggs < 0)
            currentEggs = 0;
    if ( currentPupae < 0)
            currentPupae = 0;

    for ( int i = 0; i < 7; ++ i) {
            if ( currentFemaleStages[ i] < 0)
                    currentFemaleStages[ i] = 0;

    }
 

	// calculate fertility rate
	double fertility = solveSpecificFertility(temperature, params);
	double fertilityDiapauseEffect = 1;
	
	if (!ignoreDiapause) { 
		int year = ((int) timeStep) / 365;
		int date = ((int) timeStep) % 365;
		int offset = getOffSet(year);
		double latitude = params.getParameter("latitude");
		double hours = getDayLightHours(year, date + offset, latitude);
		 
		double criticalT = params.getParameter("diapause critical temp");
		double daylightHours = params.getParameter("diapause daylight hours");
		 
		// NOTE: don't set s1 here since the previous value of s1 is needed to calculate s2
		int tempS1 = solveDiapauseMultS1(hours, temperature, s1, s2, criticalT, daylightHours); // diapause multiplier (s1)
		s2 = solveDiapauseMultS2(hours, s1, s2, daylightHours); // s2 value for current dt
		s1 = tempS1; // s1 value for current dt
		 
		fertilityDiapauseEffect = s1 * solveFertilityDiapauseEffect(hours);

		// compute switch functions for diapause 
		if (s1 == 0 && !crossedDiapause && !addInitPop)
			return;
		if (s1 != 0 && !crossedDiapause) {
			if (!addInitPop)
				readPopulation(params);
			crossedDiapause = true;
			crossedDiapDay = (int) (timeStep);
		}
		 
	}
	 
	fertility *= fertilityDiapauseEffect; // multiplicative effect of diapause on fecundity (if ignoring diapause, will still be 1)
	 
	std::vector<double> devMaxes = params.getArrayParameters("development max");
	
	double devRate[11]; // development rates per stage
	double mortalityNat[13]; // mortality rates per stage, due to natural causes (food, etc.)
	
	double fruitQConstant = 0.5; // default value taken from the aphid paper
	
	double fruitEffectDevelopment = solveDevelopmentPlantEffect(fruitQConstant, fruitQuality, params);
	 
	for (int i = 0; i < 13; i ++) { // calculate the stage-specific mortality and development rates
		if (i <= 4) // no development rates for adults
			devRate[i] = solveDev_Briere_Juvenile(temperature, devMaxes[i]); 
			//devRate[i] = solveDev_newData(temperature, devMaxes[i]);
		else if (i > 5 && i < 12)
			devRate[i - 1] = devMaxes[i - 1]; // female development is independent of temperature
		mortalityNat[i] = solveMortality(temperature, params, params.getStage(i));
		 
		double fruitEffectMortality = solveMortalityPlantEffect(fruitQConstant, fruitQuality, params, params.getStage(i));
		 
		// plant effect is multiplicative on development, and summative on mortality
		 
		if (ignoreFruit) { // if the user chose to ignore the effect of fruit quality on the flies
			fruitEffectDevelopment = 1; // since fruit is a multiplier for development, to ignore fruit the multiplier is 1
			fruitEffectMortality = 0; // since fruit is a sum on mortality, to ignore fruit the added factor is 0
		}
		 
		if (i < 5) // no development rate for adults
			devRate[i] *= fruitEffectDevelopment; // fruit has a multiplicative effect on development rate
		mortalityNat[i] += fruitEffectMortality; // fruit has a summative effect on mortality rate
	}
	 
	std::vector<double> tempFemalesPopulation = copyDoubleArray(currentFemaleStages, 7); // ensure to use populations from the timestep before
	
	// calculate the current populations of all the lifestages
	
	std::vector<double> eggViabilities = params.getArrayParameters("egg viability");
	std::vector<double> mortalitiesPred = params.getArrayParameters("mortality due to predation");
	
	double maleProportion = params.getParameter("male proportion");
	double maleProportion_advFemStage = 0; // no males develop from previous female lifestages
	
	currentFemaleStages[0] = obtainFemalesX(maleProportion, devRate[4], currentPupae, 
											mortalityNat[6], mortalitiesPred[6], devRate[5], currentFemaleStages[0], dt);
	
	// here, it's ok to go in order since all the populations from the previous timestep are saved in femalesCurrentTimestep
	
	for (int i = 1; i < 7; i ++) {
		if (i < 7 - 1) {
			currentFemaleStages[i] = obtainFemalesX(maleProportion_advFemStage, devRate[i + 4], tempFemalesPopulation[i - 1],
																	mortalityNat[i + 6], mortalitiesPred[i + 6], devRate[i + 5], 
																	currentFemaleStages[i], dt);
		}
		else {
			currentFemaleStages[i] = obtainFemalesX(maleProportion_advFemStage, devRate[i + 4], tempFemalesPopulation[i - 1],
					mortalityNat[i + 6], mortalitiesPred[i + 6], 0, // development rate N/A for females stage 7 
					currentFemaleStages[i], dt);
		}
			
	}
	 
	// note: in order of indices: 0-eggs, 1-instar1, 2-instar2, 3-instar3, 4-pupae, 5-males, 6-females
	
	// computed in reverse order to avoid excess use of temporary variables
	currentMales = obtainMales(devRate[4], maleProportion, mortalityNat[5], mortalitiesPred[5], currentPupae, currentMales, dt);
	currentPupae = obtainPupae(devRate[3], mortalityNat[4], mortalitiesPred[4], devRate[4], currentInst3, currentPupae, dt);
	currentInst3 = obtainInstX(devRate[2], mortalityNat[3], mortalitiesPred[3], devRate[3], currentInst2, currentInst3, dt);
	currentInst2 = obtainInstX(devRate[1], mortalityNat[2], mortalitiesPred[2], devRate[2], currentInst1, currentInst2, dt);
	currentInst1 = obtainInstX(devRate[0], mortalityNat[1], mortalitiesPred[1], devRate[1], currentEggs, currentInst1, dt);
	currentEggs = obtainEggs(fertility, eggViabilities, tempFemalesPopulation, currentEggs, mortalityNat[0], mortalitiesPred[0], devRate[0], dt);

   if ( currentMales < 0)
		currentMales = 0;
	if ( currentPupae < 0)
		currentPupae = 0;
	if ( currentInst3 < 0)
		currentInst3 = 0;
	if ( currentEggs < 0)
		currentEggs = 0;
	if ( currentPupae < 0)
		currentPupae = 0;

	for ( int i = 0; i < 7; ++ i) {
		if ( currentFemaleStages[ i] < 0)
			currentFemaleStages[ i] = 0;
		
	}
	 
}
