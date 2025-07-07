#ifndef PHYSICS_MOVEMENT_MECHANISM_HPP
#define PHYSICS_MOVEMENT_MECHANISM_HPP

//forward declarations
class Simulation;
class btSoftBody;
class btRigidBody;
class btHingeConstraint;
class btSoftRigidDynamicsWorld;

class Vibration{

    public:
        explicit Vibration(Simulation* simulation);
                ~Vibration();

        void onSoftBodyCreation(btSoftRigidDynamicsWorld* world, btSoftBody* softbody);
        void onSoftBodyDeletion(btSoftRigidDynamicsWorld* world);
        void onTimeStep();

        void enable();
        void disable();

    private:
        bool  ENABLED;
        float currentMotorAngle;
        
        btRigidBody*        base;
        btRigidBody*        mass;
        btHingeConstraint*  constraint;
        Simulation*         simulation;
};

#endif
