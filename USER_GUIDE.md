# USER_GUIDE.md

This guide provides instructions for how to use the framework.

## evolutionary_alg.py

### Basic Usage
An evolutionary algorithm run can be initiated using the command `python evolutionary_alg.py`. By default, the results are saved in the `/runs` folder under a timestamped folder. Inside, you’ll find a `run.csv` file with the best fitness score for each generation along with a serialized representation of the grammar which produced that fitness score. An `info.json` file is also included so you can remember which arguments which were used.

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
* Whether to export the best mesh for each generation as an .stl file. Options: 't', 'f'.
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

This script can run grammars saved in .csv files. Works for both `run.csv`and `genX.csv` files generated from `evolutionary_alg.py`. Example usage: `python grow_mesh.py —filename path/to/my/run.csv —id 20`

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

## plot.ipynb

A notebook for graphing runs and batches. Docs inside.


