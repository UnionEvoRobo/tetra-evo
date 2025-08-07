"""
Plotting functions.

Author: Thomas Breimer
July 15th, 2025
"""

import os
import argparse
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.style as style
import numpy as np
import json


def plot_runs(paths, compute_evals=True):
    """
    Plot fitness over evaluations or generations given an list of run.csv's.

    If an info.json file is in the same directory as the run.csv and the compute_evals flag is true, 
    number of evaluations is computed and plotted. Otherwise, the x-axis will be generations.

    Arguments:
    paths (list[str]): A list of path-like strings to run.csv files to plot. Include multiple to compare between runs.
    compute_evals (bool): Whether to compute and plot evaluations instead of generations.
    """

    plt.figure(figsize=(8, 5))
    plt.ylabel("Best Fitness")
    style.use('tableau-colorblind10')

    for path in paths:
        path = Path(path).resolve()
        df = pd.read_csv(path)
        
        if compute_evals: # Get population
            info_path = os.path.join(path.parent, "info.json")
            try:
                with open(info_path, 'r') as file:
                    info = json.load(file)
                    population_size = int(info["population_size"])
            except:
                print("Expected to find", info_path, " but it was not found.")
        else:
            population_size = 1

        parent_path = os.path.dirname(path)
        parent_name = os.path.basename(parent_path)

        plt.plot(np.array(df["generation"]*population_size),
                np.array(df["fitness"]),
                marker='o',
                linestyle='-',
                label=str(parent_name))

    if compute_evals:
        x_label = "Evaluations"
    else:
        x_label = "Generations"
        
    plt.xlabel(x_label)
    plt.title("Fitness Evolution Over " + x_label)
    plt.legend()
    plt.grid()
    plt.show()

csv_paths: str = ["latest_run", "/home/tbreimer/evofab/tetra-evo/runs/2025-07-30_13-08-52/run.csv"] # Use "latest_run" or path-like string to a run.csv file.
plot_runs(csv_paths, True)