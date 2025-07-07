#include <cstdlib>  //strtol
#include <cstdio>   //printf, FILE, fopen, fclose, fprintf, sprintf

#include "core/file_handler.hpp"
#include "core/genetic_algorithm.hpp"
#include "physics/simulation.hpp"

#ifdef GRAPHICS
#include "graphics/graphics.hpp"
#endif


int main(int argc, char** argv){
    
    //declare defaults
    time_t TIME = time(NULL);
    FileHandler fh;
    unsigned int NUM_EXPANSIONS = 25;
    unsigned int NUM_POPULATION = 10;
    unsigned int NUM_GENERATION = 100;
    int WAIT_TIME = 20;
    int EVAL_TIME = 20;

    bool WRITE_TO_DEBUG;
    bool PRINT_TO_SCREEN;
    bool DEBUG_NODE_POSITIONS;
    bool DEBUG_PLACEMENT;
    bool STEP_SIMULATION;
    bool TEXTURES;

    char configPath[20] = "./data/test.config";

    //Load config file
    fh.readConfig(configPath, &NUM_POPULATION, &NUM_GENERATION, &NUM_EXPANSIONS, &WAIT_TIME, &EVAL_TIME, &WRITE_TO_DEBUG, &PRINT_TO_SCREEN, &DEBUG_NODE_POSITIONS, &DEBUG_PLACEMENT, &STEP_SIMULATION, &TEXTURES, false); 

    //Parse command line arguments
    int i = 1;
    while (i < argc) {
        char* arg = argv[i];

        if(arg[0] == '-') {
            switch(arg[1]) {
                case 'r':
                    TIME = (int) strtol(argv[i+1], NULL, 10);
                    break;
                case 'e':
                    NUM_EXPANSIONS = (int) strtol(argv[i+1], NULL, 10);
                    break;
                case 'p':
                    NUM_POPULATION = (int) strtol(argv[i+1], NULL, 10);
                    break;
                case 'g':
                    NUM_GENERATION = (int) strtol(argv[i+1], NULL, 10);
                    break;
                case 'h': 
                    printf("Usage: ./bulletsoftbot [argument] [value] [argument] [value] ...\n\n"); 
                    printf("Arguments:\tEffect:\n");
                    printf("    -r    \t  set the random seed\n");
                    printf("    -e    \t  set num expansions\n");
                    printf("    -p    \t  set num population\n");
                    printf("    -g    \t  set num generation\n");
                    printf("    -h    \t  help\n");
                    printf("    -s    \t  run sim from rule set\n");
                    // printf("    -b    \t  run sim of best rule from file\n");
                    // printf("    -m    \t  run movie of evo of best rule from file\n");
                    exit(EXIT_SUCCESS);
				case 's': //rules in this format: icaccgbdbdgbadagcbcdrdccb
				{
					char* rules = argv[i+1];
                    Grammar grammar = Grammar(rules);
                    Simulation* mySim = new Simulation(&fh, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);
                    #ifdef GRAPHICS
                        Graphics myGraphics = Graphics(mySim, STEP_SIMULATION, TEXTURES);
                        char title[150];
                        sprintf(title, "Bullet Soft Bot: %s", grammar.toString());
                        myGraphics.setTitle(title);
                        genetic_algorithm ga = genetic_algorithm(&myGraphics, mySim, TIME, &fh, NUM_EXPANSIONS, NUM_POPULATION, NUM_GENERATION, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);
                    #else
                        genetic_algorithm ga = genetic_algorithm(mySim, TIME, &fh, NUM_EXPANSIONS, NUM_POPULATION, NUM_GENERATION, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);
                    #endif
                    ga.runGraphicsSim(&grammar, NUM_EXPANSIONS);
					exit(EXIT_SUCCESS); 
				} 
				
                // case 'b': //example: ./data/test.best
				// {
				// 	char* filePath = argv[++i];
				// 	printf("got file: %s\n", filePath);
				// 	genetic_algorithm::readFile(filePath, false, true, true, NUM_EXPANSIONS, NUM_POPULATION, NUM_GENERATION, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);
				// 	exit(EXIT_SUCCESS);
				// }
				// case 'm': //example: ./data/test.best
				// {
				// 	char* filePath = argv[++i];
				// 	printf("got file: %s\n", filePath);
				// 	genetic_algorithm::readFile(filePath, true, false, false);
				// 	exit(EXIT_SUCCESS);
				// }					
                default:
                    printf("Error: Invalid Argument '-%c', use -h for help with arguments\n", arg[1]); 
                    exit(EXIT_FAILURE); 
            }
        } else {
            printf("Error: Improper flag format.  Expected flag to start with '-', but given: %c.\n", arg[0]);
            exit(EXIT_FAILURE);
        }
        i += 2;
    }

    printf("Random seed: %ld\n", TIME);
    printf("NUM_EXPANSIONS: %u\n", NUM_EXPANSIONS);
    printf("NUM_POPULATION: %u\n", NUM_POPULATION);
    printf("NUM_GENERATION: %u\n", NUM_GENERATION);
    printf("WAIT_TIME: %d\n", WAIT_TIME);
    printf("EVAL_TIME: %d\n", EVAL_TIME);
    
    Simulation* mySim = new Simulation(&fh, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);
    #ifdef GRAPHICS
        Graphics myGraphics = Graphics(mySim, STEP_SIMULATION, TEXTURES);
        genetic_algorithm ga = genetic_algorithm(&myGraphics, mySim, TIME, &fh, NUM_EXPANSIONS, NUM_POPULATION, NUM_GENERATION, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);
    #else
        genetic_algorithm ga = genetic_algorithm(mySim, TIME, &fh, NUM_EXPANSIONS, NUM_POPULATION, NUM_GENERATION, WAIT_TIME, EVAL_TIME, WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT, STEP_SIMULATION, TEXTURES);
    #endif
    ga.run();
    
    printf("DONE\n");
}

