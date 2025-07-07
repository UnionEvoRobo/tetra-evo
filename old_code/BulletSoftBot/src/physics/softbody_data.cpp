#include <cstdio>       //FILE, fopen, fclose, fprintf, sprintf

#include <algorithm>    //std::min

#include "physics/softbody_data.hpp"
#include "core/grammar.hpp"

/****************************************/
/*          Public Functions            */
/****************************************/
SoftBodyData::SoftBodyData(Grammar* grammar, int expansions) : grammar(grammar){
    
    nodes.reserve(100); //reserve a some nodes to avoid a lot of relocations
    tetras.reserve(50); //same for tetras

    nodes.emplace_back(0.0,1.0,-0.353553);
    nodes.emplace_back(0.866025,-0.5,-0.353553);
    nodes.emplace_back(-0.866025,-0.5,-0.353553);
    nodes.emplace_back(0.0,0.0,1.06066);
    
    tetras.emplace_back(0,1,2,3);
    
    const char* rules = grammar->getRules();

    nonTerminalFaces.emplace_back(0,3,1,0,rules[1]);
    nonTerminalFaces.emplace_back(0,1,2,0,rules[2]);
    nonTerminalFaces.emplace_back(0,2,3,0,rules[3]);
    nonTerminalFaces.emplace_back(1,3,2,0,rules[4]);
    
    expand(expansions);
}


btSoftBody* SoftBodyData::generateSoftBody(btSoftBodyWorldInfo* worldInfo){
    
    btSoftBody* softbody = new btSoftBody(worldInfo,(int)nodes.size(),(const btVector3*)nodes.data(),0);
    
    for(int i = 0; i < (int)nonTerminalFaces.size(); ++i){
        Face& f = nonTerminalFaces[i];
        softbody->appendFace(f.node1,f.node2,f.node3);
        
        softbody->m_faces[softbody->m_faces.size()-1].m_tag = &(f.label);
    }
    
    for(int i = 0; i < (int)tetras.size(); ++i){
        Tetra& t = tetras[i];
        if(t.node1 != -1){
            softbody->appendTetra(t.node1,t.node2,t.node3,t.node4);
            
            //append the links
            softbody->appendLink(t.node1,t.node2,0,true);
            softbody->appendLink(t.node2,t.node3,0,true);
            softbody->appendLink(t.node3,t.node1,0,true);
            softbody->appendLink(t.node1,t.node4,0,true);
            softbody->appendLink(t.node2,t.node4,0,true);
            softbody->appendLink(t.node3,t.node4,0,true);
        }
    }
    
    softbody->scale(btVector3(4,4,4));
    
    softbody->generateClusters(0);
    softbody->setVolumeDensity(2);
    
//Material Properties: 
//  m_KLST = LINEAR STIFFNESS [0,1]
//  m_KAST = AREA/ANGULAR STIFFNESS [0,1]
//  m_KVST = VOLUME STIFFNESS [0,1]

    btSoftBody::Material* material = softbody->m_materials[0];

    material->m_kLST = 0.3;
    material->m_kAST = 0.7;
    material->m_kVST = 0.3;

//Softbody Properties
//  m_cfg.kVCF; ~ Velocities correction factor (Baumgarte)
//  m_cfg.kDP;  ~ Damping coefficient [0,1]
//  m_cfg.kDG;  ~ Drag coefficient [0,+inf]
//  m_cfg.kLF;  ~ Lift coefficient [0,+inf]
//  m_cfg.kPR;  ~ Pressure coefficient [-inf,+inf]
//  m_cfg.kVC;  ~ Volume conversation coefficient [0,+inf]
//  m_cfg.kDF;  ~ Dynamic friction coefficient [0,1]
//  m_cfg.kMT;  ~ Pose matching coefficient [0,1]
//  m_cfg.kCHR; ~ Rigid contacts hardness [0,1]
//  m_cfg.kKHR; ~ Kinetic contacts hardness [0,1]
//  m_cfg.kSHR; ~ Soft contacts hardness [0,1]
//  m_cfg.kAHR; ~ Anchors hardness [0,1]
    
    softbody->getCollisionShape()->setMargin(0.01);
    softbody->m_cfg.kDF = 1.0;
    softbody->m_cfg.collisions = btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;


	//THIS MIGHT BE THE BUG
    //translate softbody as close to ground as possible
    //FIXME this does not always work perfectly 
    float lowest = softbody->m_nodes[0].m_x.y();
    
    
    char debugString[100];
    if(p_placement && (p_writeToDebug || p_printToScreen))
    {
    	sprintf(debugString, "lowest before loop: %f\nNodes before translate:\n", lowest);
    	
    	
    	handleDebugString(debugString);
    	
    	
    	btSoftBody::tNodeArray nodes = softbody->m_nodes;
   		for(int i = 0; i < nodes.size(); i++)
   		{	
   			btSoftBody::Node node = nodes[i];
   			btVector3 position = node.m_x;
   			sprintf(debugString, "Position of Node %d, x: %f y: %f z: %f\n", i, position.getX(), position.getY(), position.getZ());
   			handleDebugString(debugString);
   			
   		}
    }
    
    
    //finding lowest point on soft body
    for(int i = 1; i < softbody->m_nodes.size(); ++i){
        float temp = softbody->m_nodes[i].m_x.y();
        lowest = std::min(lowest, temp);
    }
    
    
    //translating soft body up or down
    softbody->translate(btVector3(0,-lowest + 1,0));  
    
    if(p_placement && (p_writeToDebug || p_printToScreen))
    {
    	sprintf(debugString, "lowest after loop: %f\nNodes after translate:\n", lowest);
    	handleDebugString(debugString);
    	
    	btSoftBody::tNodeArray nodes = softbody->m_nodes;
    	
   		for(int i = 0; i < nodes.size(); i++)
   		{
   			btSoftBody::Node node = nodes[i];
   			btVector3 position = node.m_x;
   			
   			sprintf(debugString, "Position of Node %d, x: %f y: %f z: %f\n", i, position.getX(), position.getY(), position.getZ());
   			handleDebugString(debugString);	
   		}
    }
    
    
    return softbody;
}

