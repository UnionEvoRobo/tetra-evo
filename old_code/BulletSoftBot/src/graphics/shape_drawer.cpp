#include <GL/freeglut.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>

#include "graphics/shape_drawer.hpp"
#include "graphics/texture_handler.hpp"


/****************************************/
/*          Public Functions            */
/****************************************/
void ShapeDrawer::renderBox(const btRigidBody* box){
	

	//color
    btVector3 color = *((btVector3*)box->getUserPointer());
    glColor3f(color.x(), color.y(), color.z());
    
    
    //textures
    if(texture)
    {
    	glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0); 
	
  		th.initTextures();
  	
  		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);
  		glBindTexture(GL_TEXTURE_2D, th.getTexture(3));//TODO more flexable
    }
	

    btVector3 extent = ((btBoxShape*) box->getCollisionShape())->getHalfExtentsWithoutMargin();
    
    btTransform transform;
    btScalar matrix[16];

    box->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix(matrix);

    glPushMatrix();
    glMultMatrixf(matrix);

    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-extent.x(), extent.y(),-extent.z());
    glTexCoord2f(0,1); glVertex3f(-extent.x(),-extent.y(),-extent.z());
    glTexCoord2f(1,1); glVertex3f(-extent.x(),-extent.y(), extent.z());
    glTexCoord2f(1,0); glVertex3f(-extent.x(), extent.y(), extent.z());

    glTexCoord2f(0,0); glVertex3f( extent.x(), extent.y(),-extent.z());
    glTexCoord2f(0,1); glVertex3f( extent.x(),-extent.y(),-extent.z());
    glTexCoord2f(1,1); glVertex3f( extent.x(),-extent.y(), extent.z());
    glTexCoord2f(1,0); glVertex3f( extent.x(), extent.y(), extent.z());

    glTexCoord2f(0,0); glVertex3f(-extent.x(), extent.y(), extent.z());
    glTexCoord2f(0,1);glVertex3f(-extent.x(),-extent.y(), extent.z());
    glTexCoord2f(1,1);glVertex3f( extent.x(),-extent.y(), extent.z());
    glTexCoord2f(1,0); glVertex3f( extent.x(), extent.y(), extent.z());

    glTexCoord2f(0,0); glVertex3f(-extent.x(), extent.y(),-extent.z());
    glTexCoord2f(0,1); glVertex3f(-extent.x(),-extent.y(),-extent.z());
    glTexCoord2f(1,1); glVertex3f( extent.x(),-extent.y(),-extent.z());
    glTexCoord2f(1,0); glVertex3f( extent.x(), extent.y(),-extent.z());

    glTexCoord2f(0,0); glVertex3f(-extent.x(), extent.y(),-extent.z());
    glTexCoord2f(0,1); glVertex3f(-extent.x(), extent.y(), extent.z());
    glTexCoord2f(1,1); glVertex3f( extent.x(), extent.y(), extent.z());
    glTexCoord2f(1,0); glVertex3f( extent.x(), extent.y(),-extent.z());

    glTexCoord2f(0,0); glVertex3f(-extent.x(),-extent.y(),-extent.z());
    glTexCoord2f(0,1); glVertex3f(-extent.x(),-extent.y(), extent.z());
    glTexCoord2f(1,1); glVertex3f( extent.x(),-extent.y(), extent.z());
    glTexCoord2f(1,0); glVertex3f( extent.x(),-extent.y(),-extent.z());
    glEnd();
    
	if(texture)
	{
		glDisable(GL_TEXTURE_2D);
	}
	
    glPopMatrix();
}

void ShapeDrawer::renderCone(const btRigidBody* cone){
    btVector3 color = *((btVector3*)cone->getUserPointer());
    glColor3f(color.x(), color.y(), color.z());
    
    btConeShape* coneShape = ((btConeShape*) cone->getCollisionShape());
    
    int upIndex     = coneShape->getConeUpIndex();
    btScalar radius = coneShape->getRadius();
    btScalar height = coneShape->getHeight();
    
    btTransform transform;
    btScalar matrix[16];
    
    cone->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix(matrix);
    
    glPushMatrix();
    glMultMatrixf(matrix);
    
    switch(upIndex){
        case 0: glRotatef(90.0,0.0,1.0,0.0);  break;
        case 1: glRotatef(-90.0,1.0,0.0,0.0); break;
        default: break;
    }
    
    glTranslatef(0.0,0.0,-0.5*height);
    glutSolidCone(radius,height,10,10);
    glPopMatrix();
    
}

