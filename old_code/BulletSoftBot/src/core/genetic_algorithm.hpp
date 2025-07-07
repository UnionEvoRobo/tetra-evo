#ifndef CORE_GENETIC_ALGORITHM_HPP
#define CORE_GENETIC_ALGORITHM_HPP

#include <vector>

#include "core/grammar.hpp"
#include "core/file_handler.hpp"

#include "physics/simulation.hpp"

#ifdef GRAPHICS
#include "graphics/graphics.hpp"
#endif

class genetic_algorithm {
	public:
		#ifdef GRAPHICS
			explicit genetic_algorithm(Graphics* graphics, Simulation* simulation, time_t TIME, FileHandler* fh, unsigned int NUM_EXPANSIONS, unsigned int NUM_POPULATION, unsigned int NUM_GENERATION, int WAIT_TIME, int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT, bool STEP_SIMULATION, bool TEXTURES);
		#else
			explicit genetic_algorithm(Simulation* simulation, time_t TIME, FileHandler* fh, unsigned int NUM_EXPANSIONS, unsigned int NUM_POPULATION, unsigned int NUM_GENERATION, int WAIT_TIME, int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT, bool STEP_SIMULATION, bool TEXTURES);
		#endif
		
		void run(unsigned int num_expansions, unsigned int max_population, unsigned int max_generation);
		void run();

		void runFromConfig(char* filePath);

		void runGraphicsSim(Grammar* grammar, unsigned int num_expansions);
		void resetSimulation();

		void runRuleWithConfig(char* filePath, char* rules);

		void readFile(FileHandler* fh, char* filePath, bool runMovie, bool loopBest, bool debug, unsigned int NUM_EXPANSIONS, unsigned int NUM_POPULATION, unsigned int NUM_GENERATION, int WAIT_TIME, int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT, bool STEP_SIMULATION, bool TEXTURES);

	private:
		#ifdef GRAPHICS
        	Graphics* myGraphics;
    	#endif

        Simulation* simulation;

        time_t TIME;
        FileHandler* fh;
        unsigned int NUM_EXPANSIONS;
        unsigned int NUM_POPULATION;
        unsigned int NUM_GENERATION;
        int WAIT_TIME;
        int EVAL_TIME;
        bool WRITE_TO_DEBUG;
        bool PRINT_TO_SCREEN;
        bool DEBUG_NODE_POSITIONS;
        bool DEBUG_PLACEMENT;
        bool STEP_SIMULATION;
        bool TEXTURES;

		void initialize(int NUM_POPULATION);
		void evaluate(int NUM_EXPANSIONS, bool STEP_SIMULATION, bool TEXTURES, unsigned int generation);
		void diversify();
		void select(FileHandler* fh);
		void breed();
		void print();

		void fillRoulette(std::vector<int>* roulette);


};


/****************************************/
/*      Local Function Objects          */
/****************************************/
class GrammarComparator{
    public:
        bool operator()(const Grammar& g1, const Grammar& g2){
            return g1.getFitness() > g2.getFitness();
        }
};

#endif

