#include "WinLooseScreen.h"
#include "CSpaceInvadersBCI.h"
#include "GameScreen.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include <CEGUI.h>
#include <OIS.h>
using namespace Ogre;

#include <stdlib.h>
#include <stdlib.h> 
//#include <windows.h>

WinLooseScreen::WinLooseScreen(CSpaceInvadersBCI *ScrMan, SceneManager *mSM, CEGUI::WindowManager *mWM, CEGUI::Window *m_poSheet, GameScreen *GS) : SpaceInvadersScreen(ScrMan, mSM)
{
	m_poGUIWindowManager = mWM;
	gameScreen = GS;

	const std::string l_sWonMenu = "Resources/GUI/brainInvadersWon.png";
	const std::string l_sLostMenu = "Resources/GUI/brainInvadersLost.png";

	//----------- CREATE WINDOWS -------------//
	// background Won
	CEGUI::Window * l_poWonMenu  = m_poGUIWindowManager->createWindow("TaharezLook/StaticImage", "WonMenu");
	l_poWonMenu->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)) );
	l_poWonMenu->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, 0.f), CEGUI::UDim(1.0f, 0.f)));
	m_poSheet->addChildWindow(l_poWonMenu);	
	CEGUI::ImagesetManager::getSingleton().createFromImageFile("ImageWonMenu",l_sWonMenu); 
	l_poWonMenu->setProperty("Image","set:ImageWonMenu image:full_image");
	l_poWonMenu->setProperty("FrameEnabled","False");
	l_poWonMenu->setProperty("BackgroundEnabled","False");

	// background Lost
	CEGUI::Window * l_poLostMenu  = m_poGUIWindowManager->createWindow("TaharezLook/StaticImage", "LostMenu");
	l_poLostMenu->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)) );
	l_poLostMenu->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, 0.f), CEGUI::UDim(1.0f, 0.f)));
	m_poSheet->addChildWindow(l_poLostMenu);	
	CEGUI::ImagesetManager::getSingleton().createFromImageFile("ImageLostMenu",l_sLostMenu); 
	l_poLostMenu->setProperty("Image","set:ImageLostMenu image:full_image");
	l_poLostMenu->setProperty("FrameEnabled","False");
	l_poLostMenu->setProperty("BackgroundEnabled","False");
}



void WinLooseScreen::visible(bool visible){
	// If this is a Win screen, show the win version
	m_poGUIWindowManager->getWindow("WonMenu")->setVisible(visible && winScreen);
	// If not, show the loose version.
	m_poGUIWindowManager->getWindow("LostMenu")->setVisible(visible && !winScreen);
	loadTime.reset();
	startedLoading = false;
	skipScreen = false;
}

bool WinLooseScreen::update(double timeSinceLastUpdate){
	// Start loading the level.
	if (!startedLoading){
		startedLoading = true;
		gameScreen->finishLevel();
	}
	// Once done, wait if the screen has not been displayed long enough (so players can relax)
	if (loadTime.getMilliseconds() > 10000 || skipScreen){
		startedLoading = false;
		skipScreen = false;
		screenManager->StartGame(0);
	}
	return true;
}

void WinLooseScreen::keyPressed(const OIS::KeyEvent& evt){
	if ( evt.key == OIS::KC_SPACE){
		// If space is hit during loading (and we are done) we start
		skipScreen = true;
	}
}