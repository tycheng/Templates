#ifndef MINIFRAMELISTENER_H
#define MINIFRAMELISTENER_H

#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>
 
using namespace Ogre; 

class MiniFrameListener : public FrameListener 
{ 
public: 
    MiniFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse) 
    { 
        mKeyboard = keyboard; 
        mMouse = mouse; 
    } 
    // This gets called before the next frame is beeing rendered.
    bool frameStarted(const FrameEvent& evt) 
    {
        //update the input devices
        mKeyboard->capture();
        mMouse->capture();
 
        //exit if key KC_ESCAPE pressed 
        if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
            return false; 
 
        return true; 
    } 
    // This gets called at the end of a frame.
    bool frameEnded(const FrameEvent& evt) 
    { 
        return true; 
    } 
private: 
    OIS::Keyboard* mKeyboard; 
    OIS::Mouse* mMouse; 
}; 


#endif /* end of include guard: MINIFRAMELISTENER_H */

