#ifndef GRAPHICS_CAMERA_HPP
#define GRAPHICS_CAMERA_HPP

class Camera{
    
    public:
        Camera();
    
        void reposition();
    
        void onKeyboard(unsigned char key);
        void onMouseMotion(int x, int y);
        void onMouseButton(int button, int state);
        
    private:
        float xpos, ypos, zpos;
        float xrot, yrot;
        float movementSpeed;
        
        int lastx;
        int lasty;
        
        bool mouse_enabled;
};

#endif