void SoftBodyData::expand(int n){
    for(int i = 0; i < n; ++i){
        Face* ntf = &nonTerminalFaces.front();
    
        const char* rule = grammar->getRule(ntf->label);

        switch(rule[0]){
            case RULE::RELABEL   : relabel  (ntf, rule[1]);                            break;
            case RULE::GROW      : grow     (ntf, rule[1], rule[2], rule[3]);          break;
            case RULE::SUBDIVIDE : subdivide(ntf, rule[1], rule[2], rule[3], rule[4]); break;
        }
    
        nonTerminalFaces.pop_front();
    }
}

void SoftBodyData::exportSTL(){
    //TODO copy this over from my previous version
    char filename[20] = "./data/mesh.stl";
    
    FILE* file = fopen(filename, "r");

    for(int i = 1; file != NULL; i++){
        sprintf(filename, "./data/mesh(%d).stl", i);
        file = freopen(filename, "r", file);
    }

    file = fopen(filename, "w");
    
    for(unsigned int i = 0; i < nonTerminalFaces.size(); ++i){
        btVector3 node1 = nodes[nonTerminalFaces[i].node1];
        btVector3 node2 = nodes[nonTerminalFaces[i].node2];
        btVector3 node3 = nodes[nonTerminalFaces[i].node3];
        fprintf(file, "facet normal 0 0 0\n");
        fprintf(file, "outer loop\n");
        fprintf(file, "vertex %e %e %e\n", node1.x(), node1.y(), node1.z()); 
        fprintf(file, "vertex %e %e %e\n", node2.x(), node2.y(), node2.z());
        fprintf(file, "vertex %e %e %e\n", node3.x(), node3.y(), node3.z());
        fprintf(file, "endloop\n");
        fprintf(file, "endfacet\n");
    }

    fclose(file);
}

