#include "core/file_handler.hpp"


/****************************************/
/*          Public Functions            */
/****************************************/


// initiliazes the file handler for writing test.pop and test.best
//files
void FileHandler::init(time_t time, unsigned int maxPop, unsigned int maxGen, unsigned int numExp, int maxWait, int maxEval, bool writeToDebugFile, bool PrintToScreen, bool nodePose, bool placement, bool stepSim, bool ifTexture)
{
	char popFileName[100];
    char bestFileName[100];
    
    sprintf(popFileName, "./data/%ld.pop", time);
    sprintf(bestFileName, "./data/%ld.best", time);
    
    popFile = fopen(popFileName, "w+");
    bestFile = fopen(bestFileName, "w+");
    
    writeHeader(popFile, time, maxPop, maxGen, numExp, maxWait, maxEval, writeToDebugFile, PrintToScreen, nodePose, placement, stepSim, ifTexture);
    writeHeader(bestFile, time, maxPop, maxGen, numExp, maxWait, maxEval, writeToDebugFile, PrintToScreen, nodePose, placement, stepSim, ifTexture);
    
    if(writeToDebugFile)
    {
    	char debugFileName[100];
    	
    	sprintf(debugFileName, "./data/%ld.debug", time);
    	debugFile = fopen(debugFileName, "w+");
    }
}

//closes both files
void FileHandler::close(bool writeToDebugFile)
{
	fclose(bestFile);
    fclose(popFile);
    if (writeToDebugFile) {
    	fclose(debugFile);
    }
}


/*Writing*/

//writes the header of the test.pop or test.best files
void FileHandler::writeHeader(FILE* file, time_t time, unsigned int maxPop, unsigned int maxGen, unsigned int numExp, int maxWait, int maxEval, bool writeToDebugFile, bool PrintToScreen, bool nodePose, bool placement, bool stepSim, bool ifTexture)
{
	fprintf(file, "Time seed: %ld\n", time);
	fprintf(file, "MAX_POPULATION = %u\n", maxPop);
    fprintf(file, "MAX_GENERATION = %u\n", maxGen);
    fprintf(file, "NUM_EXPANSIONS = %u\n", numExp);
    fprintf(file, "MAX_WAIT_TIME = %d\n", maxWait);
    fprintf(file, "MAX_EVAL_TIME = %d\n", maxEval);
    fprintf(file, "WRITE_TO_DEBUG = %d\n", writeToDebugFile);
    fprintf(file, "PRINT_TO_SCREEN = %d\n", PrintToScreen);
    fprintf(file, "DEBUG_NODE_POSITIONS = %d\n", nodePose);
    fprintf(file, "DEBUG_PLACEMENT = %d\n", placement);
    fprintf(file, "STEP_SIMULATION = %d\n", stepSim);
    fprintf(file, "TEXTURES = %d\n", ifTexture);
    
    fflush(file);
}

//writes a line of data into the .pop file
void FileHandler::writePopFile(unsigned int gen, char* rules)
{
	fprintf(popFile,"Generation %4u: %s", gen, rules);
    fflush(popFile);
}

//writes a line of data into the .best file
void FileHandler::writeBestFile(unsigned int gen, char* rules)
{
	fprintf(bestFile,"Generation %4u: %s", gen, rules);
    fflush(bestFile);
}

//writes a line of data into the .debug file
void FileHandler::writeDebugFile(char* debugString)
{
	fprintf(debugFile, "%s", debugString);
	fflush(debugFile);
}

/*Reading*/



