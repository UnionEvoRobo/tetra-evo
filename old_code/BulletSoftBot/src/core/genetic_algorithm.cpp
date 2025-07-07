#include <cstdlib>      //srand, rand, abs
#include <cstdio>       //printf, FILE, fopen, fclose, fprintf, sprintf
#include <ctime>        //time

#include <vector>       //std::vector
#include <algorithm>    //std::sort, std::max
#include <iostream>
#include <fstream>		// reading and writing files
#include <cstdio>       //

#include "core/genetic_algorithm.hpp"
#include "core/grammar.hpp"
#include "core/file_handler.hpp"
#include "physics/simulation.hpp"
#include "physics/softbody_data.hpp"

#ifdef GRAPHICS
#include "graphics/graphics.hpp"
#endif


unsigned int g_generation = 1;

std::vector<Grammar>           population;
std::vector<Grammar>::iterator currentIndividual;

//Simulation simulation;
//FileHandler fh;

#ifdef GRAPHICS
    genetic_algorithm::genetic_algorithm(Graphics* graphics, Simulation* sim, time_t TIME, FileHandler* fh, unsigned int NUM_EXPANSIONS, unsigned int NUM_POPULATION, unsigned int NUM_GENERATION, int WAIT_TIME, int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT, bool STEP_SIMULATION, bool TEXTURES) : 
        myGraphics(graphics),
        simulation(sim),
        TIME(TIME),
        fh(fh),
        NUM_EXPANSIONS(NUM_EXPANSIONS),
        NUM_POPULATION(NUM_POPULATION),
        NUM_GENERATION(NUM_GENERATION),
        WAIT_TIME(WAIT_TIME),
        EVAL_TIME(EVAL_TIME),
        WRITE_TO_DEBUG(WRITE_TO_DEBUG),
        PRINT_TO_SCREEN(PRINT_TO_SCREEN),
        DEBUG_NODE_POSITIONS(DEBUG_NODE_POSITIONS),
        DEBUG_PLACEMENT(DEBUG_PLACEMENT),
        STEP_SIMULATION(STEP_SIMULATION),
        TEXTURES(TEXTURES)
    {}
#else
    genetic_algorithm::genetic_algorithm(Simulation* sim, time_t TIME, FileHandler* fh, unsigned int NUM_EXPANSIONS, unsigned int NUM_POPULATION, unsigned int NUM_GENERATION, int WAIT_TIME, int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT, bool STEP_SIMULATION, bool TEXTURES) :
        simulation(sim),
        TIME(TIME),
        fh(fh),
        NUM_EXPANSIONS(NUM_EXPANSIONS),
        NUM_POPULATION(NUM_POPULATION),
        NUM_GENERATION(NUM_GENERATION),
        WAIT_TIME(WAIT_TIME),
        EVAL_TIME(EVAL_TIME),
        WRITE_TO_DEBUG(WRITE_TO_DEBUG),
        PRINT_TO_SCREEN(PRINT_TO_SCREEN),
        DEBUG_NODE_POSITIONS(DEBUG_NODE_POSITIONS),
        DEBUG_PLACEMENT(DEBUG_PLACEMENT),
        STEP_SIMULATION(STEP_SIMULATION),
        TEXTURES(TEXTURES)
    {}
#endif

//Run the genetic algorithm with given parameters
void genetic_algorithm::run() {
    initialize(NUM_POPULATION);

    fh->init(TIME, NUM_POPULATION, NUM_GENERATION, NUM_EXPANSIONS, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);

    for (unsigned int i = 0; i < NUM_GENERATION; ++i) {
        evaluate(NUM_EXPANSIONS, STEP_SIMULATION, TEXTURES, i+1);
        diversify();
        select(fh);
        breed();
        print();
    }
    
    //close function
    fh->close(WRITE_TO_DEBUG);
}

//runs the genetic algorithm from a config file
// void runFromConfig(char* filePath)
// {
// 	fh.readConfig(filePath, &NUM_POPULATION, &NUM_GENERATION, &NUM_EXPANSIONS, &WAIT_TIME, &EVAL_TIME, &WRITE_TO_DEBUG, &PRINT_TO_SCREEN, &DEBUG_NODE_POSITIONS, &DEBUG_PLACEMENT, &STEP_SIMULATION, &TEXTURES, false); 
// 	//printf("read config\n");
	
// 	simulation.setWaitTime(WAIT_TIME);
// 	simulation.setEvalTime(EVAL_TIME);
// 	simulation.setFlags(WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);
	
// 	//printf("set flags\n");
// 	simulation.setFH(&fh);
	
// 	//printf("set simulation\n");
	
// 	printf("NUM_POPULATION = %u\n", NUM_POPULATION);
//     printf("NUM_GENERATION = %u\n", NUM_GENERATION);
//     printf("NUM_EXPANSIONS = %u\n", NUM_EXPANSIONS);
	
	
// 	initialize();
    
