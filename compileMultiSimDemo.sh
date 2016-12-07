#This file is part of the dsPopSim software and is subject to the license distributed
#with the software (see LICENSE.txt and CITATION.txt).  
#Copyright (c) 2016, Aaron B. Langille, Ellen M. Arteca, Jonathan A. Newman
#All rights reserved.

g++ -std=c++11 -pthread MultiCellRCP_demo.cpp SWDSimulatorSingle.cpp SWDCellSingle.cpp SWDPopulation.cpp Parameters.cpp UtilityMethods.cpp SolveParameters.cpp Daylight.cpp EulersMethod.cpp SWDCellMulti.cpp SWDSimulatorMulti.cpp -o multiSimDemo
