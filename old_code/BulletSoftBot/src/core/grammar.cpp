#include <cstdlib>  //rand
#include <cstring>  //strncpy
#include <cstdio>   //printf

#include "core/grammar.hpp"

/****************************************/
/*          Public Functions            */
/****************************************/


//THE LENGTH OF RULES HAS TO BE EXACTLY 25 W/ 
// NO SPACES TO WORK PROPERLY
Grammar::Grammar(const char* rules) : fitness(0.0){
    setRules(rules);
}

Grammar::Grammar() : fitness(0.0){
    randomize();
}

const char* Grammar::getRule(char label) const{
    return rules.data() + RULE_SIZE*(label - 'a' + 1);
}

const char* Grammar::getRules() const{
    return rules.data();
}

float Grammar::getFitness() const{
    return fitness;
}

int Grammar::getDifference(const Grammar& other) const{
    const char* otherRules = other.getRules();

    int difference = 0;

    for(unsigned int i = 0; i < rules.size(); ++i){
        if(rules[i] != otherRules[i]){
            //if rule : difference = 5 if label : difference = 1
            difference += (i % RULE_SIZE == 0) ? 5 : 1;
        }
    }

    return difference;
}

void Grammar::setRules(const char* new_rules){
    strncpy(rules.data(), new_rules, rules.size());
}

void Grammar::setFitness(float new_fitness){
    fitness = new_fitness;
}

void Grammar::mutate(){
    
    for(unsigned int i = 0; i < NON_TERMINALS + 1; ++i){
        int r = rand() % 100;
        
        if(r < 50){
        
            if(r < 25 && i != 0){
                //mutate the rule type
                
                switch(r % 3){
                    case 0 : rules[RULE_SIZE*i] = RULE::RELABEL;   break;
                    case 1 : rules[RULE_SIZE*i] = RULE::GROW;      break;
                    case 2 : rules[RULE_SIZE*i] = RULE::SUBDIVIDE; break;
                }
            }else{
                //mutate 1-2 labels
                
                int randomIndex;
                char rule = rules[RULE_SIZE*i];

                switch(rule){
                    case RULE::RELABEL   : 
                        rules[RULE_SIZE*i+1] = randomNonTerminalLabel(); 
                        break;

                    case RULE::GROW      :
                        randomIndex = (RULE_SIZE*i) + (rand() % 3 + 1);
                        rules[randomIndex] = randomNonTerminalLabel();
                        randomIndex = (RULE_SIZE*i) + (rand() % 3 + 1);
                        rules[randomIndex] = randomNonTerminalLabel();
                        break;

                    case RULE::SUBDIVIDE :
                    case RULE::INITIAL   : 
                        randomIndex = (RULE_SIZE*i) + (rand() % 4 + 1);
                        rules[randomIndex] = randomNonTerminalLabel();
                        randomIndex = (RULE_SIZE*i) + (rand() % 4 + 1);
                        rules[randomIndex] = randomNonTerminalLabel();
                        break;
                }
            }
        }
    }

    checkRules();
}

void Grammar::crossover(const Grammar& other){
    
    for(unsigned int i = 0; i < NON_TERMINALS; ++i){
        int r = rand() % 100;

        //swap rules uniformly (50% of the time)
        //don't allow swap of initial rule
        if(r < 50){
            strncpy(rules.data() + RULE_SIZE*(i + 1), other.getRule('a' + i), RULE_SIZE);
        }
    }

    checkRules();
}

void Grammar::randomize(){
    
    rules[0] = RULE::INITIAL;
    rules[1] = randomNonTerminalLabel();
    rules[2] = randomNonTerminalLabel();
    rules[3] = randomNonTerminalLabel();
    rules[4] = randomNonTerminalLabel();

    for(auto rule = rules.begin() + RULE_SIZE; rule != rules.end(); rule += RULE_SIZE){
    
        switch(rand() % 3){
            case 0 : rule[0] = RULE::RELABEL;    break;
            case 1 : rule[0] = RULE::GROW;       break;
            case 2 : rule[0] = RULE::SUBDIVIDE;  break;
        }

        rule[1] = randomNonTerminalLabel();
        rule[2] = randomNonTerminalLabel();
        rule[3] = randomNonTerminalLabel();
        rule[4] = randomNonTerminalLabel();
    }

    checkRules();
}

//If rules do not have at leas one growth component, randomize the rules
void Grammar::checkRules() {
    bool hasGrow = false;

    for (int i=5; i<25; i+=5) {
        if (getRules()[i] == 'g') {
            hasGrow = true;
            break;
        }
    }

    if (hasGrow == false) {
        randomize();
    }
}

//turn grammar into a printable string
char* Grammar::toString() const
{
	char* str = (char *)malloc(100 * sizeof(char));

    sprintf(str, "Grammar: rules = ");

    for(auto rule = rules.begin(); rule != rules.end(); rule += RULE_SIZE){
        sprintf(str + strlen(str), "%c%c%c%c%c ", rule[0], rule[1], rule[2], rule[3], rule[4]);
    }

    sprintf(str + strlen(str), " fitness = %f\n", fitness);
    
    return str;
}

//print grammar to the screen
void Grammar::print() const{

	printf("%s", toString());
}
    
/****************************************/
/*          Private Functions           */
/****************************************/
char Grammar::randomNonTerminalLabel(){
    return 'a' + rand() % NON_TERMINALS;
}

