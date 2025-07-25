"""
Plot fitness over generations.

Author: Thomas Breimer
July 15th, 2025
"""

import os
import argparse
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

DIR = "meshes"
TIMESTAMP = "2025-07-24_14-47-33"

def plot(dir=DIR, timestamp=TIMESTAMP):
    """
    Plot fitness over generations given an output.csv.
    """

    current_file_path = Path(__file__).resolve().parent
    path = os.path.join(current_file_path, dir, timestamp, "fitnesses.csv")

    df = pd.read_csv(path)

    plt.figure(figsize=(8, 5))
    plt.plot(np.array(df["gen"]),
            np.array(df["best_fitness"]),
            marker='o',
            linestyle='-',
            color='b',
            label="Best Fitness")
    plt.xlabel("Generation")
    plt.ylabel("Best Fitness")
    plt.title("Fitness Evolution Over Generations")
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == "__main__":
    plot()