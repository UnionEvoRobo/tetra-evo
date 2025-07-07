#ifndef CORE_GRAMMAR_HPP
#define CORE_GRAMMAR_HPP

#include <array>    //std::array

enum RULE{
    INITIAL   = 'i',
    RELABEL   = 'r',
    SUBDIVIDE = 's',
    GROW      = 'g'
};

class Grammar{

    public:
        explicit    Grammar(const char* rules);
                    Grammar();
        
        const char* getRule(char label)                 const;
        const char* getRules()                          const;
        float       getFitness()                        const;
        int         getDifference(const Grammar& other) const;
        
        void        setRules(const char* new_rules);
        void        setFitness(float new_fitness);

        void        mutate();
        void        crossover(const Grammar& other);
        void        randomize();
        void        checkRules();
        
        char* 		toString()                          const;
        void        print()                             const;
        
      
    private:
        char        randomNonTerminalLabel();

        static const unsigned int NON_TERMINALS = 4; //[1,inf]
        static const unsigned int RULE_SIZE     = 5; //DO NOT CHANGE!
       
        std::array<char, RULE_SIZE * (NON_TERMINALS+1)> rules;
        float fitness;
};

#endif
