#ifndef MINIKEYLISTENER_H
#define MINIKEYLISTENER_H

#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>
 
using namespace Ogre; 

class MiniKeyListener : public OIS::KeyListener 
{ 
public: 
    bool keyPressed(const OIS::KeyEvent& e)
    { 
        return true; 
    }
 
    bool keyReleased(const OIS::KeyEvent& e)
    { 
        return true; 
    }
};

#endif /* end of include guard: MINIKEYLISTENER_H */