//     //init function
//     fh.init(TIME, NUM_POPULATION, NUM_GENERATION, NUM_EXPANSIONS, simulation.getWaitTime(), simulation.getEvalTime(), WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);
    
//     //printf("init fh\n");
    
    

//     for(unsigned int i = 0; i < NUM_GENERATION; ++i){
//         evaluate();
//         diversify();
//         select();
//         breed();
//         print();
//     }
    
//     //close function
//     fh.close(WRITE_TO_DEBUG);
// }


//given a grammar and how many times to expand the grammar runs a sim of the grammar
void genetic_algorithm::runGraphicsSim(Grammar* grammar, unsigned int num_expansions)
{
    resetSimulation();
	SoftBodyData mySoftBody = SoftBodyData(grammar,num_expansions);
    simulation->initialize(&mySoftBody);
    
    while(!simulation->simulate(0.1)) {
        #ifdef GRAPHICS
            myGraphics->update();
        #endif
    };
}


//We don't want one simulation to affect the next, so let's delete it and make a new one to ensure independence
void genetic_algorithm::resetSimulation() {
    delete simulation;
    simulation = new Simulation(fh, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);
    #ifdef GRAPHICS
        myGraphics->setSimulation(simulation);
    #endif
}


//runs the given rule in a graphics Sim with the given configuration
//rule must be in format: icaccgbdbdgbadagcbcdrdccb
// void runRuleWithConfig(char* filePath, char* rules)
// {
// 	Grammar test = Grammar(rules); 
	
// 	fh.readConfig(filePath, &NUM_POPULATION, &NUM_GENERATION, &NUM_EXPANSIONS, &WAIT_TIME, &EVAL_TIME, &WRITE_TO_DEBUG, &PRINT_TO_SCREEN, &DEBUG_NODE_POSITIONS, &DEBUG_PLACEMENT,   &STEP_SIMULATION, &TEXTURES, false); 
	
// 	simulation.setWaitTime(WAIT_TIME);
// 	simulation.setEvalTime(EVAL_TIME);
// 	simulation.setFlags(WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);
	
// 	printf("NUM_POPULATION = %u\n", NUM_POPULATION);
//     printf("NUM_GENERATION = %u\n", NUM_GENERATION);
//     printf("NUM_EXPANSIONS = %u\n", NUM_EXPANSIONS);
    
    
//     runGraphicsSim(&test, NUM_EXPANSIONS, 0, STEP_SIMULATION, TEXTURES);
// }

//reads a file of a test.best format and runs a sim of the soft 
// robots it contains
void genetic_algorithm::readFile(FileHandler* fh, char* filePath, bool runMovie, bool loopBest, bool debug, unsigned int NUM_EXPANSIONS, unsigned int NUM_POPULATION, unsigned int NUM_GENERATION, int WAIT_TIME, int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT, bool STEP_SIMULATION, bool TEXTURES)
{
	int          maxWait;
	int          maxEval;
	char*        rules = (char *)malloc(100 * sizeof(char));
	unsigned int gen = 0;
	float        fitness = 1.0;
	
	FILE* readFile;
	do
	{
		readFile = fopen(filePath, "r");
	
		//read header
		fh->readHeader(readFile, &TIME, &NUM_POPULATION, &NUM_GENERATION, &NUM_EXPANSIONS, &maxWait, &maxEval, &WRITE_TO_DEBUG, &PRINT_TO_SCREEN, &DEBUG_NODE_POSITIONS, &DEBUG_PLACEMENT, &STEP_SIMULATION, &TEXTURES, false);
		
		if(debug)
		{
			printf("This is what I got:\n");
			printf(" time: %ld\n maxPop: %u\n maxGen: %u\n numExp: %u\n maxWait: %d\n maxEval: %d\n", TIME, NUM_POPULATION, NUM_GENERATION, NUM_EXPANSIONS, maxWait, maxEval);
		} 
		
	
		//set simulation
		simulation->setWaitTime(maxWait);
		simulation->setEvalTime(maxEval);
		simulation->setFlags(WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);
	
		bool goodRead = false;
		
		while(!feof(readFile))
		{
			//read line from best file
			goodRead = fh->readBest(readFile, &gen, rules, &fitness, false);
			
			if (debug)
			{
				printf(" Gen: %u\n best rules: %s\n fitness: %f\n", gen, rules, fitness);
			}
			
			//run sim of every soft robot
			if(runMovie && goodRead)
			{
				Grammar g = Grammar(rules);
				runGraphicsSim(&g, NUM_EXPANSIONS);
			}
			else if (!goodRead)
			{
				printf("Not a good read not running movie\n");
			}
		}
		
		if(debug)
		{
			printf(" End of File\n");
		}
		
		
		//run sim of the last soft robot
		if(!runMovie && goodRead)
		{
			Grammar g = Grammar(rules);
			runGraphicsSim(&g, NUM_EXPANSIONS);
		}
		else if (!goodRead)
		{
			printf("Not a good read not running sim\n");
		}
		
		fclose(readFile);
	}
	//loop if needed
	while(loopBest);			
}

