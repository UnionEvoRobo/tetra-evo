"""
Run an evolutionary algorithm to generate a tetrahedral mesh.

By Thomas Breimer
July 14th, 2025
"""

import random
import sys
import time
import datetime as dt
from datetime import datetime
from pathlib import Path
import os
import json
import argparse
import pandas as pd
import numpy as np
from model.grammar import Grammar
from model.tetrahedral_mesh import TetrahedralMesh, OPERATIONS
import default_args as D

GENOME_INDEX = 0
FITNESS_INDEX = 1

def is_windows():
    """
    Checks if the operating system is Windows.

    Returns:
        bool: True if the OS is Windows, False otherwise.
    """
    return os.name == 'nt' or sys.platform.startswith('win')

def set_symlink(symlink_path: str, target_path: str):
    """
    Set a symlink.

    Parameters:
        symlink (str): Path of the symlink.
        target_path (str): Path of the target.
    """

    try:
        if os.path.islink(symlink_path) or os.path.exists(symlink_path):
            os.remove(symlink_path)

        if is_windows():
            os.symlink(target_path, symlink_path)
        else:
            os.system(f'ln -s "{target_path}" "{symlink_path}"')

    except Exception as e:
        print(
            f"Warning: could not create symlink to latest_genome folder: {e}"
        )

