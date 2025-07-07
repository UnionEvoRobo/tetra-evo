#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "physics/movement_mechanism.hpp"
#include "physics/simulation.hpp"

#include <cstdio>

Vibration::Vibration(Simulation* simulation) : 
    ENABLED(true),
    currentMotorAngle(0.0)
{}

Vibration::~Vibration(){
    if(base) delete base;
    if(mass) delete mass;
    if(constraint) delete constraint;
}

void Vibration::onSoftBodyCreation(btSoftRigidDynamicsWorld* world, btSoftBody* softbody){
    float distance = ((softbody->m_nodes[0].m_x).distance(softbody->m_nodes[1].m_x)) / 2;
    
    base = simulation->createCylinder(1,btQuaternion(0,0,-SIMD_PI/2), btVector3(0,20,0), btVector3(1,distance,1), btVector3(1.0,0.0,0.0), false); 
    mass = simulation->createCylinder(2,btQuaternion(0,0,-SIMD_PI/2), btVector3(5,20,0), btVector3(1,1,1), btVector3(1.0,0.0,1.0), false);

    base->setCollisionFlags(4);
    mass->setCollisionFlags(4);

    constraint = new btHingeConstraint(*base, *mass, btVector3(0,0,0), btVector3(0,0,1), btVector3(0,4,0), btVector3(0,0,0));
    constraint->enableMotor(true);
    constraint->setMaxMotorImpulse(10);

    currentMotorAngle = 0.0; 
    world->addConstraint(constraint);   
    world->addRigidBody(base);
    world->addRigidBody(mass);

    softbody->appendAnchor(0,base,btVector3(0, distance,0),true,1.0);
    softbody->appendAnchor(1,base,btVector3(0,-distance,0),true,1.0);
    softbody->m_collisionDisabledObjects.push_back(mass);    
}

void Vibration::onSoftBodyDeletion(btSoftRigidDynamicsWorld* world){
    world->removeConstraint(constraint);
    world->removeCollisionObject(base);
    world->removeCollisionObject(mass);

    delete mass;
    delete base;
    delete constraint;
}

void Vibration::onTimeStep(){
    if(ENABLED){
        currentMotorAngle = (currentMotorAngle + 1);
        constraint->setMotorTarget(currentMotorAngle,1);
    }
}

void Vibration::enable(){
    ENABLED = true;
}

void Vibration::disable(){
    ENABLED = false;
}

