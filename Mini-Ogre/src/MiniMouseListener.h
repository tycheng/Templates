#ifndef MINIMOUSELISTENER_H
#define MINIMOUSELISTENER_H

#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>
 
using namespace Ogre; 

class MiniMouseListener : public OIS::MouseListener
{
public: 
    bool mouseMoved(const OIS::MouseEvent& e){ 
        return true; 
    }
 
    bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id){ 
        return true; 
    }
 
    bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id){ 
        return true; 
    }
};
 

#endif /* end of include guard: MINIMOUSELISTENER_H */

