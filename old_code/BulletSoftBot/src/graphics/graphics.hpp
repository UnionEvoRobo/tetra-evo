#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include "graphics/camera.hpp"
#include "graphics/shape_drawer.hpp"

#include "core/genetic_algorithm.hpp"

//forward declarations
class Simulation;

class Graphics{
    
    public:
        explicit Graphics(Simulation* simulation, bool stepSimulation, bool textures);
        ~Graphics();
        
        void setTitle(char* newTitle);
        
        void setSimulation(Simulation* s);

        void startSim();
        void update();

        void idleCallback();
        void displayCallback();
        void reshapeCallback(int width, int height);
        void keyboardCallback(unsigned char key, int x, int y);
        void keyboardUpCallback(unsigned char key, int x, int y);
        void passiveMotionCallback(int x, int y);
        void mouseCallback(int button, int state, int x, int y);

    private: 
        float oldElapsedTime;
        bool  stepSim;

        Camera      camera;
        ShapeDrawer shapedrawer;
        Simulation* simulation;
        
        char* title;
        
};

#endif
