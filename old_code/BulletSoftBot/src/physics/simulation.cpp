#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "physics/simulation.hpp"
#include "physics/softbody_data.hpp"
#include "physics/movement_mechanism.hpp"


#include <fstream>		// reading and writing files
#include <cstdio>       //
#include <cstdio>       //printf, FILE, fopen, fclose, fprintf, sprintf

//#include <vector>


/****************************************/
/*          Public Functions            */
/****************************************/
Simulation::Simulation(FileHandler* fh, unsigned int WAIT_TIME, unsigned int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT) : isRunning(true) {

    //init instance variables
    setFH(fh);
    setWaitTime(WAIT_TIME);
    setEvalTime(EVAL_TIME);
    setFlags(WRITE_TO_DEBUG, PRINT_TO_SCREEN, DEBUG_NODE_POSITIONS, DEBUG_PLACEMENT);

    //initialize bullet physics
    broadphase      = new btDbvtBroadphase();
    collisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
    dispatcher      = new btCollisionDispatcher(collisionConfig);
    solver          = new btSequentialImpulseConstraintSolver();
    softBodySolver  = new btDefaultSoftBodySolver();
    dynamicsWorld   = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionConfig, softBodySolver);

    dynamicsWorld->setGravity(btVector3(0,-9.8,0));

    //create ground
    createBox(0, btQuaternion(0,0,0), btVector3(0,0,0), btVector3(1000,0,1000), btVector3(1.0,1.0,1.0));

    movementMechanism = new Vibration(this);
}   

Simulation::~Simulation(){

    //remove all constraints from the collision world and delete them 
    for (int i = dynamicsWorld->getNumConstraints()-1; i>=0; --i){
        btTypedConstraint* constraint = dynamicsWorld->getConstraint(i);
        dynamicsWorld->removeConstraint(constraint);
        delete constraint;
    }

    //remove all softbodies from the collision world and delete them
    for (int i = dynamicsWorld->getSoftBodyArray().size()-1; i>=0; --i){
        btSoftBody* softbody = dynamicsWorld->getSoftBodyArray()[i];
        dynamicsWorld->removeSoftBody(softbody);
        delete softbody;
    } 

    //remove all rigidbodies from the collision world and delete them
    for (int i = dynamicsWorld->getNumCollisionObjects()-1; i>=0; --i){
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = (btRigidBody*)obj;
        if (body){
            if(body->getMotionState())      delete body->getMotionState();
            if(body->getCollisionShape())   delete body->getCollisionShape();
        }

        dynamicsWorld->removeCollisionObject(obj);
        delete (btVector3*)obj->getUserPointer();
        delete obj;
    }   
    
    delete dynamicsWorld;
    delete softBodySolver;
    delete solver;
    delete dispatcher;
    delete collisionConfig;
    delete broadphase;
}

void Simulation::toggleRunning(){
    isRunning = !isRunning;
}

int Simulation::getWaitTime()
{
	return MAX_WAIT_TIME;
}

int Simulation::getEvalTime()
{
	return MAX_EVAL_TIME;
}

void Simulation::setWaitTime(int time)
{
	MAX_WAIT_TIME = time;
}

void Simulation::setEvalTime(int time)
{
	MAX_EVAL_TIME = time;
}

void Simulation::setFlags(bool writeToDebug, bool printToScreen, bool nodePostion, bool placement)
{
	p_writeToDebug = writeToDebug;
	p_printToScreen = printToScreen;
	p_nodePostion = nodePostion;
	p_placement = placement;
	//printf("set private vari in sim\n");
	
	
	
}

void Simulation::setFH(FileHandler* fh)
{
	p_fh = fh;	
}

void Simulation::initialize(SoftBodyData* softbodydata){
    this->isWaiting    = true;
    this->timer        = 0.0;
    this->softbodydata = softbodydata;
    this->movementMechanism->disable();
    softbodydata->setFlags(p_writeToDebug, p_printToScreen, p_placement);
	//printf("set flags for softbody\n");
	softbodydata->setFH(p_fh);
    createSoftBody();
}