void ShapeDrawer::renderSphere(const btRigidBody* sphere){
    btVector3 color = *((btVector3*)sphere->getUserPointer());
    glColor3f(color.x(), color.y(), color.z());
    
    btScalar radius = ((btSphereShape*) sphere->getCollisionShape())->getRadius();
    
    btTransform transform;
    btScalar matrix[16];

    sphere->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix(matrix);

    glPushMatrix();
    glMultMatrixf(matrix);
    glutSolidSphere(radius,100,100);
    glPopMatrix();
}

void ShapeDrawer::renderCylinder(const btRigidBody* cylinder){
    btVector3 color = *((btVector3*)cylinder->getUserPointer());
    glColor3f(color.x(), color.y(), color.z());
    
    btCylinderShape* cylinderShape = ((btCylinderShape*)cylinder->getCollisionShape());
    
    int upAxis      = cylinderShape->getUpAxis();
    btScalar radius = cylinderShape->getRadius();
    btScalar height = cylinderShape->getHalfExtentsWithMargin()[upAxis];
    
    btTransform transform;
    btScalar matrix[16];
    
    cylinder->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix(matrix);
    
    glPushMatrix();
    glMultMatrixf(matrix);
    
    switch (upAxis){
        case 0:
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 0.0, -height);
            break;
        case 1:
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, -height);
            break;
        case 2:
            glTranslatef(0.0, 0.0, -height);
            break;
    }
    
    GLUquadricObj *quad = gluNewQuadric();
    
    gluQuadricDrawStyle(quad, (GLenum)GLU_FILL);
    gluQuadricNormals(quad, (GLenum)GLU_SMOOTH);
    
    gluCylinder(quad, radius, radius, 2.0*height, 15, 10);
    gluQuadricOrientation(quad, (GLenum)GLU_INSIDE);
    gluDisk(quad,0,radius,15, 10);
    glTranslatef(0.0, 0.0, 2.0*height);
    glRotatef(-180.0, 0.0, 1.0, 0.0);
    gluDisk(quad,0,radius,15, 10);

    gluDeleteQuadric(quad);
    glPopMatrix();
    
}

void ShapeDrawer::renderSoftBody(const btSoftBody* softbody){

	if(texture)
	{
		th.initTextures();
	}
  	

    const btSoftBody::tFaceArray& faces(softbody->m_faces);

    for(int j = 0; j < faces.size(); ++j){
        btSoftBody::Node* node_0=faces[j].m_n[0];
        btSoftBody::Node* node_1=faces[j].m_n[1];
        btSoftBody::Node* node_2=faces[j].m_n[2];
        
        
        //set color of face based on label
        //TODO CHANGE TO TEXTURES
        switch(*((char*)faces[j].m_tag)){
            case 'a': texture ? glBindTexture(GL_TEXTURE_2D, th.getTexture(0)) : glColor3f(0.0,1.0,0.0); break;
            case 'b': texture ? glBindTexture(GL_TEXTURE_2D, th.getTexture(1)) : glColor3f(0.0,0.0,1.0); break;
            case 'c': texture ? glBindTexture(GL_TEXTURE_2D, th.getTexture(2)) : glColor3f(1.0,0.5,0.0); break;
            case 'd': texture ? glBindTexture(GL_TEXTURE_2D, th.getTexture(0)) : glColor3f(1.0,0.0,0.0); break;
            default: texture ? glBindTexture(GL_TEXTURE_2D, th.getTexture(0)) : glColor3f(0.5,0.5,0.5); break;
        }
        
        drawTriangle(node_0->m_x,node_1->m_x,node_2->m_x); 
    }
}


void ShapeDrawer::setTextures(bool ifTextures)
{
	texture = ifTextures;
}

/****************************************/
/*          Private Functions           */
/****************************************/
void ShapeDrawer::drawLine(const btVector3& p1,const btVector3& p2){
    glBegin(GL_LINES);
        glVertex3f(p1.x(), p1.y(), p1.z());
        glVertex3f(p2.x(), p2.y(), p2.z());
    glEnd();
}

void ShapeDrawer::drawTriangle(const btVector3& p1,const btVector3& p2,const btVector3& p3){
	if(texture)
	{
		glEnable(GL_TEXTURE_2D);
	}
	
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);   glVertex3f(p1.x(), p1.y(), p1.z()); 
        glTexCoord2f(1, 0);   glVertex3f(p2.x(), p2.y(), p2.z());
        glTexCoord2f(0.5, 1); glVertex3f(p3.x(), p3.y(), p3.z());
    glEnd();
    
    if(texture)
    {
    	glDisable(GL_TEXTURE_2D);
    }
    
}

