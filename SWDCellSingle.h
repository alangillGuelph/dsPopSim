#ifndef SWD_CELL_SINGLE_H
#define SWD_CELL_SINGLE_H

#include "SWDPopulation.h"

/*

	Header file for the SWDCellSingle.cpp.  Contains the SWDCellSingle class definition,
	described below.  
	Method definitions are included in the cpp file, along with descriptions of the code.

*/

/*
	Class representing a coordinate pair; used to store (time, value) data points.
*/ 
class XYPair {

	double x, y;
	
public:

	XYPair(double xi, double yi) : x(xi), y(yi) {} // constructor
	// accessors for the coordinates
	double getX() const { return x; }
	double getY() const { return y; }

}; 


// XYSeries exists basically for symmetry with the old Java code - XYSeries was a JFreechart 
// object describing a list of (x, y) coordinate pairs of data points
// Here, XYSeries is a list (vector) of XYPairs
typedef std::vector<XYPair> XYSeries;


/*
	This class describes an SWDCellSingle object, modeling the environment for
	an SWDPopulation.  This class has a Parameters object representing the various 
	parameters to calculate the vitals for the flies and the fruit quality, given 
	the temperature of the cell.
	There is a general accessor for each of the datafields (from the parameters object),
	and accessors for the population's datafields (using the population's accessors).  
	There is also a mutator to reset the timestep back to 0, and a mutator to reset 
	all of the cell's parameters to those specified in the Parameters object passed in.

	Methods described in-code, in the implementation file SWDCellSingle.cpp.
*/

class SWDCellSingle {
	double temp; // temperature of the cell during the current timestep
	SWDPopulation population; // population of the cell (for all lifestages - for details see SWDPopulation class)
	
	double currentFruitQ; // current fruit quality
	double dayCrossedMaxFruit;
	
	double fruitQualities[365]; // the fruit qualities for one full year
	bool killAllFruit; // has fruit quality passed the cutoff (during the current year)? true or false 
	bool ignoreFruit; // on startup, the default is to ignore the fruit
	bool ignoreDiapause;

	// each series keeps all the data for its respective lifestage (or fruit quality) up to the current timestep
	// there is one data point for every dt
	XYSeries eggSeries, inst1Series, inst2Series, inst3Series, pupaeSeries, malesSeries, femalesSeries, fruitQualitySeries;
	XYSeries femaleStageSeries[7];
	
	Parameters params; // parameters for all life processes stored 
	
	double maxEggs, maxInst1, maxInst2, maxInst3, maxPupae, maxMales, maxFemales; // max population of each respective lifestage
	double maxEggsDay, maxInst1Day, maxInst2Day, maxInst3Day, maxPupaeDay, maxMalesDay, maxFemalesDay; // timestep where the max occured
	
	double totEggs, totInst1, totInst2, totInst3, totPupae, totMales, totFemales; // total cumulative population per stage
	
	// this was basically just here for debugging
	double thresholdPop[10];
	double thresholdPopDay[10];

public:

	SWDCellSingle(Parameters paramsNew, double newTemp = 888); // hooray for default parameters!! :)
	errormsg setSingleParameter(std::string &param, double newVal);
	double getSpecificParameter(std::string &param) const;

	void stepForward(double temperature, bool ignoreFruitNew, bool ignoreDiapauseNew, double dt, double timeStep);
	void resetTime();

	bool getIgnoreFruit() const { return ignoreFruit; }
	double getFruitQuality() const { return currentFruitQ; }
	double getDayCrossedMaxFruit() const { return dayCrossedMaxFruit; }
	double getTemperature() const { return temp; }
	double getThresholdPopDay(int index) const { return thresholdPopDay[index]; }
	void readInitFlies() { population.readPopulation(params); }
	void setAddInitPop(bool toSet) { population.setAddInitPop(toSet); }
	int getCrossedDiapDay() const { return population.getCrossedDiapDay(); }

	double getEggs() const { return population.getEggs(); }
	double getInst1() const { return population.getInst1(); }
	double getInst2() const { return population.getInst2(); }
	double getInst3() const { return population.getInst3(); }
	double getPupae() const { return population.getPupae(); }
	double getMales() const { return population.getMales(); }
	double getFemales() const { return population.getFemales(); }
	double getTotalPopulation() const { return population.getTotalPopulation(); }
	const double* getFemStages() const { return population.getFemStages(); }

	double getMaxFemales() const { return maxFemales; }
	double getMaxMales() const { return maxMales; }
	double getMaxPupae() const { return maxPupae; }
	double getMaxInst3() const { return maxInst3; }
	double getMaxInst2() const { return maxInst2; }
	double getMaxInst1() const { return maxInst1; }
	double getMaxEggs() const { return maxEggs; }

	double getDayMaxFemales() const { return maxFemalesDay; }
	double getDayMaxMales() const { return maxMalesDay; }
	double getDayMaxPupae() const { return maxPupaeDay; }
	double getDayMaxInst3() const { return maxInst3Day; }
	double getDayMaxInst2() const { return maxInst2Day; }
	double getDayMaxInst1() const { return maxInst1Day; }
	double getDayMaxEggs() const { return maxEggsDay; }

	double getTotFemales() const { return totFemales; }
	double getTotMales() const { return totMales; }
	double getTotPupae() const { return totPupae; }
	double getTotInst3() const { return totInst3; }
	double getTotInst2() const { return totInst2; }
	double getTotInst1() const { return totInst1; }
	double getTotEggs() const { return totEggs; }

	Parameters getParams() const { return Parameters(params); }
	void resetCellParams(Parameters &paramsNew, bool resetFruitParams);
	errormsg resetFruitParams(std::map<std::string, double> fruitMap);

	const XYSeries& getEggSeries() const { return eggSeries; }
	const XYSeries& getInst1Series() const { return inst1Series; }
	const XYSeries& getInst2Series() const { return inst2Series; }
	const XYSeries& getInst3Series() const { return inst3Series; }
	const XYSeries& getPupaeSeries() const { return pupaeSeries; }
	const XYSeries& getMalesSeries() const { return malesSeries; }
	const XYSeries& getFemalesSeries() const { return femalesSeries; }
	const XYSeries& getFruitQualitySeries() const { return fruitQualitySeries; }
	const XYSeries& getFemaleStageSeries(int index) const { return femaleStageSeries[index]; }
		
};

#endif