bool Simulation::simulate(btScalar timeStep){
    if(!isRunning) return false;

    dynamicsWorld->stepSimulation(timeStep,10);
    movementMechanism->onTimeStep();

    timer += timeStep;
    
    //print all node positions 
   	if(p_nodePostion && (p_writeToDebug || p_printToScreen))
   	{
   		btSoftBody* softbody = dynamicsWorld->getSoftBodyArray()[0];
   		btSoftBody::tNodeArray nodes = softbody->m_nodes;
   		//btVector3 com = softbody->m_pose.m_com;
   		
   		//fprintf(debugFile, "\nThe COM is, x: %f y: %f z: %f\n\n", com.getX(), com.getY(), com.getZ());
   		
   		char debugString[100];
   		sprintf(debugString, "\nPrinting all node positions\n\n");
   		handleDebugString(debugString);
   	   	for(int i = 0; i < nodes.size(); i++) //was node.size()-1
   		{
   			btSoftBody::Node node = nodes[i];
   			btVector3 position = node.m_x;
   			sprintf(debugString, "Position of Node %d, x: %f y: %f z: %f\n", i, position.getX(), position.getY(), position.getZ());
   			handleDebugString(debugString);
   		}	
   	}
   
    
    if(isWaiting && timer >= MAX_WAIT_TIME){
        isWaiting = false;
        timer = 0.0;
            
        movementMechanism->enable();

        btSoftBody* softbody = dynamicsWorld->getSoftBodyArray()[0];

        initialPosition = softbody->m_nodes[0].m_x; //initial position //change to center?
        initialPosition.setY(0);
            
        return false;
        
    }else if(timer >= MAX_EVAL_TIME){
        btSoftBody* softbody = dynamicsWorld->getSoftBodyArray()[0];

        btVector3 finalPosition = softbody->m_nodes[0].m_x; //final psoition
        finalPosition.setY(0);

        float fitness = initialPosition.distance2(finalPosition);
        
        softbodydata->setGrammarFitness(fitness);

        return true;
    }
    
    return false;
    
    
}

btRigidBody* Simulation::createBox(btScalar mass, btQuaternion rotation, btVector3 position, btVector3 halfExtents, btVector3 color){
    btCollisionShape*       boxShape    = new btBoxShape(halfExtents);
    btDefaultMotionState*   boxMotion   = new btDefaultMotionState(btTransform(rotation, position));

    btVector3 inertia = btVector3(0,0,0);
    boxShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotion, boxShape, inertia);
    btRigidBody* box = new btRigidBody(boxRigidBodyCI);
    
    box->setUserPointer(new btVector3(color.x(), color.y(), color.z()));

    dynamicsWorld->addRigidBody(box);
    return box;
}

btRigidBody* Simulation::createCone(btScalar mass, btQuaternion rotation, btVector3 position, btScalar radius, btScalar height, btVector3 color){
    btCollisionShape*       coneShape   = new btConeShape(radius, height);
    btDefaultMotionState*   coneMotion  = new btDefaultMotionState(btTransform(rotation,position));
    
    btVector3 inertia = btVector3(0,0,0);
    coneShape->calculateLocalInertia(mass, inertia);
    
    btRigidBody::btRigidBodyConstructionInfo coneRigidBodyCI(mass, coneMotion, coneShape, inertia);
    btRigidBody* cone = new btRigidBody(coneRigidBodyCI);
    
    cone->setUserPointer(new btVector3(color.x(), color.y(), color.z()));

    dynamicsWorld->addRigidBody(cone);
    return cone;
}

btRigidBody* Simulation::createSphere(btScalar mass, btQuaternion rotation, btVector3 position, btScalar radius, btVector3 color){
    btCollisionShape*       sphereShape     = new btSphereShape(radius);
    btDefaultMotionState*   sphereMotion    = new btDefaultMotionState(btTransform(rotation, position));

    btVector3 inertia = btVector3(0,0,0);
    sphereShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotion, sphereShape, inertia);
    btRigidBody* sphere = new btRigidBody(sphereRigidBodyCI);
    
    sphere->setUserPointer(new btVector3(color.x(), color.y(), color.z()));

    dynamicsWorld->addRigidBody(sphere);
    return sphere;
}

btRigidBody* Simulation::createCylinder(btScalar mass, btQuaternion rotation, btVector3 position, btVector3 halfExtents, btVector3 color, bool addToWorld){
    btCollisionShape*       cylinderShape   = new btCylinderShape(halfExtents);
    btDefaultMotionState*   cylinderMotion  = new btDefaultMotionState(btTransform(rotation, position));
    
    btVector3 inertia = btVector3(0,0,0);
    cylinderShape->calculateLocalInertia(mass, inertia);
    
    btRigidBody::btRigidBodyConstructionInfo cylinderRigidBodyCI(mass, cylinderMotion, cylinderShape, inertia);
    btRigidBody* cylinder = new btRigidBody(cylinderRigidBodyCI);
    
    cylinder->setUserPointer(new btVector3(color.x(), color.y(), color.z()));

    if(addToWorld){
        dynamicsWorld->addRigidBody(cylinder);
    }

    return cylinder;
}

/****************************************/
/*          Private Functions           */
/****************************************/
btSoftBody* Simulation::createSoftBody(){

    //FIXME this is ugly
    if(dynamicsWorld->getSoftBodyArray().size() > 0){
        btSoftBody* softbody = dynamicsWorld->getSoftBodyArray()[0];
        movementMechanism->onSoftBodyDeletion(dynamicsWorld);
        dynamicsWorld->removeSoftBody(softbody);
        delete softbody;
    }
    
        
    btSoftBody* softbody = softbodydata->generateSoftBody(&dynamicsWorld->getWorldInfo()); 
    movementMechanism->onSoftBodyCreation(dynamicsWorld, softbody);

    dynamicsWorld->addSoftBody(softbody);

    return softbody;
}

void Simulation::handleDebugString(char* debugString)
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

