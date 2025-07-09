"""
Define a grammar for modifying tetrahedral meshes.

By Thomas Breimer
July 9th, 2025
"""

from dataclasses import dataclass, field

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

    rules: dict[Rule] = field(default_factory=dict)

    def add_rule(self, lhs: str, func: str, rhs: list[str]):
        """
        Add a rule to the grammar.

        Parameters:
            lhs (str): Face label for the lhs of the rule.
            func (str): Operation to be preformd on the lhs. Can be "relabel", "grow", or "divide". 
            rhs (list[str]): Face labels to be generated on application of the rule.
        """
        new_rule = Rule(func, rhs)
        self.rules[lhs] = new_rule

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

        Parameters:
            label (str): LHS of rule rhs to get.

        Return:
            list[str]: "relabel", "grow", or "divide".
        """

        return self.rules[label].rhs
