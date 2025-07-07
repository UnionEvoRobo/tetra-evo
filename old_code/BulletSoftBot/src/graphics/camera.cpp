#include <GL/freeglut.h>
#include <LinearMath/btScalar.h>

#include "camera.hpp"

/****************************************/
/*          Public Functions            */
/****************************************/
Camera::Camera() : 
    xpos(0.0),
    ypos(5.0),
    zpos(50.0),
    xrot(0.0),
    yrot(0.0),
    movementSpeed(1.0),
    mouse_enabled(false){}

void Camera::reposition(){
    glRotatef(xrot,1.0,0.0,0.0);
    glRotatef(yrot,0.0,1.0,0.0);
    glTranslatef(-xpos,-ypos,-zpos);
}

void Camera::onKeyboard(unsigned char key){
    float xrotrad, yrotrad;
    
    switch(key){
        case 'q':
            ypos += movementSpeed;
            break;
        case 'z':
            ypos -= movementSpeed;

            if(ypos < 1) ypos = 1;
            break;

        case 'w':
            xrotrad = (xrot / 180 * SIMD_PI);
            yrotrad = (yrot / 180 * SIMD_PI);
            xpos += float(btSin(yrotrad)) * movementSpeed;
            zpos -= float(btCos(yrotrad)) * movementSpeed;
            ypos -= float(btSin(xrotrad)) * movementSpeed;
            
            if(ypos < 1) ypos = 1;
            break;
        case 's':
            xrotrad = (xrot / 180 * SIMD_PI);
            yrotrad = (yrot / 180 * SIMD_PI);
            xpos -= float(btSin(yrotrad)) * movementSpeed;
            zpos += float(btCos(yrotrad)) * movementSpeed;
            ypos += float(btSin(xrotrad)) * movementSpeed;
            
            if(ypos < 1) ypos = 1;
            break;
        case 'd':
            yrotrad = (yrot / 180 * SIMD_PI);
            xpos += float(btCos(yrotrad)) * movementSpeed;
            zpos += float(btSin(yrotrad)) * movementSpeed;
            break;
        case 'a':
            yrotrad = (yrot / 180 * SIMD_PI);
            xpos -= float(btCos(yrotrad)) * movementSpeed;
            zpos -= float(btSin(yrotrad)) * movementSpeed;
            break;
    }
}

void Camera::onMouseMotion(int x, int y){
    int dx = x - lastx;
    int dy = y - lasty;
    
    lastx = x;
    lasty = y;
    
    if(mouse_enabled){
        xrot += dy;
        yrot += dx;
    
        if(xrot >  90) xrot =  90;
        if(xrot < -90) xrot = -90;
    }
}

void Camera::onMouseButton(int button, int state){
    if(state == GLUT_UP){
        if(button == GLUT_RIGHT_BUTTON) mouse_enabled = !mouse_enabled;
    }
}
