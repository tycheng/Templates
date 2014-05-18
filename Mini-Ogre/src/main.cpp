#include <OGRE/Ogre.h>

using namespace Ogre;

int main()
{
	Root *root = new Root("plugins.cfg","miniogre.cfg","miniogre.log");
	root->showConfigDialog();
	root->initialise(true, "Hello World!");
	
	RenderWindow *window = root->getAutoCreatedWindow();

	SceneManager *smgr = root->createSceneManager(ST_GENERIC, "SceneManager");

	Camera *cam = smgr->createCamera("MainCamera");

	Viewport *vp = window->addViewport(cam);
	vp->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));

	root->startRendering();

	return 0;
}
