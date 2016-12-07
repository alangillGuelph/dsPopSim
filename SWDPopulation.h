/*
This file is part of the dsPopSim software and is subject to the license distributed
with the software (see LICENSE.txt and CITATION.txt).  
Copyright (c) 2016, Aaron B. Langille, Ellen M. Arteca, Jonathan A. Newman
All rights reserved.
*/



#ifndef SWD_POPULATION_H
#define SWD_POPULATION_H

#include "Parameters.h"
#include "UtilityMethods.h"
#include "SolveParameters.h"
#include "EulersMethod.h"
#include "Daylight.h"

/*

	This class describes an SWDPopulation object.  This object tracks the populations of 
	the various different life stages for the flies (egg, pupae, instar1, instar2, instar3, 
	males, and all female stages) over time.
	There are various accessors to see the current values for each lifestage population, 
	and a method for running the lifestages for a timestep (of length dt, where dt is the integration
	step).  There is also a mutator for reseting the current life-stage specific 
	populations to their respective initial populations. The population object also has 
	datafields for the initial populations of each life stage, and there are accessors for each one.

	Method definitions are included in the cpp file, along with descriptions of the code.

*/

class SWDPopulation {

	double currentEggs, currentInst1, currentInst2, currentInst3, currentPupae, currentMales; // the current stage-specific populations
	double currentFemaleStages[7]; // female-stage specific populations
	
	// initially, s1 and s2 are both 0 (start not in diapause) - set in constructor
	int s1, s2;
	bool crossedDiapause;
	bool addInitPop;
	 
	int crossedDiapDay;

public:
	
	// constructors
	SWDPopulation(const Parameters &params); 
	SWDPopulation(); 

	// accessors for the current populations of each stage

	double getEggs() const { return currentEggs; }
	double getInst1() const { return currentInst1; }
	double getInst2() const { return currentInst2; }
	double getInst3() const { return currentInst3; }
	double getPupae() const { return currentPupae; }
	double getMales() const { return currentMales; }

	double getFemales() const;
	const double* getFemStages() const { return currentFemaleStages; }

	// get current population across all stages
	double getTotalPopulation() const { 
		return currentEggs + currentInst1 + currentInst2 + currentInst3 + currentPupae + currentMales + getFemales();
	}

	void setAddInitPop(bool toSet) { addInitPop = toSet; } // set day to add the initial females (if not adding in diapause cross day)
	int getCrossedDiapDay() const { return crossedDiapDay; } // day diapause has been crossed
	
	// changing populations
	void readPopulation(const Parameters &params);
	void resetPopulation();

	// update population to next timestep
	void computePopulation(double temperature, double fruitQuality, const Parameters &params, bool ignoreFruit, bool ignoreDiapause, double dt, double timeStep);

};

#endif