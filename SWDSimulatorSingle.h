#ifndef SWD_SIMULATOR_SINGLE_H
#define SWD_SIMULATOR_SINGLE_H

#include "SWDCellSingle.h"

/*

	This class describes an SWDSimulatorSingle object, which is a simulator to run 
	its cell datafield under various specified conditions.  
	There are accessors for all the cell's accessors (to access the cell's and its
	population's parameters).  
	There are also methods to reset the parameters (i.e. the parameter map) either 
	with arguments or by reading in from a file).
	There are also mutators to run the cell for a specified time interval under 
	specified temperature conditions.

	Method definitions are included in the SWDSimulatorSingle.cpp file, along with descriptions of the code.
*/

class SWDSimulatorSingle {
	SWDCellSingle cell; // cell to run the simulation on
	
	double timeStep; // current timestep
	double dt; // integration step (defaults to 0.05)
	
	bool injectFlies;

public:
	// constructors
	SWDSimulatorSingle(double dtNew = 0.05, std::string fileName = "config.txt");
	SWDSimulatorSingle(double dtNew, Parameters &params);

	// these methods are described in the cpp file
	void setDefaultParams();
	errormsg setSingleParameter(std::string &param, double newVal);
	double getSpecificParameter(std::string &param) const;
	errormsg setConfigParams(std::string &configFileName);
	errormsg setMapParams(std::map<std::string, double> inputMap);

	// methods to run a sim - also explained in the cpp file
	void run(double temperature, double numTimeSteps, bool ignoreFruit, bool ignoreDiapause);
	void run(std::vector<double> temperatures, double numTimeSteps, bool ignoreFruit, bool ignoreDiapause, int startDay);

	// self-explanatory accessors and mutators, similar to those in the SWDCellSingle class
	void setDT(double dtNew) { dt = dtNew; }
	Parameters getParams() const { return cell.getParams(); }
	double getFruitQuality() const { return cell.getFruitQuality(); }
	double getDayCrossedMaxFruit() const { return cell.getDayCrossedMaxFruit(); }
	double getThresholdPopDay(int index) const { return cell.getThresholdPopDay(index); }

	double getEggs() const { return cell.getEggs(); }
	double getInst1() const { return cell.getInst1(); }
	double getInst2() const { return cell.getInst2(); }
	double getInst3() const { return cell.getInst3(); }
	double getPupae() const { return cell.getPupae(); }
	double getMales() const { return cell.getMales(); }
	double getFemales() const { return cell.getFemales(); }
	const double* getFemStages() const { return cell.getFemStages(); }
	double getTotalPopulation() const { return cell.getTotalPopulation(); }

	double getMaxFemales() const { return cell.getMaxFemales(); }
	double getMaxMales() const { return cell.getMaxMales(); }
	double getMaxPupae() const { return cell.getMaxPupae(); }
	double getMaxInst3() const { return cell.getMaxInst3(); }
	double getMaxInst2() const { return cell.getMaxInst2(); }
	double getMaxInst1() const { return cell.getMaxInst1(); }
	double getMaxEggs() const { return cell.getMaxEggs(); }

	double getDayMaxFemales() const { return cell.getDayMaxFemales(); }
	double getDayMaxMales() const { return cell.getDayMaxMales(); }
	double getDayMaxPupae() const { return cell.getDayMaxPupae(); }
	double getDayMaxInst3() const { return cell.getDayMaxInst3(); }
	double getDayMaxInst2() const { return cell.getDayMaxInst2(); }
	double getDayMaxInst1() const { return cell.getDayMaxInst1(); }
	double getDayMaxEggs() const { return cell.getDayMaxEggs(); }

	double getTotFemales() const { return cell.getTotFemales(); }
	double getTotMales() const { return cell.getTotMales(); }
	double getTotPupae() const { return cell.getTotPupae(); }
	double getTotInst3() const { return cell.getTotInst3(); }
	double getTotInst2() const { return cell.getTotInst2(); }
	double getTotInst1() const { return cell.getTotInst1(); }
	double getTotEggs() const { return cell.getTotEggs(); }

	double getTemperature() const { return cell.getTemperature(); }
	double getTimeStep() const { return timeStep; }
	void resetTime();
	int getCrossedDiapDay() const { return cell.getCrossedDiapDay(); }

	const XYSeries& getEggSeries() const { return cell.getEggSeries(); }
	const XYSeries& getInst1Series() const { return cell.getInst1Series(); }
	const XYSeries& getInst2Series() const { return cell.getInst2Series(); }
	const XYSeries& getInst3Series() const { return cell.getInst3Series(); }
	const XYSeries& getPupaeSeries() const { return cell.getPupaeSeries(); }
	const XYSeries& getMalesSeries() const { return cell.getMalesSeries(); }
	const XYSeries& getFemalesSeries() const { return cell.getFemalesSeries(); }
	const XYSeries& getFruitQualitySeries() const { return cell.getFruitQualitySeries(); }
	const XYSeries& getFemaleStageSeries(int index) const { return cell.getFemaleStageSeries(index); }

	SWDCellSingle& getCell() { return cell; }

};


#endif