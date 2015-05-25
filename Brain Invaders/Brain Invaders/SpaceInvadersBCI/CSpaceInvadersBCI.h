#ifndef __OpenViBEApplication_CSpaceInvadersBCI_H__
#define __OpenViBEApplication_CSpaceInvadersBCI_H__

#include <Ogre.h>        ////////////////////////difference
using namespace Ogre;        ////////////////////////difference
#include "../ovavrdCOgreVRApplication.h"
#include "SpaceInvadersScreen.h"
using namespace OpenViBEVRDemos;

#include "../OVComm.h"
#include <map>
#include <string>
#include <fstream>
#include <iostream>

#define SCREEN_NUM 3


namespace OpenViBEVRDemos {

	class SpaceInvadersScreen;

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief The Brain Invaders Game. Extends COgreVRApplication.
	 *
	 * \details Brain Invaders, a BCI controlled version of Space Invaders.
	 * 
	 */
	class CSpaceInvadersBCI : public COgreVRApplication
	{
	public:

		/**
		* \brief Constructor.
		* \param newProtocol Integer giving the protocol to use.
		* \param doShuffle true if we want to shuffle the flash matrices
		* \param addValues true if we want to add VRPN values for each repetition.
		*/
		CSpaceInvadersBCI(int newProtocol, bool doShuffle);

		/**
		* \brief Makes the game Exit.
		*/
		void ExitGame();

		/**
		* \brief Finishes the current game, resetting everything back to default.
		*/
		void FinishGame();

		/**
		* \brief Starts the game at a given level.
		* \param level The level the game should start in.
		*/
		void StartGame(bool Training);

		/**
		* \brief Returns the game to its Pauze screen.
		*/
		void PauseGame();

		/**
		* \brief Sets up the Loading Screen.
		* \param won Indicated whether the loading screen is applied for winning or loosing players.
		*/
		void LoadScreen(bool won);

		void LogEvent(std::string eventInfo);

		OVComm* communicationHandler;							//!< Handles the communication through the parallel port.

	private:
		/**
		* \brief Initializes the scene, camera, lights and GUI.
		* \return \em true if the scene is successfully set up.
		*/
		virtual bool initialise(void);

		/**
		* \brief Updates the active SpaceInvadersScreen.
		* \param timeSinceLastProcess The time in seconds since the last Update.
		* \param Tells whether game will continue after this frame.
		*/
		virtual bool process(double timeSinceLastProcess);

		/**
		* \brief Processes Keyboard events
		* \param evt The Keyboard Event.
		* \return True if processed correctly.
		*/
		bool CSpaceInvadersBCI::keyPressed(const OIS::KeyEvent& evt);

		/**
		* \brief Activates the given screen.
		* \param toActivate The screen to activate, as specified in the Enumeration.
		*/
		void activateScreen(int toActivate);

		/**
		* \brief Sets up all the screens.
		*/
		void loadScreens();

		enum{													//!< Enumeration of all the screens.
			GUI,
			MainGame,
			Loading
		};

		SpaceInvadersScreen* screenManager[SCREEN_NUM];			//!< All the SpaceInvadersScreens used in the game.
		int currentScreen;										//!< The currently active SpaceInvadersScreen.

		bool m_bContinue;										//!< Indicates whether to continue the game.

		bool allowShuffle;

		bool resetTimer;

		std::ofstream gameLog;
	};
};
#endif //__OpenViBEApplication_CSpaceInvadersBCI_H__
