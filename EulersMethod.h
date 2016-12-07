#ifndef EULERS_METHOD_H
#define EULERS_METHOD_H

#include "Parameters.h"

/**

	Header file for the EulersMethod.cpp methods.  
	Refer to the commenting and implementation in EulersMethods.cpp for explanations of
	what these methods do.

*/

double obtainEggs(double fertility, std::vector<double> eggViabilities, std::vector<double> femStagePopulations, double eggsI, 
											double eggMortalityNat, double eggMortalityPred, double eggDevelopment, double step);


double obtainInstX(double stageX_1Development, double instXMortalityNat, double instXMortalityPred, double instXDevelopment, 
								double stageX_1I, double instXI, double step);


double obtainPupae(double inst3Development, double pupaeMortalityNat, double pupaeMortalityPred, double pupaeDevelopment, 
								double inst3I, double pupaeI, double step);

double obtainMales(double pupaeDevelopment, double maleProportion, double malesMortalityNat, double malesMortalityPred, 
								double pupaeI, double malesI, double step);


double obtainFemalesX(double maleProportion, double stageX_1Development, double stageX_1I, double femalesXMortalityNat, 
									double femalesXMortalityPred, double femalesXDevelopment, double femalesXI, double step);


double obtainFruitQuality(double gt, double fruitQualityI, double fruitQLag, double step, Parameters& params);


#endif
