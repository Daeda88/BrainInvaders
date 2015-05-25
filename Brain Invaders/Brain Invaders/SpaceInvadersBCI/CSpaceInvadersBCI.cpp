#include "CSpaceInvadersBCI.h"
#include "GUIScreen.h"
#include "GameScreen.h"
#include "WinLooseScreen.h"
#include "SpaceInvadersScreen.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
using namespace Ogre;


// Parallele port installation 
#include "../OVComm.h"
#include <stdio.h>
#include <stdlib.h> 
#include <windows.h>

CSpaceInvadersBCI::CSpaceInvadersBCI(int newProtocol, bool doShuffle) : COgreVRApplication()
{
	// Set the correct protocol for the ParallelPort
	if (newProtocol == EightbitPP)
		communicationHandler = new OVComm(EightbitPP);
	else if (newProtocol == SoftwareTagging)
		communicationHandler = new OVComm(SoftwareTagging);
	else
		communicationHandler = new OVComm(TwobitPP);
	allowShuffle = doShuffle;
	resetTimer = true;
	gameLog.open("GameSession.txt");

}

void CSpaceInvadersBCI::ExitGame(){
	gameLog.close();
	m_bContinue = false;
}

void CSpaceInvadersBCI::StartGame(bool Training){
	// If we start the training mode, change the gameMode.
	if (Training){
		((GameScreen *)screenManager[MainGame])->setMode(true);
		LogEvent("StartTraining");
	}
	else{
		if (resetTimer)
			((GameScreen *)screenManager[MainGame])->resetTimer();
		resetTimer = false;
		LogEvent("Start Game");
	}
	// Start the game
	activateScreen(MainGame);
}

void CSpaceInvadersBCI::PauseGame(){
	// Pausing equals just showing the main menu.
	activateScreen(GUI);
}

void CSpaceInvadersBCI::FinishGame(){
	// Reset the game to regular mode.
	((GameScreen *)screenManager[MainGame])->setMode(false);
	LogEvent("Game Finished");
	// We want to see the Main Menu at the end of the game.
	activateScreen(GUI);
}

void CSpaceInvadersBCI::LoadScreen(bool won){
	// Show a load screen, which varies based on whether the previous level was won or not.
	((WinLooseScreen *)screenManager[Loading])->winScreen = won;
	activateScreen(Loading);
}

// load 3D scene
bool CSpaceInvadersBCI::initialise()
{
	printf("\n\nSPACE INVADERS\n\n");

	//Adjust Resolution
	// lights
	m_poSceneManager->setAmbientLight(ColourValue(1, 1, 1));

	Ogre::Light* l_poLight1 = m_poSceneManager->createLight("Light1");
	l_poLight1->setType(Light::LT_POINT);
	l_poLight1->setPosition(Vector3(250, 150, 250));
	l_poLight1->setSpecularColour(ColourValue::White);
	l_poLight1->setDiffuseColour(ColourValue::White);

	// camera
	m_poCamera->setNearClipDistance(5);
	m_poCamera->setProjectionType(PT_ORTHOGRAPHIC);
	m_poCamera->setPosition(Vector3(625, -450, 200));
	m_poCamera->setDirection(Vector3(0,0,-1));
	loadScreens();
	activateScreen(GUI);

	// Make sure the parallel Port is not sending any data other than 0
	communicationHandler->SetFinalized();

	m_bContinue = true;

	return true;
}

void CSpaceInvadersBCI::loadScreens(){
	// Create all the screens used by the game.
	GUIScreen* gui = new GUIScreen(this, m_poSceneManager, m_poGUIWindowManager, m_poSheet);
	screenManager[GUI] = gui;
	
	GameScreen* game = new GameScreen(this, m_poSceneManager, m_poVrpnPeripheral, allowShuffle);
	screenManager[MainGame] = game;

	WinLooseScreen* winLoose = new WinLooseScreen(this, m_poSceneManager, m_poGUIWindowManager, m_poSheet, game);
	screenManager[Loading] = winLoose;
}

void CSpaceInvadersBCI::activateScreen(int toActivate){
	// Hide all the screens except for the one specified.
	for (int i = 0; i < SCREEN_NUM; i++){
		if (i == toActivate)
			screenManager[i]->visible(true);
		else
			screenManager[i]->visible(false);
	}
	currentScreen = toActivate;
}

bool CSpaceInvadersBCI::keyPressed(const OIS::KeyEvent& evt)
{
	// Send key event to the active screen.
	screenManager[currentScreen]->keyPressed(evt);
	return true;
}


// called by OV Ogre engine
bool CSpaceInvadersBCI::process(double timeSinceLastProcess)
{
	// Update the current screen.
	screenManager[currentScreen]->update(timeSinceLastProcess);
	return m_bContinue;
}

void CSpaceInvadersBCI::LogEvent(std::string eventInfo){
	gameLog << eventInfo << endl;
}
