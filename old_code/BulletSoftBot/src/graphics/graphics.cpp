#include <GL/freeglut.h>
#include <btBulletCollisionCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "physics/simulation.hpp"
#include "core/genetic_algorithm.hpp"
#include "graphics/graphics.hpp"

/****************************************/
/*      GLUT C++ Callback Wrappers      */
/****************************************/
namespace glut{

Graphics* graphics;

void idleCallback(){
    graphics->idleCallback();
}
void displayCallback(){
    graphics->displayCallback();
}
void reshapeCallback(int width, int height){
    graphics->reshapeCallback(width, height);
}
void keyboardCallback(unsigned char key, int x, int y){
    graphics->keyboardCallback(key, x, y);
}
void keyboardUpCallback(unsigned char key, int x, int y){
    graphics->keyboardUpCallback(key, x, y);
}
void passiveMotionCallback(int x, int y){
    graphics->passiveMotionCallback(x, y);
}
void mouseCallback(int button, int state, int x, int y){
    graphics->mouseCallback(button, state, x, y);
}

} // end of glut namespace

/****************************************/
/*          Public Functions            */
/****************************************/
Graphics::Graphics(Simulation* simulation, bool stepSimulation, bool textures) : oldElapsedTime(0.0), simulation(simulation) {
    glut::graphics = this;
    
    stepSim = stepSimulation;
    shapedrawer.setTextures(textures);

    int         argc = 1;
    const char* argv = "./bulletSoftBot";

    glutInit(&argc,(char**)&argv); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(840,480);
    
    title = (char *)malloc(100 * sizeof(char));//does this cause a memory leak??
    
    sprintf(title, "Bullet Soft Bot");

    glutCreateWindow(title);
    
    glClearColor(0,0,0,1);
 //   glEnable(GL_CULL_FACE); //FIXME
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);/////does this make colors
    glShadeModel(GL_SMOOTH);////shader?

    glutIdleFunc(glut::idleCallback);
    glutDisplayFunc(glut::displayCallback);
    glutReshapeFunc(glut::reshapeCallback);
    glutKeyboardFunc(glut::keyboardCallback);
    glutKeyboardUpFunc(glut::keyboardUpCallback);
    glutPassiveMotionFunc(glut::passiveMotionCallback);
    glutMouseFunc(glut::mouseCallback);
    
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    // glutMainLoop();
}

Graphics::~Graphics() {
    glutLeaveMainLoop();
}

void Graphics::setTitle(char* newTitle) {
    glutSetWindowTitle(newTitle);
}

void Graphics::setSimulation(Simulation* s) {
    simulation = s;
}

void Graphics::startSim() {
    glutMainLoop(); //runs the graphics loop continuously
}

void Graphics::update() {
    glutMainLoopEvent(); //run the graphics loop once to display what the simulation is currently doing
    displayCallback(); //because for some strange reason, it doesn't execute every time you call glutMainLoopEvent.  calling gludPostRedisplay from the idle callback doesn't help either :/
}

void Graphics::idleCallback(){
    // float newElapsedTime    = glutGet(GLUT_ELAPSED_TIME) / 500.0;
    // float deltaTime         = newElapsedTime - oldElapsedTime;
    // oldElapsedTime          = newElapsedTime;
    
    /* Commented out because we no longer want the graphics to be in charge of stepping the simulation
    float deltaTime = 0.1;
    oldElapsedTime += deltaTime;
    if (!stepSim)
    {
    	if(simulation->simulate(deltaTime)){ 
            glutLeaveMainLoop();
    	}else{
        	glutPostRedisplay();
    	}
    }*/
    
}
    
void Graphics::displayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camera.reposition();

    btCollisionObjectArray objects = simulation->dynamicsWorld->getCollisionObjectArray();
    
    for (int i = objects.size()-1; i >= 0; --i){
        btCollisionObject* obj = objects[i];

        int shapeType = obj->getCollisionShape()->getShapeType();

        switch(shapeType){
            case BOX_SHAPE_PROXYTYPE      : shapedrawer.renderBox((btRigidBody*)obj);      break;
            case CONE_SHAPE_PROXYTYPE     : shapedrawer.renderCone((btRigidBody*)obj);     break;
            case SPHERE_SHAPE_PROXYTYPE   : shapedrawer.renderSphere((btRigidBody*)obj);   break;
            case CYLINDER_SHAPE_PROXYTYPE : shapedrawer.renderCylinder((btRigidBody*)obj); break;
            default                       : shapedrawer.renderSoftBody((btSoftBody*)obj);  break;
        }
    }
    
    glFlush();
    glutSwapBuffers();
}

void Graphics::reshapeCallback(int width, int height){
    height = (height == 0 ? 1 : height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(45, float(width)/float(height), 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void Graphics::keyboardCallback(unsigned char key, int x, int y){
    camera.onKeyboard(key);
}

void Graphics::keyboardUpCallback(unsigned char key, int x, int y){
    switch(key){
        case 'p':
            simulation->toggleRunning();
            break;
        case ' ':
            //TODO manual expansion of softbody
            break;
        case 27 : //escape key
            exit(EXIT_SUCCESS);
            break;
        case 's':
        {
        	if (stepSim)
    		{
    			if(simulation->simulate(0.1)){ //TODO check if this actually evaluates correctly :)
        			glutLeaveMainLoop();
    			}else{
        			glutPostRedisplay();
    			}
    		}
        }
    }
}

void Graphics::passiveMotionCallback(int x, int y){
    camera.onMouseMotion(x,y);
}

void Graphics::mouseCallback(int button, int state, int x, int y){
    camera.onMouseButton(button,state);
}   
