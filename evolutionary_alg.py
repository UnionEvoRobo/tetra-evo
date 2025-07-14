"""
Run an evolutionary algorithm to generate a tetrahedral mesh.

By Thomas Breimer
July 14th, 2025
"""

import random
from grammar import Grammar

ALPHABET = ["A", "B", "C", "D", "E", "F", "G"]
OPERATIONS = {"rename": 1, "grow": 3, "divide": 4} # Possible operations with number of rhs labels.

class EvolutionRun:
    """
    One run of an evolutionary algorithm to generate a tetrahedral mesh.
    """

    def __init__(self, generations: int, population_size: int, iters_per_run: int, mutuation_rate: float, crossover_rate: float, alphabet: list):
        """
        Returns an EvolutionRun instance.

        Parameters:
            generations (int): Number of generations to run.
            population_size (int): Number of individuals in the population.
            iters_per_run (int): Number of grammar rules to apply per individual.
            mutation_rate (float): Rate at which to mutate.
            crossover_rate (float): Rate at which to crossover.
            alphabet (list[str]): List of face labels to use.
        """

        self.generations = generations
        self.population_size = population_size
        self.iter_per_run = iters_per_run
        self.mutation_rate = mutuation_rate
        self.crossover_rate = crossover_rate
        self.alphabet = alphabet

        # Initialize random population, a list of (genome, fitness) pairs
        self.population: list[Grammar, float] = []
        for i in range(self.population_size):
            self.population.append((Grammar(ALPHABET, OPERATIONS).generate_random(), None))
            

my_run = EvolutionRun(20, 1, 2000, 0.1, 0.5, ALPHABET)




