"""
Run an evolutionary algorithm to generate a tetrahedral mesh.

By Thomas Breimer
July 14th, 2025
"""

import random
import time
import datetime as dt
from datetime import datetime
import numpy as np
from grammar import Grammar
from tetrahedral_mesh import TetrahedralMesh

ALPHABET = ["A", "B", "C", "D", "E", "F", "G"]
OPERATIONS = {"relabel": 1, "grow": 3, "divide": 4} # Possible operations with number of rhs labels.

GENOME_INDEX = 0
FITNESS_INDEX = 1

class EvolutionRun:
    """
    One run of an evolutionary algorithm to generate a tetrahedral mesh.
    """

    def __init__(self, generations: int, population_size: int, num_elites: int, iters_per_run: int, mutuation_rate: float, 
                 crossover_rate: float, crossover_strategy: str):
        """
        Returns an EvolutionRun instance.

        Parameters:
            generations (int): Number of generations to run.
            population_size (int): Number of individuals in the population.
            num_elites (int): Number of elites to keep over a generation.
            iters_per_run (int): Number of grammar rules to apply per individual.
            mutation_rate (float): Rate at which to mutate.
            crossover_rate (float): Rate at which to crossover.
            crossover_strategy (str): "one" or "two" for single or double point crossover, "uniform" for uniform crossover. 
        """

        self.generations = generations
        self.population_size = population_size
        self.num_elites = num_elites
        self.iters_per_run = iters_per_run
        self.mutation_rate = mutuation_rate
        self.crossover_rate = crossover_rate
        self.crossover_strategy = crossover_strategy

        self.new_per_gen = self.population_size - self.num_elites # Num of new individuals per gen

        self.current_gen = 0
        self.start_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

        # Initialize random population, a list of (genome, fitness) pairs
        self.population: list[Grammar, float] = []
        for i in range(self.population_size):
            self.population.append([Grammar(ALPHABET, OPERATIONS).generate_random(), None])

    def run(self):
        """
        Run the evolutionary algorithm.
        """

        # Compute time
        self.last_gen_clock = time.time()

        while self.current_gen < self.generations:

            # Get fitnesses
            for i, individual in enumerate(self.population):
                genome = individual[GENOME_INDEX]
                self.population[i][FITNESS_INDEX] = self.get_fitness(genome) 
            
            self.sort_population() # Sort population

            # Print fitnesses
            print("")
            print("Generation {}".format(self.current_gen))
            print("Fitnesses:", [sublist[FITNESS_INDEX] for sublist in self.population])

            del self.population[-(self.new_per_gen):] # Delete all but the elites

            # Compute crossover selection rates
            fitnesses = [sublist[FITNESS_INDEX] for sublist in self.population]
            sum_fitness = sum(fitnesses)
            selection_probs = [fitness/sum_fitness for fitness in fitnesses]

            new_individuals = []

            # Fill rest of population
            while len(new_individuals) < self.new_per_gen:
                # Pick one parent
                p1 = self.population[np.random.choice(len(self.population), p=selection_probs)][GENOME_INDEX].copy()
                p2 = p1

                # Pick a different parent
                while p1 is p2:
                    p2 = self.population[np.random.choice(len(self.population), p=selection_probs)][GENOME_INDEX].copy()

                # Choose selected crossover strategy
                if random.random() < self.crossover_rate:
                    match self.crossover_strategy:
                        case "one":
                            p1.crossover(p2)
                        case "two":
                            p1.two_point_crossover(p2)
                        case "uniform":
                            p1.uniform_crossover(p2)
                        case _:
                            raise ValueError('Unexpected crossover strategy {}. Try "one", "two", or "uniform"'.
                                            format(self.crossover_strategy))
                
                # Mutate
                p1.regenerate_random(self.mutation_rate)
                p2.regenerate_random(self.mutation_rate)

                new_individuals.append([p1, None])
                new_individuals.append([p2, None])
        
            self.population.extend(new_individuals)
            self.current_gen += 1

            # Print grammar
            print(self.population[0][GENOME_INDEX])

            # Export .stl
            best_mesh = TetrahedralMesh(self.population[0][GENOME_INDEX])
            for i in range(self.iters_per_run):
                best_mesh.apply_rule()
            best_mesh.export_to_stl("gen{}_score{}".format(str(self.current_gen).zfill(4), self.population[0][FITNESS_INDEX]),
                                    self.start_time)

            # Time estimate
            last_gen_time = time.time() - self.last_gen_clock
            self.last_gen_clock = time.time()
            gens_remaining = self.generations - self.current_gen
            print("Last gen time: {} | Gens remaining: {} | Time est: {}".
                  format(dt.timedelta(seconds=int(last_gen_time)), gens_remaining, dt.timedelta(seconds=int(last_gen_time* gens_remaining))))

    def get_fitness(self, genome: Grammar) -> float:
        """
        Get fitness of a genome.

        Parameters:
            genome (Grammar): Grammar object to get fitness of.

        Return:
            float: The fitness of the grammar, in this case the volume of the resulting tetra.
        """

        mesh = TetrahedralMesh(genome)

        for i in range(self.iters_per_run):
            mesh.apply_rule()

        score = mesh.get_num_faces()

        return score

    def sort_population(self):
        """
        Sorts the population by decreasing fitness.
        """

        self.population.sort(key = lambda x: x[FITNESS_INDEX], reverse=True)

my_run = EvolutionRun(generations=100, 
                      population_size=50, 
                      num_elites=24, 
                      iters_per_run=200, 
                      mutuation_rate=0.1, 
                      crossover_rate=0.5,
                      crossover_strategy="one")

my_run.run()