//reads the header of the test.best file and sets variables 
//according to file
void FileHandler::readHeader(FILE* readFile, time_t* time, unsigned int* maxPop, unsigned int* maxGen, unsigned int* numExp, int* maxWait, int* maxEval, bool* writeToDebugFile, bool* PrintToScreen, bool* nodePose, bool* placement, bool* stepSim, bool* ifTexture, bool debug)
{
	int check = 0;
	
	check += fscanf(readFile, "Time seed: %ld\n", time);
	check += fscanf(readFile, "MAX_POPULATION = %u\n", maxPop);
	check += fscanf(readFile, "MAX_GENERATION = %u\n", maxGen);
	check += fscanf(readFile, "NUM_EXPANSIONS = %u\n", numExp);
	check += fscanf(readFile, "MAX_WAIT_TIME = %d\n", maxWait);
	check += fscanf(readFile, "MAX_EVAL_TIME = %d\n", maxEval);
	
	int temp;
	check += fscanf(readFile,"WRITE_TO_DEBUG = %d\n", &temp);
	convertToBool(writeToDebugFile, temp);
	
    check += fscanf(readFile, "PRINT_TO_SCREEN = %d\n", &temp);
    convertToBool(PrintToScreen, temp);
    
    check += fscanf(readFile, "DEBUG_NODE_POSITIONS = %d\n", &temp);
    convertToBool(nodePose, temp);
    
    check += fscanf(readFile, "DEBUG_PLACEMENT = %d\n", &temp);
    convertToBool(placement, temp);
    
    check += fscanf(readFile, "STEP_SIMULATION = %d\n", &temp);
    convertToBool(stepSim, temp);
    
    check += fscanf(readFile, "TEXTURES = %d\n", &temp);
    convertToBool(ifTexture, temp);
	if (debug)
	{
		printf("check should be 12 and it is: %d\n", check);
	}
	
}


//reads one line of a test.best results and sets variables 
//according to file
bool FileHandler::readBest(FILE* readFile, unsigned int* gen, char* rules, float* fitness, bool debug)
{
	char*        rule1 = (char *)malloc(10 * sizeof(char));
	char*        rule2 = (char *)malloc(10 * sizeof(char));
	char*        rule3 = (char *)malloc(10 * sizeof(char));
	char*        rule4 = (char *)malloc(10 * sizeof(char));
	char*        rule5 = (char *)malloc(10 * sizeof(char));
	
	
	int check = 0;
	
	check = fscanf(readFile, "Generation %u: Grammar: rules = %s %s %s %s %s fitness = %f\n", gen, rule1, rule2, rule3, rule4, rule5, fitness);
	
	sprintf(rules, "%s%s%s%s%s", rule1, rule2, rule3, rule4, rule5);
	
	if(debug)
	{
		printf("check check should be 7: %d\n", check);
		printf("rule1: %s rule2: %s rule3: %s rule4: %s rule5: %s\n", rule1, rule2, rule3, rule4, rule5);
	}
	
	return check == 7;
		
}


//reads config file and set variables according to file
void FileHandler::readConfig(char* filePath, unsigned int* maxPop, unsigned int* maxGen, unsigned int* numExp, int* maxWait, int* maxEval, bool* writeToDebugFile, bool* PrintToScreen, bool* nodePose, bool* placement, bool* stepSim, bool* ifTexture, bool debug)
{
	FILE* configFile = fopen(filePath, "r");
	
	int check = 0;
	
	check += fscanf(configFile, "MAX_POPULATION = %u\n", maxPop);
	check += fscanf(configFile, "MAX_GENERATION = %u\n", maxGen);
	check += fscanf(configFile, "NUM_EXPANSIONS = %u\n", numExp);
	check += fscanf(configFile, "MAX_WAIT_TIME = %d\n", maxWait);
	check += fscanf(configFile, "MAX_EVAL_TIME = %d\n", maxEval);
	
	int temp;
	check += fscanf(configFile,"WRITE_TO_DEBUG = %d\n", &temp);
	convertToBool(writeToDebugFile, temp);
	
    check += fscanf(configFile, "PRINT_TO_SCREEN = %d\n", &temp);
    convertToBool(PrintToScreen, temp);
    
    check += fscanf(configFile, "DEBUG_NODE_POSITIONS = %d\n", &temp);
    convertToBool(nodePose, temp);
    
    check += fscanf(configFile, "DEBUG_PLACEMENT = %d\n", &temp);
    convertToBool(placement, temp);
    
    check += fscanf(configFile, "STEP_SIMULATION = %d\n", &temp);
    convertToBool(stepSim, temp);
    
    check += fscanf(configFile, "TEXTURES = %d\n", &temp);
    convertToBool(ifTexture, temp);
	
	// printf("writeToDebugFile: %d\nPrintToScreen: %d\nnodePose: %d\nplacement: %d\nstepSim: %d\nifTexture: %d\n", *writeToDebugFile, *PrintToScreen, *nodePose, *placement, *stepSim, *ifTexture);
	if (debug)
	{
		printf("check should be 11 and it is: %d\n", check);
	}
	
	fclose(configFile);
	
}


/****************************************/
/*          Private Functions           */
/****************************************/

void FileHandler::convertToBool(bool* theBoolean, int temp)
{
	*theBoolean = temp == 1 ? true : false;//static_cast<bool>(temp);
}








