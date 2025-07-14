"""
Build a TetrahedralMesh by applying a Grammar.

By Thomas Breimer
July 9th, 2025
"""

from grammar import Grammar
from tetrahedral_mesh import TetrahedralMesh

ITERS = 1900
FILENAME = "my_mesh"

ALPHABET = ["A", "B", "C", "D", "E", "F", "G"] # Labels
OPERATIONS = {"rename": 1, "grow": 3, "divide": 4} # Possible operations with number of rhs labels.

def make_example_grammar() -> Grammar:
    """
    Generate an example grammar.
    Ruleset by Dr. John Rieffel. 

    Returns:
        Grammar: A basic grammar.
    """

    grammar = Grammar(ALPHABET, OPERATIONS)

    grammar.add_rule("A", "grow", ["D", "B", "F"])
    grammar.add_rule("B", "grow", ["A", "D", "F"])
    grammar.add_rule("C", "grow", ["E", "D", "F"])
    grammar.add_rule("D", "relabel", ["D"])
    grammar.add_rule("E", "grow", ["D", "C", "F"])
    grammar.add_rule("F", "divide", ["D", "D", "D", "G"])
    grammar.add_rule("G", "grow", ["D", "G", "D"])

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
        #print("step {} ---------------------------------------".format(i))
        mesh.apply_rule()
        #mesh.export_to_stl(filename + str(i).zfill(3))

    mesh.export_to_stl(filename + str(i))
        
if __name__ == "__main__":
    grow_mesh(make_example_grammar(), ITERS, FILENAME)
