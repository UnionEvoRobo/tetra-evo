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


def single_run(path):
    """
    Plot fitness over generations given an output.csv.
    """
    df = pd.read_csv(path)

    plt.figure(figsize=(8, 5))
    plt.plot(np.array(df["generation"]),
            np.array(df["fitness"]),
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