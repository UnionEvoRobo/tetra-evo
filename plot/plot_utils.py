"""
Plotting functions.

Author: Thomas Breimer
July 15th, 2025
"""

import os
import argparse
from pathlib import Path
from matplotlib.cm import *
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.style as style
import numpy as np
import json


def plot_runs(paths, compute_evals=True):
    """
    Plot fitness over evaluations or generations given a list of run.csv's.

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
    plt.title("Fitness Progression Over " + x_label)
    plt.legend()
    plt.grid()
    plt.show()

def plot_batch(path, compute_evals=True):
    """
    Plot fitness over evaluations or generations given a batch of runs.

    Will plot mean, min or max, and 25-75 percentile shading.

    If an info.json file is in the same directory as the run.csv and the compute_evals flag is true, 
    number of evaluations is computed and plotted. Otherwise, the x-axis will be generations.
    The info.json file is also relied on to correctly plot either the min or max fitness based on the
    sort_reverse flag for the run. If sort_reverse is True, the max will be plotted, and otherwise
    the min is plotted. 

    Arguments:
    paths (str): A path-like string to a batch folder. run0, run1, run2, ... directories should exist as subfolders.
    compute_evals (bool): Whether to compute and plot evaluations instead of generations.
    """

    avg_color = '#ff871d'  # Orange for average best fitness
    best_color = '#4fafd9'  # Blue for best fitness per generation
    shade_color = '#ffb266'

    batch_path = Path(path).resolve()

    if not batch_path.exists() or not batch_path.is_dir():
        ValueError(f"Path '{batch_path}' does not exist or is not a directory.")
        return

    run_folders = list(batch_path.glob("run*"))
    num_files = len(run_folders)

    all_data = []

    for run_path in run_folders:
        run_path = Path(run_path)
        run_csv_path = Path(os.path.join(run_path, "run.csv"))
        info_path = Path(os.path.join(run_path, "info.json"))
        
        # Get data from info.json
        try:
            with open(info_path, 'r') as file:
                info = json.load(file)
                sort_reverse = info["sort_reverse"]
                if compute_evals: # Get population
                    population_size = int(info["population_size"])
                else:
                    population_size = 1
        except:
            print("Expected to find", info_path, " but it was not found.")

        try:
            df = pd.read_csv(run_csv_path)
            if 'generation' in df.columns and 'fitness' in df.columns:
                df = df[['generation', 'fitness']]
                df["generation"] = df["generation"] * population_size
                df['run'] = run_csv_path.name
                all_data.append(df)
        except Exception as e:
            print(f"Could not read {run_csv_path}: {e}")

    combined = pd.concat(all_data, ignore_index=True)

    stats = combined.groupby('generation')['fitness'].agg([
        'mean', 'min', 'max', lambda x: x.quantile(0.25), lambda x: x.quantile(0.75)
    ]).rename(columns={
        '<lambda_0>': 'q25',
        '<lambda_1>': 'q75'
    }).reset_index()

    plt.plot(stats['generation'], stats['mean'], marker='o', label="Mean Best Fitness", linewidth=2, color=avg_color)
    if sort_reverse:
        plt.plot(stats['generation'], stats['max'], marker='o', label="Overall Best Fitness", color=best_color)
    else:
        plt.plot(stats['generation'], stats['min'], marker='o', label="Overall Best Fitness", color=best_color)
    plt.fill_between(stats['generation'], stats['q25'], stats['q75'], alpha=0.2, label='25-75th Percentile', color=shade_color)

    if compute_evals:
        plt.xlabel('Evaluations')
    else:
        plt.xlabel('Generation')

    plt.ylabel('Best Fitness So Far')
    title = (f'Fitness Progression Across {num_files} Runs')
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()
