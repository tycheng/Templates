#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include "MiniKeyListener.h"
#include "MiniFrameListener.h"
#include "MiniMouseListener.h"

using namespace Ogre;

// global variables
Root *root;
RenderWindow *window; 
SceneManager *smgr;
Camera *cam;

void initCamera()
{
    // camera settings
    cam->setPosition(Ogre::Vector3(0,10,500));
    cam->lookAt(Ogre::Vector3(0,0,0));
    cam->setNearClipDistance(5);
}

void initScene()
{
    // init light
    smgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

    // create an entity
    Ogre::Entity *ogreHead = smgr->createEntity("Head", "ogrehead.mesh");

    // // create a scene node and attach entity
    Ogre::SceneNode *headNode = smgr->getRootSceneNode()->createChildSceneNode("HeadNode");
    headNode->attachObject(ogreHead);

    // create a light and set its position
    Ogre::Light *light = smgr->createLight("MainLight");
    light->setPosition(20.0f, 80.0f, 50.0f);
}

void setupResources(const char *mResourcesCfg)
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);
 
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			// OS X does not set the working directory relative to the app,
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location
			if (!Ogre::StringUtil::startsWith(archName, "/", false)) // only adjust relative dirs
				archName = Ogre::String(Ogre::macBundlePath() + "/" + archName);
#endif
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

int main()
{
	root = new Root("plugins.cfg","miniogre.cfg","miniogre.log");
	root->showConfigDialog();
	root->initialise(true, "Mini Ogre");
	
    // basic settings
	window = root->getAutoCreatedWindow();
	smgr = root->createSceneManager(ST_GENERIC, "SceneManager");
	cam = smgr->createCamera("MainCamera");
	Viewport *viewport = window->addViewport(cam);
	viewport->setBackgroundColour(ColourValue(0.4, 0.3, 0.4));

    // ---------------- {{{ ------------------
    // control settings
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    //tell OIS about the Ogre window
    window->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if defined OIS_WIN32_PLATFORM
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
    pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif

    // Set up events listener
    //setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));

    //key events
    MiniKeyListener* keyListener = new MiniKeyListener();
    keyboard->setEventCallback(keyListener);

    // //mouse events
    MiniMouseListener* mouseListener = new MiniMouseListener();
    mouse->setEventCallback(mouseListener);

    //render events
    MiniFrameListener* frameListener = new MiniFrameListener(keyboard, mouse);
    root->addFrameListener(frameListener); 

    keyboard->capture();
    if (keyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;
    // --------------- }}} --------------------
    
    // ---------------- {{{ ------------------
    // init resources
    setupResources("./resources.cfg");

    // init Scene
    initCamera();
    initScene();
    // --------------- }}} --------------------

    // main loop
	root->startRendering();

	return 0;
}