class EvolutionRun:
    """
    One run of an evolutionary algorithm to generate a tetrahedral mesh.
    """

    def __init__(self, generations: int, population_size: int, num_elites: int, iters_per_run: int, mutuation_rate: float, 
                 crossover_rate: float, crossover_strategy: str, fitness_function: str, sort_reverse: bool, check_collision: bool,
                 export_generations: bool, export_stl: bool, export_extension: str, alphabet: list[str], run_name: str = None,
                 data_path: str = None):
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
            export_generations (bool): Whether to export a .csv file representing each generation.
            export_stl (bool): Whether to export the best individual of every generation as an .stl file.
            export_extension (str): What file extension to use when exporting meshes. Supports .stl and .obj.
            alphabet (list[str]): Possible labels for faces.
            run_name (str): Folder name to save run data under. Will save as timestamp otherwise.
            data_dir (str): Path to store run data in. Expects path-like string, defaults to /runs.
        """

        # Args
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
        self.export_generations = export_generations
        self.export_stl = export_stl
        self.export_extension = export_extension
        self.alphabet = alphabet
        self.run_name = run_name

        # Book-keeping
        self.best_fitness = []
        self.best_individuals = []

        # Setup
        self.this_dir = Path(Path(__file__).resolve().parent)
        self.new_per_gen = self.population_size - self.num_elites # Num of new individuals per gen
        self.current_gen = 0
        self.start_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
        self.data_path = self.resolve_data_path(run_name, data_path)
        self.population: list[Grammar, float] = []
        self.export_info()
        self.export_run()
        set_symlink(os.path.join(self.this_dir, "latest_run"), os.path.join(self.data_path, "run.csv"))

    def resolve_data_path(self, run_name: str, data_dir: str) -> Path:
        """
        Resolves and creates directory run data will be stored in.

        Parameters:
            run_name (str): Name of the run, to be the top-level folder name.
            data_dir (str): Place to put the top-level data folder.

        Returns:
            Path: The new-ly created directory where data should be stored.
        """

        if data_dir is None or data_dir == "None":
            data_path = os.path.join(self.this_dir, "runs")
        else:
            data_path = data_dir

        if run_name is None or data_dir == "None":
            data_path = os.path.join(data_path, self.start_time)
        else:
            data_path = os.path.join(data_path, self.run_name)

        Path(data_path).mkdir(parents=True, exist_ok=True)

        return data_path

    def run(self):
        """
        Run the evolutionary algorithm.
        """

        self.last_gen_clock = time.time()

        # Initialize random population, a list of (genome, fitness) pairs
        for i in range(self.population_size):
            self.population.append([Grammar(self.alphabet, OPERATIONS).generate_random(), None])

        while self.current_gen < self.generations:

            ### Get & sort fitnesses

            for i, individual in enumerate(self.population):
                genome = individual[GENOME_INDEX]
                self.population[i][FITNESS_INDEX] = self.get_fitness(genome) 
            
            self.sort_population() # Sort population

            ### Housekeeping

            # Export generation and extract best individual
            if self.export_generations:
                self.export_current_population()

            self.best_fitness.append(self.population[0][FITNESS_INDEX])
            self.best_individuals.append(self.population[0][GENOME_INDEX].to_dict())

            # Print fitnesses
            print("")
            print("Generation {}".format(self.current_gen))
            print("Fitnesses:", [sublist[FITNESS_INDEX] for sublist in self.population])

            # Print grammar
            print(self.population[0][GENOME_INDEX])

            # Export best mesh
            if self.export_stl:
                best_mesh = TetrahedralMesh(self.population[0][GENOME_INDEX], self.check_collision)
                for i in range(self.iters_per_run):
                    best_mesh.apply_rule()
                best_mesh.export(self.export_extension, "gen{}_score{}".format(str(self.current_gen), 
                                    self.population[0][FITNESS_INDEX]), self.data_path)

            ### Make next generation

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
                
                if len(new_individuals) < self.new_per_gen - 1: # If we need more than 1 new individual, pick another and maybe crossover
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
                    
                    p2.regenerate_random(self.mutation_rate) # Mutate
                    new_individuals.append([p2, None])
                
                p1.regenerate_random(self.mutation_rate)
                new_individuals.append([p1, None])
        
            self.population.extend(new_individuals)
            self.current_gen += 1

            ### Time estimate

            last_gen_time = time.time() - self.last_gen_clock
            self.last_gen_clock = time.time()
            gens_remaining = self.generations - self.current_gen
            print("Last gen time: {} | Gens remaining: {} | Time est: {}".
                  format(dt.timedelta(seconds=int(last_gen_time)), gens_remaining, dt.timedelta(seconds=int(last_gen_time* gens_remaining))))

            self.export_run()

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
                return mesh.dist_to_point(D.POINT)
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

    def export_current_population(self):
        """
        Exports current self.population as a .csv file.
        """

        # Init columns id, fitness, num_rules, lhs0, operation0, rhs0, lhs1, ...
        columns = ["id", "fitness", "num_rules"]
        columns += [item + str(i) for i in range(len(self.alphabet)) for item in ['lhs', 'operation', 'rhs']]
        rows = []

        for id, individual in enumerate(self.population):
            fitness = individual[FITNESS_INDEX]
            grammar = individual[GENOME_INDEX]
            row = {"id": id, "fitness": fitness, "num_rules": len(self.alphabet)}
            row.update(grammar.to_dict())
            rows.append(row)

        file_path = os.path.join(self.data_path, "gen" + str(self.current_gen) + ".csv")

        pd.DataFrame(rows, columns=columns).to_csv(file_path, index=False)
    
    def export_run(self):
        """
        Exports the run.csv file of the best fitness and grammars per generation.
        """

        columns = ["generation", "fitness", "num_rules"]
        columns += [item + str(i) for i in range(len(self.alphabet)) for item in ['lhs', 'operation', 'rhs']]
        rows = []

        num_rules = len(self.alphabet)

        for generation, individual in enumerate(self.best_individuals):
            row = {"generation": generation, "fitness": self.best_fitness[generation], "num_rules": num_rules}
            row.update(individual)
            rows.append(row)

        file_path = os.path.join(self.data_path, "run.csv")

        pd.DataFrame(rows, columns=columns).to_csv(file_path, index=False)

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
            "check_collsion": self.check_collision,
            "alphabet": self.alphabet
        }

        filepath = os.path.join(self.data_path, "info.json")

        with open(filepath, 'w') as f:
            json.dump(info, f, indent=4)
        

def run_batch(runs: int, batch_name: str = None, batch_path: str = None):
    """
    Runs a batch of EvolutionRun Experiments.

    Parameters:
        runs (int): Number of runs to perform.
        batch_name (str): Name of run. Defaults to timestamp.
        batch_path (str): Where to save run data. Defaults to /batches/.
    """

    start_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

    if batch_path is None or batch_path == "None":
        this_dir = Path(Path(__file__).resolve().parent)
        data_path = os.path.join(this_dir, "batches")
    else:
        data_path = batch_path

    if batch_name is None or batch_name == "None":
        data_path = os.path.join(data_path, start_time)
    else:
        data_path = os.path.join(data_path, batch_name)

    Path(data_path).mkdir(parents=True, exist_ok=True)

    for i in range(runs):
        run_name = "run" + str(i)
        print("RUN {} ---------------------------------------".format(i))
        my_run = EvolutionRun(generations=D.GENERATIONS, 
                            population_size=D.POPULATION_SIZE, 
                            num_elites=D.NUM_ELITES,
                            iters_per_run=D.ITERS_PER_RUN, 
                            mutuation_rate=D.MUTATION_RATE, 
                            crossover_rate=D.CROSSOVER_RATE,
                            crossover_strategy=D.CROSSOVER_STRATEGY,
                            fitness_function=D.FITNESS_FUNCTION,
                            sort_reverse=D.SORT_REVERSE,
                            check_collision=D.CHECK_COLLISION,
                            export_generations=D.EXPORT_GENERATIONS,
                            export_stl=D.EXPORT_STL,
                            alphabet=D.ALPHABET,
                            run_name=run_name,
                            data_path=data_path)
        my_run.run()
        del my_run

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='RL')
    parser.add_argument('--runs',
                        type=int,
                        help='number of runs to do',
                        default=D.RUNS)
    parser.add_argument('--generations',
                        type=int,
                        help='number of generations to run',
                        default=D.GENERATIONS)
    parser.add_argument('--population_size',
                        type=int,
                        help='number of individuals in each generations',
                        default=D.POPULATION_SIZE)
    parser.add_argument('--num_elites',
                        type=int,
                        help='number of individuals to keep around for the next generation',
                        default=D.NUM_ELITES)
    parser.add_argument('--iters_per_run',
                        type=int,
                        help='number of production rules to follow when growing the mesh',
                        default=D.ITERS_PER_RUN)
    parser.add_argument('--mutation_rate',
                        type=float,
                        help='probability each production rule gets mutated',
                        default=D.MUTATION_RATE)
    parser.add_argument('--crossover_rate',
                        type=float,
                        help='probability new individuals created are crossed over',
                        default=D.CROSSOVER_RATE)
    parser.add_argument('--crossover_strategy',
                        type=str,
                        help='strategy to use for crossover, options: "one", "two", "uniform"',
                        default=D.CROSSOVER_STRATEGY)
    parser.add_argument('--fitness_function',
                        type=str,
                        help='fitness function, options: "dist_to_point", "out_there_score", "num_faces", "hull_volume"',
                        default=D.FITNESS_FUNCTION)
    parser.add_argument('--sort_reverse',
                        default=D.SORT_REVERSE,
                        type=str,
                        help="whether to sort fitnesses in reverse order ('t'/'f'')")
    parser.add_argument('--check_collision',
                        default=D.CHECK_COLLISION,
                        type=str,
                        help="whether to check for face collision when building the mesh ('t'/'f')")
    parser.add_argument('--export_generations',
                        default=D.EXPORT_GENERATIONS,
                        type=str,
                        help="whether to generate a .csv file for each generation ('t'/'f')")
    parser.add_argument('--export_stl',
                        default=D.EXPORT_STL,
                        type=str,
                        help="whether to export the best mesh for each generation as an .stl file ('t'/'f')")
    parser.add_argument('--export_extension',
                        default=D.EXPORT_EXTENSION,
                        type=str,
                        help='what file extension to save meshes as, supports ".stl" and ".obj"')
    parser.add_argument('--run_name',
                        type=str,
                        help='name of directory to store run data in',
                        default=D.RUN_NAME)
    parser.add_argument('--data_path',
                        type=str,
                        help='path to save run data in',
                        default=D.DATA_PATH)
    parser.add_argument('--batch_name',
                        type=str,
                        help='name of directory to save runs in',
                        default=D.BATCH_NAME)
    parser.add_argument('--batch_path',
                        type=str,
                        help='path to save runs data in',
                        default=D.BATCH_PATH)
    args = parser.parse_args()

    bool_map = {
        "true": True,
        "false": False,
        "True": True,
        "False": False,
        "1": True,
        "0": False,
        "t": True,
        "f": False,
        "T": True,
        "F": False,
        True: True,
        False: False
    }

    D.RUNS = int(args.runs)
    D.GENERATIONS = int(args.generations)
    D.POPULATION_SIZE = int(args.population_size)
    D.NUM_ELITES = int(args.num_elites)
    D.ITERS_PER_RUN = int(args.iters_per_run)
    D.MUTATION_RATE = float(args.mutation_rate)
    D.CROSSOVER_RATE = float(args.crossover_rate)
    D.CROSSOVER_STRATEGY = str(args.crossover_strategy)
    D.FITNESS_FUNCTION = str(args.fitness_function)
    D.SORT_REVERSE = bool_map[args.sort_reverse]
    D.CHECK_COLLISION = bool_map[args.check_collision]
    D.EXPORT_GENERATIONS = bool_map[args.export_generations]
    D.EXPORT_STL = bool_map[args.export_stl]
    D.EXPORT_EXTENSION = args.export_extension
    D.RUN_NAME = str(args.run_name)
    D.DATA_PATH = str(args.data_path)
    D.BATCH_PATH = str(args.batch_path)
    D.BATCH_NAME = str(args.batch_name)

    if D.RUNS > 1:
        run_batch(D.RUNS, D.BATCH_NAME, D.BATCH_PATH)
    elif D.RUNS == 1:
        my_run = EvolutionRun(generations=D.GENERATIONS, 
                            population_size=D.POPULATION_SIZE, 
                            num_elites=D.NUM_ELITES,
                            iters_per_run=D.ITERS_PER_RUN, 
                            mutuation_rate=D.MUTATION_RATE, 
                            crossover_rate=D.CROSSOVER_RATE,
                            crossover_strategy=D.CROSSOVER_STRATEGY,
                            fitness_function=D.FITNESS_FUNCTION,
                            sort_reverse=D.SORT_REVERSE,
                            check_collision=D.CHECK_COLLISION,
                            export_generations=D.EXPORT_GENERATIONS,
                            export_stl=D.EXPORT_STL,
                            export_extension=D.EXPORT_EXTENSION,
                            alphabet=D.ALPHABET,
                            run_name=D.RUN_NAME,
                            data_path=D.DATA_PATH)
        my_run.run()
    else:
        ValueError("Specified number of runs {} is invalid.".format(args.runs))