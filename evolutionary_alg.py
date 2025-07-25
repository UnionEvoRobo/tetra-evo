"""
Run an evolutionary algorithm to generate a tetrahedral mesh.

By Thomas Breimer
July 14th, 2025
"""

import random
import time
import datetime as dt
from datetime import datetime
from pathlib import Path
import os
import json
import pandas as pd
import numpy as np
from grammar import Grammar
from tetrahedral_mesh import TetrahedralMesh

ALPHABET = ["A", "B", "C", "D", "E", "F", "G"]
OPERATIONS = {"relabel": 1, "grow": 3, "divide": 4} # Possible operations with number of rhs labels.

POINT = [25, 0, 25]

GENOME_INDEX = 0
FITNESS_INDEX = 1

class EvolutionRun:
    """
    One run of an evolutionary algorithm to generate a tetrahedral mesh.
    """

    def __init__(self, generations: int, population_size: int, num_elites: int, iters_per_run: int, mutuation_rate: float, 
                 crossover_rate: float, crossover_strategy: str, fitness_function: str, sort_reverse: bool, check_collision: bool):
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
            fitness_function (str): What fitness function to use.
            sort_reverse (bool): Whether to sort solutions in descending order. Should be True if higher fitness is better.
            check_collision (bool): Whether the mesh should block grow commands that overlap with the mesh.
        """

        self.generations = generations
        self.population_size = population_size
        self.num_elites = num_elites
        self.iters_per_run = iters_per_run
        self.mutation_rate = mutuation_rate
        self.crossover_rate = crossover_rate
        self.crossover_strategy = crossover_strategy
        self.fitness_function = fitness_function
        self.sort_reverse = sort_reverse
        self.check_collision = check_collision

        self.best_fitness = []

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

        self.export_info()

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
            self.best_fitness.append(self.population[0][FITNESS_INDEX])

            del self.population[-(self.new_per_gen):] # Delete all but the elites

            # Compute crossover selection rates
            fitnesses = [sublist[FITNESS_INDEX] for sublist in self.population]
            sum_fitness = sum(fitnesses)
            selection_probs = [fitness/sum_fitness for fitness in fitnesses]

            new_individuals = []

            # Fill rest of population !FIXME num_elites must be even or population will grow 
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
            best_mesh = TetrahedralMesh(self.population[0][GENOME_INDEX], self.check_collision)
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

        self.export_fitness_log()

    def export_fitness_log(self):
        """
        Exports the fitnesses.csv file of the best fitness per generation.
        """

        df = pd.DataFrame({
            "gen": range(len(self.best_fitness)),
            "best_fitness": self.best_fitness
        })

        current_file_path = Path(__file__).resolve().parent
        filepath = os.path.join(current_file_path, "meshes", self.start_time, "fitnesses.csv")

        df.to_csv(filepath, index=False)

    def export_info(self):
        """
        Exports info.json.
        """

        info = {
            "time": self.start_time,
            "generations": self.generations,
            "population_size": self.population_size, 
            "num_elites": self.num_elites,
            "iters_per_run": self.iters_per_run,
            "mutation_rate": self.mutation_rate,
            "crossover_rate": self.crossover_rate,
            "crossover_strategy": self.crossover_strategy,
            "fitness_function": self.fitness_function,
            "sort_reverse": self.sort_reverse,
            "check_collsion": self.check_collision
        }

        current_file_path = Path(__file__).resolve().parent

        directory_path = Path(current_file_path, "meshes", self.start_time,)
        directory_path.mkdir(parents=True, exist_ok=True)

        filepath = os.path.join(directory_path, "info.json")

        with open(filepath, 'w') as f:
            json.dump(info, f, indent=4)

    def get_fitness(self, genome: Grammar) -> float:
        """
        Get fitness of a genome.

        Parameters:
            genome (Grammar): Grammar object to get fitness of.

        Return:
            float: The fitness of the grammar, in this case the volume of the resulting tetra.
        """

        mesh = TetrahedralMesh(genome, self.check_collision)

        for i in range(self.iters_per_run):
            mesh.apply_rule()

        match self.fitness_function:
            case "dist_to_point":
                return mesh.dist_to_point(POINT)
            case "out_there_score":
                return mesh.out_there_score()
            case "num_faces":
                return mesh.get_num_faces()
            case "hull_volume":
                return mesh.get_hull()
            case _:
                raise ValueError("Unknown fitness function {}.".format(self.fitness_function))

    def sort_population(self):
        """
        Sorts the population.
        """

        self.population.sort(key = lambda x: x[FITNESS_INDEX], reverse=self.sort_reverse)

if __name__ == "__main__":
    my_run = EvolutionRun(generations=50, 
                        population_size=50, 
                        num_elites=20,
                        iters_per_run=250, 
                        mutuation_rate=0.2, 
                        crossover_rate=0.5,
                        crossover_strategy="one",
                        fitness_function="hull_volume",
                        sort_reverse=True,
                        check_collision=True)
    my_run.run()

