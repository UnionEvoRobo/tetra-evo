# USER_GUIDE.md

This guide provides instructions for how to use the framework.

## evolutionary_alg.py

### Basic Usage
An evolutionary algorithm run can be initiated using the command `python evolutionary_alg.py`. By default, the results are saved in the `/runs` folder under a timestamped folder. Inside, you’ll find a `run.csv` file with the best fitness score for each generation along with a serialized representation of the grammar which produced that fitness score. An `info.json` file is also included so you can remember which arguments were used.

### Arguments
Default arguments can be found and changed in `default_args.py`. Editing this file is the easiest way to edit run characteristics. However, command line arguments are also supported using the format`python evolutionary_algorithm.py —ARG_NAME ARG_VALUE`. A complete list of arguments can be found below.

--generations INT
* Number of generations for each run.

--population_size INT
* Number of individuals in a generation.

--num_elites INT
* Number of best-preforming individuals to keep around for the next generation.

--iters_per_run INT
* Number of production rules to follow when growing the mesh.

--mutation_rate FLOAT
* Probability each production rule gets mutated.

--crossover_rate FLOAT
* Probability new individuals created are the result of crossovers.

--crossover_strategy STR
* Strategy to use for crossover. Options: "one", "two", "uniform".

--fitness_function STR
* Fitness function used to score tetrahedral meshes. Options: "dist_to_point", "out_there_score", "num_faces", “hull_volume”.

--sort_reverse STR
* Whether to sort fitnesses in reverse order. Options: 't', 'f'.

--check_collision STR
* Whether to check and prevent face collision when building the mesh. Options: 't', 'f'.

--export_generations STR
* Whether to generate a .csv file for each generation. Each file will contain all grammars in the generation. Options: 't', 'f'.

--export_stl STR
* Whether to export the best mesh for each generation. Options: 't', 'f'.

--export_extension STR
* What file extension to use for mesh exports. Supports ".stl" and ".obj"

--run_name STR
* Name of directory to store run data in. Defaults to a timestamp.

--data_path STR
* Path to save run data in. Defaults to `/runs`.

--batch_name STR
* Name of directory to store batches in. Defaults to a timestamp.

--batch_path STR
* Path to store batches. Defaults to `/batches`.

--runs INT
* Number of runs to preform. If > 1, the run will be treated as a batch. By default, data from batched runs are stored in the `/batches` directory under a timestamped folder.

## grow_mesh.py

This script can run grammars saved in .csv files. Works for both `run.csv`and `genX.csv` files generated from `evolutionary_alg.py`. Example usage: `python grow_mesh.py —filepath path/to/my/run.csv —id 20`

### Arguments
--filepath STR
* Path of .csv file to read from.

--id INT
* ID of grammar to use in `run.csv` file or generation number if dealing with a `genX.csv` file.

--show_mesh STR
* Whether to display the mesh after it is saved. Options: 't', 'f'.

--export_filepath STR
* Filepath to store the mesh.

--export_filename STR
* Name to store the mesh as.

--iters INT
* Number of production rules to use to grow the mesh.

--check_collision STR
* Whether to check for collision.

--export_extension STR
* What file extension to use for export. Supports ".stl" and ".obj".

## plot.ipynb

A notebook for graphing runs and batches. Docs inside.

## Custom Fitness Function

The fitness function that is used during a run is passed into the `EvolutionRun()` constructor as a string. This class is defined in `evolutionary_alg.py`. When it's time to evaluate a grammar, `EvolutionRun.get_fitness()` is called. This method grows a mesh using the grammar and evaluates it using a match statement on the string `EvolutionRun.fitness_function` so that the correct fitness function is used.

The four provided fitness functions exist as methods associated with the generated `TetrahedralMesh` object. This class is defined in `model/tetrahedral_mesh.py`. Thus, if you wish to write a new fitness function, you may want to follow these steps.

1. Write a new `TetrahedralMesh` method which scores the TetrahedralMesh based on some attribute and returns a `float` or an `int`.
2. Choose some string identifier for this new fitness function and extend the `match` statement in `EvolutionRun.get_fitness()` to call your new fitness function when your new string identifier is passed into `EvolutionRun.fitness_function`.
3. Edit `FITNESS_FUNCTION` in `default_args.py` to use your new string identifier and consider whether the `SORT_REVERSE` flag should be set to true if a higher fitness is better.