void SoftBodyData::setGrammarFitness(float fitness){
    grammar->setFitness(fitness);
}


void SoftBodyData::setFlags(bool writeToDebug, bool printToScreen, bool placement)
{
	//printf("about to set privates for softbody\n");
	p_writeToDebug = writeToDebug; //seg faults here
	//printf("writetodebug\n");
	p_printToScreen = printToScreen;
	//printf("printtoscreen\n");
	p_placement = placement;
	//printf("placement\n");	
	//printf("set privates in softbody\n");
	
}

void SoftBodyData::setFH(FileHandler* fh)
{
	p_fh = fh;
}

/****************************************/
/*          Private Functions           */
/****************************************/
void SoftBodyData::relabel(Face* ntf, char newLabel){
    ntf->label = newLabel;

    nonTerminalFaces.push_back(*ntf);
}

//FIXME rewrite this more clearly
void SoftBodyData::subdivide(Face* ntf, char newLabel1, char newLabel2, char newLabel3, char newLabel4){
    Tetra tet = tetras[ntf->tetra];
    
    if(tet.node1 >= 0){
        
        btVector3 vx0 = nodes[tet.node1];
        btVector3 vx1 = nodes[tet.node2];
        btVector3 vx2 = nodes[tet.node3];
        btVector3 vx3 = nodes[tet.node4];
        
        //add 7 intermediate nodes
        btVector3 vx4((vx0.x()+vx1.x())/2.,(vx0.y()+vx1.y())/2.,(vx0.z()+vx1.z())/2.); // v4-9 are the midpoints of the edges
        btVector3 vx5((vx0.x()+vx2.x())/2.,(vx0.y()+vx2.y())/2.,(vx0.z()+vx2.z())/2.);
        btVector3 vx6((vx0.x()+vx3.x())/2.,(vx0.y()+vx3.y())/2.,(vx0.z()+vx3.z())/2.);
        btVector3 vx7((vx1.x()+vx2.x())/2.,(vx1.y()+vx2.y())/2.,(vx1.z()+vx2.z())/2.);
        btVector3 vx8((vx1.x()+vx3.x())/2.,(vx1.y()+vx3.y())/2.,(vx1.z()+vx3.z())/2.);
        btVector3 vx9((vx2.x()+vx3.x())/2.,(vx2.y()+vx3.y())/2.,(vx2.z()+vx3.z())/2.);
        btVector3 vx10((vx4.x()+vx9.x())/2.,(vx4.y()+vx9.y())/2.,(vx4.z()+vx9.z())/2.); // v10 is the center of the tetra
        
        int lastNode = nodes.size();
        
        int v0 = tet.node1;
        int v1 = tet.node2;
        int v2 = tet.node3;
        int v3 = tet.node4;
        int v4 = lastNode++;
        int v5 = lastNode++;
        int v6 = lastNode++;
        int v7 = lastNode++;
        int v8 = lastNode++;
        int v9 = lastNode++;
        int v10 = lastNode++;
        
        nodes.push_back(vx4);
        nodes.push_back(vx5);
        nodes.push_back(vx6);
        nodes.push_back(vx7);
        nodes.push_back(vx8);
        nodes.push_back(vx9);
        nodes.push_back(vx10);
        
        int lastTetra = tetras.size();
        
        int t0 = lastTetra++;
        int t1 = lastTetra++;
        int t2 = lastTetra++;
        int t3 = lastTetra++;
        int t012 = lastTetra++;
        int t013 = lastTetra++;
        int t023 = lastTetra++;
        int t123 = lastTetra++;
        
        tetras.emplace_back(v0,v4,v5,v6); // the tetrahedra at the corners
        tetras.emplace_back(v1,v4,v7,v8);
        tetras.emplace_back(v2,v5,v7,v9);
        tetras.emplace_back(v3,v6,v8,v9);
        
        tetras.emplace_back(v4,v5,v7,v10); // the tetrahedra in the middle
        tetras.emplace_back(v4,v6,v8,v10);
        tetras.emplace_back(v5,v6,v9,v10);
        tetras.emplace_back(v7,v8,v9,v10);
        
        tetras.emplace_back(v4,v5,v6,v10); // the inside tetrahedra
        tetras.emplace_back(v4,v7,v8,v10);
        tetras.emplace_back(v5,v7,v9,v10);
        tetras.emplace_back(v6,v8,v9,v10);
        
        // the invalidate the original tetrahedron
        tet.node1 = -1;
        tet.node2 = -1;
        tet.node3 = -1;
        tet.node4 = -1;
        
        // create 4 new faces, and add them
        int curf[6]; // the vertices that make up the current face
        int curt[4]; // the four associated tetrahedra
        if(ntf->node1 == v0)      {curf[0] = v0; curt[0] = t0;}
        else if(ntf->node1 == v1) {curf[0] = v1; curt[0] = t1;}
        else if(ntf->node1 == v2) {curf[0] = v2; curt[0] = t2;}
        else if(ntf->node1 == v3) {curf[0] = v3; curt[0] = t3;}
        if(ntf->node2 == v0)      {curf[1] = v0; curt[1] = t0;}
        else if(ntf->node2 == v1) {curf[1] = v1; curt[1] = t1;}
        else if(ntf->node2 == v2) {curf[1] = v2; curt[1] = t2;}
        else if(ntf->node2 == v3) {curf[1] = v3; curt[1] = t3;}
        if(ntf->node3 == v0)      {curf[2] = v0; curt[2] = t0;}
        else if(ntf->node3 == v1) {curf[2] = v1; curt[2] = t1;}
        else if(ntf->node3 == v2) {curf[2] = v2; curt[2] = t2;}
        else if(ntf->node3 == v3) {curf[2] = v3; curt[2] = t3;}
        
        if(curf[0] == v0 && curf[1] == v1)      curf[3] = v4;
        else if(curf[0] == v1 && curf[1] == v0) curf[3] = v4;
        else if(curf[0] == v0 && curf[1] == v2) curf[3] = v5;
        else if(curf[0] == v2 && curf[1] == v0) curf[3] = v5;
        else if(curf[0] == v0 && curf[1] == v3) curf[3] = v6;
        else if(curf[0] == v3 && curf[1] == v0) curf[3] = v6;
        else if(curf[0] == v1 && curf[1] == v2) curf[3] = v7;
        else if(curf[0] == v2 && curf[1] == v1) curf[3] = v7;
        else if(curf[0] == v1 && curf[1] == v3) curf[3] = v8;
        else if(curf[0] == v3 && curf[1] == v1) curf[3] = v8;
        else if(curf[0] == v2 && curf[1] == v3) curf[3] = v9;
        else if(curf[0] == v3 && curf[1] == v2) curf[3] = v9;
        if(curf[1] == v0 && curf[2] == v1)      curf[4] = v4;
        else if(curf[1] == v1 && curf[2] == v0) curf[4] = v4;
        else if(curf[1] == v0 && curf[2] == v2) curf[4] = v5;
        else if(curf[1] == v2 && curf[2] == v0) curf[4] = v5;
        else if(curf[1] == v0 && curf[2] == v3) curf[4] = v6;
        else if(curf[1] == v3 && curf[2] == v0) curf[4] = v6;
        else if(curf[1] == v1 && curf[2] == v2) curf[4] = v7;
        else if(curf[1] == v2 && curf[2] == v1) curf[4] = v7;
        else if(curf[1] == v1 && curf[2] == v3) curf[4] = v8;
        else if(curf[1] == v3 && curf[2] == v1) curf[4] = v8;
        else if(curf[1] == v2 && curf[2] == v3) curf[4] = v9;
        else if(curf[1] == v3 && curf[2] == v2) curf[4] = v9;
        if(curf[0] == v0 && curf[2] == v1)      curf[5] = v4;
        else if(curf[0] == v1 && curf[2] == v0) curf[5] = v4;
        else if(curf[0] == v0 && curf[2] == v2) curf[5] = v5;
        else if(curf[0] == v2 && curf[2] == v0) curf[5] = v5;
        else if(curf[0] == v0 && curf[2] == v3) curf[5] = v6;
        else if(curf[0] == v3 && curf[2] == v0) curf[5] = v6;
        else if(curf[0] == v1 && curf[2] == v2) curf[5] = v7;
        else if(curf[0] == v2 && curf[2] == v1) curf[5] = v7;
        else if(curf[0] == v1 && curf[2] == v3) curf[5] = v8;
        else if(curf[0] == v3 && curf[2] == v1) curf[5] = v8;
        else if(curf[0] == v2 && curf[2] == v3) curf[5] = v9;
        else if(curf[0] == v3 && curf[2] == v2) curf[5] = v9;

        if((curf[0] == v0 && curf[1] == v1 && curf[2] == v2) ||
           (curf[0] == v0 && curf[1] == v2 && curf[2] == v1) ||
           (curf[0] == v1 && curf[1] == v0 && curf[2] == v2) ||
           (curf[0] == v1 && curf[1] == v2 && curf[2] == v0) ||
           (curf[0] == v2 && curf[1] == v0 && curf[2] == v1) ||
           (curf[0] == v2 && curf[1] == v1 && curf[2] == v0)) curt[3] = t012;
        if((curf[0] == v0 && curf[1] == v1 && curf[2] == v3) ||
           (curf[0] == v0 && curf[1] == v3 && curf[2] == v1) ||
           (curf[0] == v1 && curf[1] == v0 && curf[2] == v3) ||
           (curf[0] == v1 && curf[1] == v3 && curf[2] == v0) ||
           (curf[0] == v3 && curf[1] == v0 && curf[2] == v1) ||
           (curf[0] == v3 && curf[1] == v1 && curf[2] == v0)) curt[3] = t013;
        if((curf[0] == v0 && curf[1] == v3 && curf[2] == v2) ||
           (curf[0] == v0 && curf[1] == v2 && curf[2] == v3) ||
           (curf[0] == v3 && curf[1] == v0 && curf[2] == v2) ||
           (curf[0] == v3 && curf[1] == v2 && curf[2] == v0) ||
           (curf[0] == v2 && curf[1] == v0 && curf[2] == v3) ||
           (curf[0] == v2 && curf[1] == v3 && curf[2] == v0)) curt[3] = t023;
        if((curf[0] == v3 && curf[1] == v1 && curf[2] == v2) ||
           (curf[0] == v3 && curf[1] == v2 && curf[2] == v1) ||
           (curf[0] == v1 && curf[1] == v3 && curf[2] == v2) ||
           (curf[0] == v1 && curf[1] == v2 && curf[2] == v3) ||
           (curf[0] == v2 && curf[1] == v3 && curf[2] == v1) ||
           (curf[0] == v2 && curf[1] == v1 && curf[2] == v3)) curt[3] = t123;
           
        nonTerminalFaces.emplace_back(curf[0],curf[3],curf[5],curt[0],newLabel1);
        nonTerminalFaces.emplace_back(curf[3],curf[1],curf[4],curt[1],newLabel2);
        nonTerminalFaces.emplace_back(curf[5],curf[4],curf[2],curt[2],newLabel3);
        nonTerminalFaces.emplace_back(curf[5],curf[3],curf[4],curt[3],newLabel4);
        
    }else{
        // convert one face into four subdivided faces
        btVector3 vx0 = nodes[ntf->node1];
        btVector3 vx1 = nodes[ntf->node2];
        btVector3 vx2 = nodes[ntf->node3];
        
        btVector3 vx3((vx0.x()+vx1.x())/2.,(vx0.y()+vx1.y())/2.,(vx0.z()+vx1.z())/2.);
        btVector3 vx4((vx1.x()+vx2.x())/2.,(vx1.y()+vx2.y())/2.,(vx1.z()+vx2.z())/2.);
        btVector3 vx5((vx0.x()+vx2.x())/2.,(vx0.y()+vx2.y())/2.,(vx0.z()+vx2.z())/2.);
        
        int lastNode = nodes.size();
        
        int v0 = ntf->node1;
        int v1 = ntf->node2;
        int v2 = ntf->node3;
        int v3 = lastNode++;
        int v4 = lastNode++;
        int v5 = lastNode++;
        
        nodes.push_back(vx3);
        nodes.push_back(vx4);
        nodes.push_back(vx5);
        
        // find tetra for each new face
        int t0,t1,t2,t3;
        for(int i = 0; i < (int)tetras.size(); ++i){
            if(tetras[i].node1 >= 0){
                if      (matches(i,v0,v3,v5)) t0 = i;
                else if (matches(i,v3,v1,v4)) t1 = i;
                else if (matches(i,v5,v4,v2)) t2 = i;
                else if (matches(i,v5,v3,v4)) t3 = i;
            }
        }
        
        nonTerminalFaces.emplace_back(v0,v3,v5,t0,newLabel1);
        nonTerminalFaces.emplace_back(v3,v1,v4,t1,newLabel2);
        nonTerminalFaces.emplace_back(v5,v4,v2,t2,newLabel3);
        nonTerminalFaces.emplace_back(v5,v3,v4,t3,newLabel4);
    }
}

