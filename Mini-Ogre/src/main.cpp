#include <OGRE/Ogre.h>
#include "MiniKeyListener.h"
#include "MiniFrameListener.h"
#include "MiniMouseListener.h"

using namespace Ogre;

int main()
{
	Root *root = new Root("plugins.cfg","miniogre.cfg","miniogre.log");
	root->showConfigDialog();
	root->initialise(true, "Mini Ogre");
	
    // basic settings
	RenderWindow *window = root->getAutoCreatedWindow();
	SceneManager *smgr = root->createSceneManager(ST_GENERIC, "SceneManager");
	Camera *cam = smgr->createCamera("MainCamera");
	Viewport *viewport = window->addViewport(cam);
	viewport->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));

    // ------ {{{
    // control settings
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    //tell OIS about the Ogre window
    window->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    // Set up events listener
    //setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));

    //key events
    MiniKeyListener* keyListener = new MiniKeyListener();
    keyboard->setEventCallback(keyListener);
    //mouse events
    MiniMouseListener* mouseListener = new MiniMouseListener();
    mouse->setEventCallback(mouseListener);
    //render events
    MiniFrameListener* frameListener = new MiniFrameListener(keyboard, mouse);
    root->addFrameListener(frameListener); 

    keyboard->capture();
    if (keyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;
    // ------ }}}

    // main loop
	root->startRendering();

	return 0;
}
