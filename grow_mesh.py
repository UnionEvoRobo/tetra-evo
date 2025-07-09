"""
Build a TetrahedralMesh by applying a Grammar.

By Thomas Breimer
July 9th, 2025
"""

from grammar import Grammar
from tetrahedral_mesh import TetrahedralMesh

ITERS = 1000
FILENAME = "my_mesh.stl"

def make_example_grammar() -> Grammar:
    """
    Generate an example grammar.
    Ruleset by Dr. John Rieffel. 

    Returns:
        Grammar: A basic grammar.
    """

    grammar = Grammar()

    grammar.add_rule("A", "grow", ["D", "B", "F"])
    grammar.add_rule("B", "grow", ["A", "D", "F"])
    grammar.add_rule("C", "grow", ["E", "D", "F"])
    grammar.add_rule("D", "relabel", ["D"])
    grammar.add_rule("E", "grow", ["D", "C", "F"])
    grammar.add_rule("F", "divide", ["D", "D", "D", "G"])
    grammar.add_rule("G", "grow", ["D", "D", "G"])

    return grammar

def grow_mesh(grammar: Grammar, iters: int, filename: str):
    """
    Grows a mesh using a grammar. Will be saved in the meshes directory.

    Parameters:
        grammar (Grammar): Grammar to use.
        iters (int): Number of times to apply a rule.
        filename (str): Filename of .stl file to save.
    """

    grammar = make_example_grammar()
    mesh = TetrahedralMesh(grammar)

    for i in range(iters):
        mesh.apply_rule()

    mesh.export_to_stl(filename)
        
if __name__ == "__main__":
    grow_mesh(make_example_grammar(), ITERS, FILENAME)
