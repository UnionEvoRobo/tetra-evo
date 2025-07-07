#ifndef PHYSICS_SIMULATION_HPP
#define PHYSICS_SIMULATION_HPP

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

#include "core/file_handler.hpp"

//forward declarations
class SoftBodyData;
class Vibration;
class btSoftBody;
class btRigidBody;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultSoftBodySolver;
class btSoftRigidDynamicsWorld;

class Simulation{
    
    friend class Graphics;
    
    public:
         Simulation(FileHandler* fh, unsigned int WAIT_TIME, unsigned int EVAL_TIME, bool WRITE_TO_DEBUG, bool PRINT_TO_SCREEN, bool DEBUG_NODE_POSITIONS, bool DEBUG_PLACEMENT);
        ~Simulation();
        
        void toggleRunning();
        
        //changed from the original
        int getWaitTime();
        int getEvalTime();
        
        void setWaitTime(int time);
        void setEvalTime(int time);
        
        void setFlags(bool writeToDebug, bool printToScreen, bool nodePostion, bool placement);
        
        void setFH(FileHandler* fh);
        //

        void initialize(SoftBodyData* softbodydata);
        bool simulate(btScalar timeStep);
        
        btRigidBody*    createBox       (btScalar mass, btQuaternion rotation, btVector3 position, btVector3 halfExtents, btVector3 color);
        btRigidBody*    createCone      (btScalar mass, btQuaternion rotation, btVector3 position, btScalar radius, btScalar height, btVector3 color);
        btRigidBody*    createSphere    (btScalar mass, btQuaternion rotation, btVector3 position, btScalar radius, btVector3 color);
        btRigidBody*    createCylinder  (btScalar mass, btQuaternion rotation, btVector3 position, btVector3 halfExtents, btVector3 color, bool addToWorld=true);

    private:
        btSoftBody*     createSoftBody  ();
        
        int MAX_WAIT_TIME;
        int MAX_EVAL_TIME;

        bool      isRunning;
        bool      isWaiting;
        btScalar  timer;
        btVector3 initialPosition;
        
        SoftBodyData* softbodydata;
        Vibration*    movementMechanism;
        
        btBroadphaseInterface*              broadphase;
        btDefaultCollisionConfiguration*    collisionConfig;
        btCollisionDispatcher*              dispatcher;
        btConstraintSolver*                 solver;
        btDefaultSoftBodySolver*            softBodySolver;
        btSoftRigidDynamicsWorld*           dynamicsWorld;
        
        bool p_writeToDebug;
        bool p_printToScreen;
        bool p_nodePostion;
        bool p_placement;
        
        FileHandler* p_fh;
        
        void handleDebugString(char* debugString);
};

#endif