/****************************************/
/*           Local Functions            */
/****************************************/


void genetic_algorithm::initialize(int NUM_POPULATION) {
    /* Initializes population */
    
    srand(TIME);
    
    population.reserve(NUM_POPULATION);

    for(int i = 0; i < NUM_POPULATION; ++i){
        population.emplace_back();
    }

    currentIndividual = population.begin();
    
    
    
}

void genetic_algorithm::evaluate(int NUM_EXPANSIONS, bool STEP_SIMULATION, bool TEXTURES, unsigned int generation) {
    /* Evaluates all individuals yet to be evaluated */
    unsigned int i;
    if (generation != 1) {
        i = NUM_POPULATION/2 + 1;//since we sort the pop and replace the 2nd half, we don't need to resimulate the first half
    } else {
        i = 1;
    }
    for(; currentIndividual != population.end(); ++currentIndividual) {
        Grammar* grammar = &(*currentIndividual);
        #ifdef GRAPHICS
            char title[150];
            sprintf(title, "Bullet Soft Bot Generation %u of %u, individual %u of %u: %s", generation, NUM_GENERATION, i, NUM_POPULATION, currentIndividual->toString());
            myGraphics->setTitle(title);
        #endif
        runGraphicsSim(grammar, NUM_EXPANSIONS);
        i++;
    }
}

void genetic_algorithm::diversify(){
    /* Diversifies the individuals in the population */
    
    const float diversityScalar = 0.1;

    for(unsigned int i = 0; i < population.size()-1; ++i){
        float fitness1 = population[i].getFitness();

        for(unsigned int j = i+1; j < population.size(); ++j){
            float fitness2 = population[j].getFitness();

            if(fitness1 != 0 && fitness1 == fitness2){
                population[i].setFitness(0.0);
            }else if(fabs(fitness1 - fitness2) < diversityScalar*fitness1){
                int indexDiff   = j - i;
                int grammarDiff = population[i].getDifference(population[j]);
                
                if(grammarDiff < 3*indexDiff){
                    population[j].setFitness(0.0);
                }
            }
        }
    }
}

void genetic_algorithm::select(FileHandler* fh){
    /* Selects the best fitnesses from the population */
    
    std::sort(population.begin(), population.end(), GrammarComparator());

    //record fitnesses and rules for all populations for all generations
    for(auto individual = population.begin(); individual != population.end(); ++individual)
     {    
     	//write popfile
     	fh->writePopFile(g_generation, (*individual).toString());
     }
    
    //write best file
    //record best fitness and rule for each generation
    fh->writeBestFile(g_generation, population.front().toString());    
     
    //currentIndividual = start of population that will be replaced (the lower half)
    currentIndividual = population.begin() + (population.size() / 2);
}

void genetic_algorithm::breed(){
    /* Replaces the lower fitness individuals with "newly breed" individuals */
    /* NOTE: Breeding just replaces the rules of an individual */
    
    std::vector<int> roulette;
    
    fillRoulette(&roulette);

    for(auto individual = currentIndividual; individual != population.end(); ++individual){
        int r = rand() % 100;

        int p1;
        int p2;
        
        if(r < 40){
            do{
                int p1index = rand() % roulette.size();
                int p2index = rand() % roulette.size();

                p1 = roulette[p1index];
                p2 = roulette[p2index];
            }while(p1 == p2);
            
            (*individual).setFitness(0.0);
            (*individual).setRules(population[p1].getRules());
            (*individual).crossover(population[p2]);
        }else{
            int p1index = rand() % roulette.size();

            p1 = roulette[p1index];
            
            (*individual).setFitness(0.0);
            (*individual).setRules(population[p1].getRules());
            (*individual).mutate();
            (*individual).mutate();
        }
    }
    
    g_generation++;
}
void genetic_algorithm::print(){
    /* Print best individual of generation */
	
    printf("Generation %4u: ", g_generation - 1);
    population.front().print();
}

void genetic_algorithm::fillRoulette(std::vector<int>* roulette){
    /* Creates a roulette for breeding. Assumes sorted population. */

    roulette->reserve(population.size() + 50);

    float best    = population.front().getFitness();
    float worst   = population.back().getFitness();
    float bestFit = (best - worst) == 0 ? 1 : best - worst;
    
    int magFactor = 1;
    while(magFactor * bestFit < 10){
        magFactor *= 10;
    }
    
    for(unsigned int i = 0; i < population.size(); ++i){
        float currentFit = population[i].getFitness() - worst;

        int intFactor = std::max(1, (int)(currentFit*magFactor));

        for(int j = 0; j < intFactor; ++j){
            roulette->emplace_back(i);
        }
    }
}

