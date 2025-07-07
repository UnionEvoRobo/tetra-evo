#ifndef GRAPHICS_SHAPE_DRAWER_HPP
#define GRAPHICS_SHAPE_DRAWER_HPP


#include "graphics/texture_handler.hpp"
//forward declarations
class btVector3;
class btSoftBody;
class btRigidBody;

class ShapeDrawer{
    
    public:
        void renderBox(const btRigidBody* box);
        void renderCone(const btRigidBody* cone);
        void renderSphere(const btRigidBody* sphere);
        void renderCylinder(const btRigidBody* cylinder);
        void renderSoftBody(const btSoftBody*  softbody);
        void setTextures(bool ifTextures);
    
    private:
    
    	TextureHandler th;
    	
    	bool texture;// = false; //set to true if you want textures
    	
        void drawLine(const btVector3& p1, const btVector3& p2);
        void drawTriangle(const btVector3& p1, const btVector3& p2, const btVector3& p3);
};

#endif
