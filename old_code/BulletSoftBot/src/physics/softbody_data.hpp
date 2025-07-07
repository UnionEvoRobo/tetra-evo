#ifndef PHYSICS_SOFTBODY_DATA_HPP
#define PHYSICS_SOFTBODY_DATA_HPP

#include <deque>
#include <vector>

#include <BulletSoftBody/btSoftBody.h>
#include "core/file_handler.hpp"

//forward declarations
class Grammar;

class Face{

    public:
        int  node1;
        int  node2;
        int  node3;
        int  tetra;
        char label;
    
        Face(int node1, int node2, int node3, int tetra, char label) :
            node1(node1),
            node2(node2),
            node3(node3),
            tetra(tetra),
            label(label){}
};

class Tetra{

    public:
        int  node1;
        int  node2;
        int  node3;
        int  node4;
    
        Tetra(int node1, int node2, int node3, int node4) :
            node1(node1),
            node2(node2),
            node3(node3),
            node4(node4){}
};

class SoftBodyData{
    
    public:
        SoftBodyData(Grammar* grammar, int expansions);
        
        btSoftBody* generateSoftBody(btSoftBodyWorldInfo* worldInfo);
        
        void expand(int num = 1);
        void exportSTL();

        void setGrammarFitness(float fitness);
        
        void setFlags(bool writeToDebug, bool printToScreen, bool placement);
        void setFH(FileHandler* p_fh);
        
    private:
        std::vector<btVector3>  nodes;
        std::vector<Tetra>      tetras;
        std::deque<Face>        nonTerminalFaces;
        
        void relabel    (Face* ntf, char newLabel);
        void grow       (Face* ntf, char newLabel1, char newLabel2, char newLabel3);
        void subdivide  (Face* ntf, char newLabel1, char newLabel2, char newLabel3, char newLabel4);
        
        bool equal(const btVector3& v1, const btVector3& v2);
        bool matches(int tetra, int node0, int node1, int node2);
        
        Grammar* grammar;
        
        bool p_writeToDebug; 
        bool p_printToScreen; 
        bool p_placement;
        
        FileHandler* p_fh;
        
        void handleDebugString(char* debugString);
        
};

#endif
