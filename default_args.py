"""
Default arguments for an evolution run.
"""

# Population attributes
GENERATIONS: int = 50
POPULATION_SIZE: int = 5
NUM_ELITES: int = 3

# Population generation
MUTATION_RATE: float = 0.2
CROSSOVER_RATE: float = 0.5
CROSSOVER_STRATEGY: str = "two" # Options: "one", "two", "uniform"

# Mesh & Grammar settings
ITERS_PER_RUN: int = 100
CHECK_COLLISION: bool = True
ALPHABET: list[str] = ["A", "B", "C", "D", "E", "F", "G"]

# Fitness settings
FITNESS_FUNCTION: str = "hull_volume" # Options: "dist_to_point", "out_there_score", "num_faces", "hull_volume"
SORT_REVERSE: bool = False
POINT: list[float] = [25, 0, 25] # Point for mesh to grow toward for "dist_to_point" fitness benchmark. Should be length 3.

# Export settings
EXPORT_GENERATIONS: bool = True
EXPORT_STL: bool = True
DATA_PATH: bool = None # Expects path-like string, defaults to /runs when None
RUN_NAME: str = None # Defaults to current timestamp when None

# Run batch settings
RUNS: int = 1
BATCH_PATH: str = None # Expects path-like string, defaults to /batches when None
BATCH_NAME: str = None # Defaults to current timestamp when None