void SoftBodyData::grow(Face* ntf, char newLabel1, char newLabel2, char newLabel3){
    btVector3 v1 = nodes[ntf->node1];
    btVector3 v2 = nodes[ntf->node2];
    btVector3 v3 = nodes[ntf->node3];
    
    btVector3 center((v1.x()+v2.x()+v3.x())/3.,(v1.y()+v2.y()+v3.y())/3.,(v1.z()+v2.z()+v3.z())/3.);
    btVector3 ctov1 (v1.x()-center.x(),v1.y()-center.y(),v1.z()-center.z());
    btVector3 ctov2 (v2.x()-center.x(),v2.y()-center.y(),v2.z()-center.z());

    btVector3 normal = ctov1.cross(ctov2);

    normal = normal.normalized();
    normal *= (v1.distance(v2) * 0.81649658);
    
    int lastNode  = nodes.size();
    int lastTetra = tetras.size(); 
    
    nodes.emplace_back(center.x()+normal.x(),center.y()+normal.y(),center.z()+normal.z());
    
    tetras.emplace_back(ntf->node1,ntf->node2,ntf->node3,lastNode);
    
    nonTerminalFaces.emplace_back(ntf->node1,ntf->node2,lastNode,lastTetra,newLabel1);
    nonTerminalFaces.emplace_back(ntf->node2,ntf->node3,lastNode,lastTetra,newLabel2);
    nonTerminalFaces.emplace_back(ntf->node3,ntf->node1,lastNode,lastTetra,newLabel3);
}

bool SoftBodyData::equal(const btVector3& v1, const btVector3& v2){
    return (v1.distance(v2) < 0.001);
}

bool SoftBodyData::matches(int tetra, int node0, int node1, int node2){
    Tetra t = tetras[tetra];
    
    btVector3 tv0 = nodes[t.node1];
    btVector3 tv1 = nodes[t.node2];
    btVector3 tv2 = nodes[t.node3];
    btVector3 tv3 = nodes[t.node4];
    btVector3 fv0 = nodes[node0];
    btVector3 fv1 = nodes[node1];
    btVector3 fv2 = nodes[node2];
    
    
    return ((equal(tv0,fv0) || equal(tv1,fv0) || equal(tv2,fv0) || equal(tv3,fv0)) &&
            (equal(tv0,fv1) || equal(tv1,fv1) || equal(tv2,fv1) || equal(tv3,fv1)) && 
            (equal(tv0,fv2) || equal(tv1,fv2) || equal(tv2,fv2) || equal(tv3,fv2)));
}



void SoftBodyData::handleDebugString(char* debugString)
{
	if(p_printToScreen)
   		{
   			printf("%s", debugString);
   		}
   	if(p_writeToDebug)
   		{
   			p_fh->writeDebugFile(debugString);
   		}
}







