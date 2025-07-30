"""
Grow and display and save a mesh using a Grammar from a saved .csv file.

Author: Thomas Breimer
July 28th, 2025
"""

import os
from pathlib import Path
import pandas as pd
import trimesh
from model.grammar import Grammar
from model.tetrahedral_mesh import TetrahedralMesh

FILEPATH = "runs/2025-07-28_14-09-23/gen0.csv"
ID = 0 # genome id in csv file or generation if looking at a run.csv file
SHOW_MESH = True # Whether to display the mesh after it is saved.

EXPORT_FILEPATH = "meshes" # Export filepath
EXPORT_FILENAME = "grow_mesh_test" # Export filename

ITERS = 10
CHECK_COLLISION = True

MY_PATH = Path(__file__).resolve().parent

def read_csv(filepath: Path, id: int) -> Grammar:
    """
    Reads a .csv file containing Grammars and returns a Grammar object.

    Parameters:
        filepath (Path): Path of the .csv file.
        id (int): The id or generation number of the grammar to read.

    Returns:
        Grammar: The Grammar object specified.
    """

    df = pd.read_csv(os.path.join(MY_PATH, filepath))

    if "id" in df.columns:
        row = df[df['id'] == id]
    elif "generation" in df.columns:
        row = df[df['generation'] == id]
    else:
        raise ValueError("Expected .csv file {} to have an 'id' or 'generations' column, but it didn't have either!".format(filepath))
    
    grammar = Grammar()
    grammar.add_from_dict(row.iloc[0].to_dict())

    return grammar

def apply_rules(grammar: Grammar, iters: int, check_collision: bool) -> TetrahedralMesh:
    """
    Builds a mesh by applying rules from a grammar.

    Parameters:
        grammar (Grammar): A Grammar object to follow to generate the mesh.
        iters (int): The number of Grammar productions to perform.

    Returns:
        TetrahedralMesh: A grown tetra mesh.
    """

    mesh = TetrahedralMesh(grammar, check_collision)

    for i in range(iters):
        mesh.apply_rule()

    return mesh

def grow_mesh():
    """
    Grow a mesh.
    """

    grammar = read_csv(filepath=FILEPATH, id=ID)
    mesh = apply_rules(grammar, ITERS, CHECK_COLLISION)

    mesh.export_to_stl(EXPORT_FILENAME, os.path.join(MY_PATH, EXPORT_FILEPATH))

    if SHOW_MESH:
        trimesh.load_mesh(os.path.join(MY_PATH, EXPORT_FILEPATH, EXPORT_FILENAME + ".stl")).show()

if __name__ == "__main__":
    grow_mesh()



