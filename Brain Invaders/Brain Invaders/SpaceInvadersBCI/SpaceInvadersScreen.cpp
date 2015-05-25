#include "CSpaceInvadersBCI.h"
#include "SpaceInvadersScreen.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
using namespace Ogre;

#include <stdlib.h>

SpaceInvadersScreen::SpaceInvadersScreen(CSpaceInvadersBCI *scrMan, SceneManager *mSM)
{
	screenManager = scrMan;
	sceneManager = mSM;
}

void SpaceInvadersScreen::visible(bool visible){

}

bool SpaceInvadersScreen::update(double timeSinceLastUpdate){
	return true;
}

void SpaceInvadersScreen::keyPressed(const OIS::KeyEvent& evt){
}