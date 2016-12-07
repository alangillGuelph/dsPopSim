#include "SolveParameters.h"

/**

	This file contains methods for calculating the model parameters - most
	of these are dependent on temperature or daylight hours; either way, they
	are dependent on timestep-specific values, but are not modelled by 
	differential equations.  
	Here we have methods to compute fecundity, diapause effect on fecundity, 
	the switch functions for turning diapause on/off, development, mortality, and
	effects on fruit quality.  All of these are modelled by equations included 
	in the model paper.

*/


// method to calculate the fecundity at a given temperature, given the parameters
// of the current simulation (specified in the parameters object passed in)
double solveSpecificFertility(double T, const Parameters &params) {
	
	long double d = 5.88L;
	long double l = 52.68L;
	
	if (T > params.getParameter("fertility tmax"))
		return 0;
	
	if((pow(T,2) + pow(d,2)) < pow(l,2)){
		long double temp1 = 3.3315e-304L;
		long double temp2 = 2740.50L - pow((-23.26L+T),2L);
		if (temp2 <= 0)
			return 0;
	    long double temp3 = exp(log(temp2) * 88.38L);
		
		long double fertility = temp1 * temp3;
		return (double) fertility;
	}
	else 
		return 0;
}

// method to calculate the effect of diapause on fecundity
// this is a multiplicative effect - the multiplier is what is returned from this method
// this is dependent on the number of daylight hours
double solveFertilityDiapauseEffect(double hours) {
	/*
	 * So, as I understand this:
	 * 1. solve the logistic function for the value of the the number of females in diapause 
	 * 2. the effect on the fecundity is 100 - this value
	 * 3. then, it is a multiplier for the fecundity (but divide by 100 first)
	 */
	
	double A = 0.04056;
	double K = 99.8;
	double v = 1.2428535918;
	double M = 0;
	long double Q = 3.23967951563418E-016L; // has to be long double
	double B = -2.871323611;
	
	long double expo = Q * exp(-B * (hours - M));	
	// then, this value should be an ok value for doubles to store (i.e. instead of long double)
	
	double denom = 1 + (double) expo;
	double effect = A + (K - A) / pow(denom, (1/v));
	
	// then, effect is a value between 0 and 100, where 100 is all females are in diapause
	effect = 100 - effect;
	// now, turn this into a percentage
	effect /= 100;
	
	return effect;
}

// method to return the current timestep value of s1, the first switch function for diapause
// note that this is a step function whose value is either 0 or 1.
int solveDiapauseMultS1(double hours, double temp, int s1prev, int s2prev, double tCrit, double daylightHours) {
	if (s1prev * s2prev > 0 && hours < daylightHours)
		return 0;
	else if (s2prev == 0 && temp > tCrit)
		return 1;
	else
		return s1prev;
}

// method to return the current timestep value of s2, the second switch function for diapause
// note that this, like s1, is a step function whose value is either 0 or 1
// this is coupled with s1 to control the switching on and off of diapause
int solveDiapauseMultS2(double hours, int s1prev, int s2prev, double daylightHours) {
	if (s1prev == 0)
		return 0;
	else if (hours >= daylightHours)
		return 1;
	else
		return s2prev;
}

// method to compute the development value for the non-adult stages, using the Briere equation
// the only difference between the computations for different stages is the stage-specific multiplier
// which is passed into this method as a parameter
// the development is temperature dependent, so the temperature value is also passed in
double solveDev_Briere_Juvenile(double T, double devMult) {
	// here, we're using the values from the calculated equation for egg-to-adult development rate
	// the equation is in the form: 1/d = aT(T-T0)sqrt(TL-T)
	
	double a = 0.0001113;
	double T0 = 9.8504;
	double TL = 30.99;
	
	// if temperature is above max or below min, no development
	if (T > TL || T < T0)
		return 0;
	
	
	double devRate = a * T * (T-T0) * sqrt(TL - T);

	devRate /= devMult; 
	return devRate;
}

// method to compute the stage-specific mortality at the given temperature
// similar idea to development - the mortality is calculated via the same equation, but 
// with stage dependent values which are passed in via the specified stage and
// the parameters object
double solveMortality(double T, const Parameters &params, std::string stage) {		
	
	std::vector<double> betas = params.getMortBetas(stage);
	double Tlower = params.getParameter(stage + " mortality min temp");
	double Tupper = params.getParameter(stage + " mortality max temp");
	double maxM = params.getParameter(stage + " mortality max");
	double tau = params.getParameter(stage + " mortality tau");
	
	if ((!(Tlower <= T && T <= Tupper))) // if temperature is not within tolerable range, max mortality is reached
		return maxM; 
	
	double mortality = 0; // mortality is a sum, initialize outside the loop
	
	for (int i = 0; i < 4; i ++) 
		mortality += betas[i] * pow((T - tau), i);
	
	return mortality;
}

// method to compute the value of G(T) (i.e. G at the specified temperature)
// which is a necessary intermediate result for fruit quality calculation
double getGT(double baseTemp, double currentTemp) {
	if (currentTemp <= baseTemp)
		return std::numeric_limits<double>::quiet_NaN();;
	double gt = 1100 / (currentTemp - baseTemp) + 30; // a reciprocal function estimation (1 asymptote)
	return gt;
	
}

// method for computing the effect of fruit quality on development
// this is a multiplier for development (as specified in the model explanation)
// relevant constants are passed in via a parameters object
double solveDevelopmentPlantEffect(double fruitQConstant, double currentQuality, const Parameters &params) {
	
	double m = params.getParameter("fruit m");
	double n = params.getParameter("fruit n");
	
	double ratio = pow((currentQuality / fruitQConstant), n);
	double effect = m * ratio * pow((1 + ratio), -1) + 1 - m;
	
	return effect;
}

// method for computing the stage-specific effect of fruit quality on mortality
// this is an additive effect on mortality (as specified in the model explanation)
// relevant constants are passed in via a parameters object
// here stage is also passed in, since the effect is stage-specific
double solveMortalityPlantEffect(double fruitQConstant, double currentQuality, const Parameters &params, std::string stage) {
	
	double n = params.getParameter("fruit n");
	double maxMort = params.getParameter(stage + " mortality max");
	
	double m = 0.1 * maxMort;
	
	double ratio = pow((currentQuality / fruitQConstant), n);
	double effect = m * pow((1 + ratio), -1);
	
	return effect;
}
