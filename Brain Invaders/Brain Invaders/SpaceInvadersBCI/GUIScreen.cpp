#include "GUIScreen.h"
#include "CSpaceInvadersBCI.h"
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

GUIScreen::GUIScreen(CSpaceInvadersBCI *ScrMan, SceneManager *mSM, CEGUI::WindowManager *mWM, CEGUI::Window *m_poSheet) : SpaceInvadersScreen(ScrMan, mSM)
{
	m_poGUIWindowManager = mWM;
	
	const std::string l_sFondMenu = "Resources/GUI/brainInvadersMain.png";
	const std::string l_sTextMenu = "Resources/GUI/startText.png";

	//----------- CREATE WINDOWS -------------//
	// background window
	CEGUI::Window * l_poFondMenu  = m_poGUIWindowManager->createWindow("TaharezLook/StaticImage", "FondMenu");
	l_poFondMenu->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)) );
	l_poFondMenu->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, 0.f), CEGUI::UDim(1.0f, 0.f)));
	m_poSheet->addChildWindow(l_poFondMenu);	
	CEGUI::ImagesetManager::getSingleton().createFromImageFile("ImageFondMenu",l_sFondMenu); 
	l_poFondMenu->setProperty("Image","set:ImageFondMenu image:full_image");
	l_poFondMenu->setProperty("FrameEnabled","False");
	l_poFondMenu->setProperty("BackgroundEnabled","False");

	// text menu 2
	CEGUI::Window * l_poTextMenu  = m_poGUIWindowManager->createWindow("TaharezLook/StaticImage", "TextMenu");
	l_poTextMenu->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim(0.8f)) );
	l_poTextMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0.5f, 0.f), CEGUI::UDim(0.1f, 0.f)));
	m_poSheet->addChildWindow(l_poTextMenu);	
	CEGUI::ImagesetManager::getSingleton().createFromImageFile("ImageTextMenu",l_sTextMenu); 
	l_poTextMenu->setProperty("Image","set:ImageTextMenu image:full_image");
	l_poTextMenu->setProperty("FrameEnabled","False");
	l_poTextMenu->setProperty("BackgroundEnabled","False");
}



void GUIScreen::visible(bool visible){
	startMenuVisible = true;
	elapsedTime = 0;
	m_poGUIWindowManager->getWindow("FondMenu")->setVisible(visible);
	m_poGUIWindowManager->getWindow("TextMenu")->setVisible(visible);
}

bool GUIScreen::update(double timeSinceLastUpdate){
	elapsedTime += timeSinceLastUpdate;
	// Lets blink the TextMenu. Gives it a more "gamey" feel.
	if(elapsedTime > 0.8){
		elapsedTime = 0;
		startMenuVisible = !startMenuVisible;
		m_poGUIWindowManager->getWindow("TextMenu")->setVisible(startMenuVisible);
	}
	return true;
}

void GUIScreen::keyPressed(const OIS::KeyEvent& evt){
	if ( evt.key == OIS::KC_ESCAPE){
		// Exit the game completely at escape.
		screenManager->ExitGame();
	}
	if ( evt.key == OIS::KC_SPACE){
		// Space Starts a new Game.
		screenManager->StartGame(false);
	}
	if ( evt.key == OIS::KC_P){
		// The P starts the Practice round.
		screenManager->StartGame(true);
	}
}