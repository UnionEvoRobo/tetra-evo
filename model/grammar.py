"""
Define a grammar for modifying tetrahedral meshes.
Empowered with mutuation and crossover abillities.

By Thomas Breimer
July 9th, 2025
"""

from dataclasses import dataclass
import random
import copy


@dataclass
class Rule:
    """
    A single Rule.
    
    Attributes:
        operation (str): Operation to be preformd on the lhs. Can be "rename", "grow", or "split". 
        rhs (str): Symbols on the right hand side of the rule.
    """
    operation: str
    rhs: list[str]


@dataclass
class Grammar:
    """
    A collection of Rules.
    """

    def __init__(self, alphabet: list = None, operations: dict[str, int] = None):
        """
        Initialize an empty grammar.

        Parameters:
            alphabet (list[str]): List of all valid labels.
            operations (dict[str, int]): A dict of strings representing operations with the corresponding 
                                         number of rhs labels for that rule.
        """

        self.alphabet = alphabet
        self.operations = operations
        self.rules = {}

    def add_rule(self, lhs: str, func: str, rhs: list[str]):
        """
        Add a rule to the grammar.

        Parameters:
            lhs (str): Face label for the lhs of the rule.
            func (str): Operation to be preformed on the lhs. Can be "relabel", "grow", or "divide". 
            rhs (list[str]): Face labels to be generated on application of the rule.
        """
        new_rule = Rule(func, rhs)
        self.rules[lhs] = new_rule

    def add_from_dict(self, new_rules: dict):
        """
        Adds new rules from a dict. Expects a dict in the form of {"lhs0": A, "operation0": grow, "rhs0":BCD, lhs1: ...}
        """

        for i in range(len(new_rules)):
            if "lhs" + str(i) in new_rules.keys():
                rhs = list(new_rules["rhs" + str(i)])
                self.add_rule(new_rules["lhs" + str(i)], new_rules["operation" + str(i)], rhs)
            else:
                break

    def clear(self):
        """
        Erases all the rules in this grammar.
        """

        self.rules = {}

    def get_rule_operation(self, label: str) -> str:
        """
        Get the rule operation associated with the given label.

        Parameters:
            label (str): LHS of rule operation to get.

        Return:
            str: "relabel", "grow", or "divide".
        """

        return self.rules[label].operation

    def get_rule_rhs(self, label: str) -> list[str]:
        """
        Get the rule rhs associated with the given label.

        Parameters:s
            label (str): LHS of rule rhs to get.

        Return:
            list[str]: The face labels resulting from the operation, the rhs.
        """

        return self.rules[label].rhs

    def regenerate_random(self, probability: float):
        """
        Changes rules in the grammar randomly.
        
        Parameters:
            probability (float): Chance to regenerate each rule.
        """

        for lhs in self.rules:
            if random.random() < probability:
                operation = random.choice(list(self.operations.keys()))  # Get random operation
                rhs = random.sample(self.alphabet, self.operations[operation])  # Get random labels for rhs
                self.rules[lhs] = Rule(operation, rhs)

    def generate_random(self):
        """
        Erases all rules in the grammar and generates a new random grammar.

        Returns:
            Grammar: This Grammar object.
        """
        self.clear()

        for label in self.alphabet:
            operation = random.choice(list(self.operations.keys()))  # Get random operation
            rhs = random.sample(self.alphabet, self.operations[operation])  # Get random labels for rhs
            self.add_rule(label, operation, rhs)

        return self

    def crossover(self, other_grammar):
        """
        Execute single point crossover. Modifies both grammars in place.

        Parameters:
            other_grammar (Grammar): Grammar to crossover with.
        """
        assert self.operations == other_grammar.operations, "Tried to crossover two grammars with different operation sets!"
        assert self.alphabet == other_grammar.alphabet, "Tried to crossover two grammars with different alphabets!"

        my_rule_list = self.get_rules_list()
        other_rule_list = other_grammar.get_rules_list()

        assert len(my_rule_list) == len(
            other_rule_list), "Tried to crossover two grammars with different numbers of rules!"

        pivot_location = random.randint(0, len(my_rule_list) - 1)  # Choose pivot location

        # Perform crossover and set new rules
        self.set_rules_from_list(my_rule_list[0:pivot_location] + other_rule_list[pivot_location:])
        other_grammar.set_rules_from_list(other_rule_list[0:pivot_location] + my_rule_list[pivot_location:])

    def two_point_crossover(self, other_grammar):
        """
        Execute two point crossover. Modifies both grammars in place.

        Parameters:
            other_grammar (Grammar): Grammar to crossover with.
        """

        assert self.operations == other_grammar.operations, "Tried to crossover two grammars with different operation sets!"
        assert self.alphabet == other_grammar.alphabet, "Tried to crossover two grammars with different alphabets!"

        my_rule_list = self.get_rules_list()
        other_rule_list = other_grammar.get_rules_list()

        assert len(my_rule_list) == len(
            other_rule_list), "Tried to crossover two grammars with different numbers of rules!"

        # Get two crossover points
        pivot_location1 = random.randint(0, len(my_rule_list) - 2)
        pivot_location2 = random.randint(pivot_location1, len(my_rule_list) - 1)

        #
        my_new_rule_list = my_rule_list[0:pivot_location1] + other_rule_list[
            pivot_location1:pivot_location2] + my_rule_list[pivot_location2:]
        other_new_rule_list = other_rule_list[0:pivot_location1] + my_rule_list[
            pivot_location1:pivot_location2] + other_rule_list[pivot_location2:]

        self.set_rules_from_list(my_new_rule_list)
        other_grammar.set_rules_from_list(other_new_rule_list)

    def uniform_crossover(self, other_grammar):
        """
        Execute two point crossover. Modifies both grammars in place.

        Parameters:
            other_grammar (Grammar): Grammar to crossover with.
        """

        assert self.operations == other_grammar.operations, "Tried to crossover two grammars with different operation sets!"
        assert self.alphabet == other_grammar.alphabet, "Tried to crossover two grammars with different alphabets!"
        assert len(self.rules) == len(
            other_grammar.rules), "Tried to crossover two grammars with different numbers of rules!"

        # Switch rules with random probability
        for lhs, rule in self.rules.items():
            if random.random() < 0.5:
                other_rule = other_grammar.rules[lhs]
                other_grammar.rules[lhs] = rule
                self.rules[lhs] = other_rule

    def set_rules_from_list(self, new_rules):
        """
        Set grammar rules from list.

        Parameters:
            new_rules: list[str, Rule]: New rules. Assumes a format of list[str, Rule] where the str is the lhs label.
        """

        self.rules = dict(new_rules)

    def get_rules_list(self):
        """
        Returns the rules of this grammar as a list of (lhs, Rule) tuple pairs.
        
        Returns:
            list[str, Rule]: This grammar as a list of (lhs, Rule) tuple pairs.
        """

        return list(self.rules.items())

    def copy(self):
        """
        Copies this grammar and returns a new identical grammar.

        Returns:
            Grammar: A new identical Grammar.
        """

        new_grammar = Grammar(self.alphabet, self.operations)

        for lhs, rule in self.rules.items():
            new_grammar.add_rule(lhs[:], rule.operation[:], copy.deepcopy(rule.rhs))

        return new_grammar

    def to_dict(self):
        """
        Returns a dict representation of the Grammar in the form of {lhs0:"A", operation0:"grow", rhs0:"BCD", lhs1:...}

        Returns: A dict representation of the Grammar.
        """

        to_return = {}
        for i, (lhs, rule) in enumerate(self.rules.items()):
            to_return["lhs" + str(i)] = lhs
            to_return["operation" + str(i)] = rule.operation
            to_return["rhs" + str(i)] = ''.join(rule.rhs)  # Join rhs list into str

        return to_return

    def __str__(self):
        """
        Returns a string representation of the Grammar.

        Returns:
            str: A string representation of the Grammar.
        """

        to_return = "Grammar Object\n"

        for lhs, rule in self.rules.items():
            newln = "{} -> {} {}\n".format(lhs, rule.operation, rule.rhs)
            to_return += newln

        return to_return
