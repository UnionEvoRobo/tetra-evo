#ifndef CORE_FILE_HANDLER_HPP
#define CORE_FILE_HANDLER_HPP


#include <cstdio>       //printf, FILE, fopen, fclose, fprintf, sprintf
#include <ctime>        //time
#include <iostream>
#include <fstream>		// reading and writing files
#include <cstdio>       //


using namespace std;      // for string, vector, iostream and other standard C++ stuff

class FileHandler{

	public:
	
		void init(time_t time, unsigned int maxPop, unsigned int maxGen, unsigned int numExp, int maxWait, int maxEval, bool writeToDebugFile, bool PrintToScreen, bool nodePose, bool placement, bool stepSim, bool ifTexture);
		
		void close(bool writeDebugFile);
		
		//writing
		void writeHeader(FILE* file, time_t time, unsigned int maxPop, unsigned int maxGen, unsigned int numExp, int maxWait, int maxEval, bool writeToDebugFile, bool PrintToScreen, bool nodePose, bool placement, bool stepSim, bool ifTexture);
		
		void writePopFile(unsigned int gen, char* rules);
		
		void writeBestFile(unsigned int gen, char* rules);
		
		void writeDebugFile(char* debugString);
		
		//reading
		void readHeader(FILE* readFile, time_t* time, unsigned int* maxPop, unsigned int* maxGen, unsigned int* numExp, int* maxWait, int* maxEval, bool* writeToDebugFile, bool* PrintToScreen, bool* nodePose, bool* placement, bool* stepSim, bool* ifTexture, bool debug);
		
		bool readBest(FILE* readFile, unsigned int* gen, char* rules, float* fitness, bool debug);
		
		
		void readConfig(char* filePath, unsigned int* maxPop, unsigned int* maxGen, unsigned int* numExp, int* maxWait, int* maxEval, bool* writeToDebugFile, bool* PrintToScreen, bool* nodePose, bool* placement, bool* stepSim, bool* ifTexture, bool debug);
		
		
		
		
	private:
		FILE* popFile;
		FILE* bestFile;
		FILE* debugFile;
		
		
		void convertToBool(bool* theBoolean, int temp);


};






#endif
