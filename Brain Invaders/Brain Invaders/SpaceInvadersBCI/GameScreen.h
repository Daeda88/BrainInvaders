#ifndef __OpenViBEApplication_GameScreen_H__
#define __OpenViBEApplication_GameScreen_H__

#include "SpaceInvadersScreen.h"
#include "CSpaceInvadersBCI.h"
#include "Alien.h"
#include "AlienBlock.h"
#include "SoundManager.h"
#include "ProgressBar.h"
using namespace OpenViBEVRDemos;

#include <OGRE.h>
using namespace Ogre;

#define MAXMATRIXSIZE 25
#define MAXBLOCKS 36

namespace OpenViBEVRDemos {

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief The Game Screen of Brain Invaders.
	 *
	 * \details In this Game Screen, al the elements of the actual Game are managed. This is also where game logic is processed.
	 * 
	 */
	class GameScreen : public SpaceInvadersScreen{

	public:

		/**
		* \brief Constructor.
		* \param ScrMan The CSpaceInvadersBCI class managing this screen.
		* \param mSM The Ogre::SceneManager managing all elements.
		*/
		GameScreen(CSpaceInvadersBCI *ScrMan, SceneManager *mSM, CAbstractVrpnPeripheral * vrpnPeripheral, bool shuffle);

		/**
		* \brief Destructor.
		*/
		~GameScreen();

		/**
		* \brief Sets the screens visibility.
		* \param isVisible Indicates whether the screen should be visible.
		*/
		virtual void visible(bool isVisible);

		/**
		* \brief Updates the Game Screen, handling all Game logic and Managing Flashing and BCI input.
		* \return indicates whether to continue.
		*/
		virtual bool update(double timeSinceLastUpdate);

		/**
		* \brief Handles Keyboard events occuring in this screen.
		* \param evt The Keyboard Event to be processed.
		*/
		virtual void keyPressed(const OIS::KeyEvent& evt);
	
		/**
		* \brief Finishes a level and if possible loads the next one.
		*/
		void finishLevel();

		/**
		* \brief Sets the mode of the game to either regular or trainingmode.
		* \param training True if the game should run in training mode.
		*/
		void setMode(bool training);

		void resetTimer();

	private:

		/**
		* \brief Loads all the Assets this screen will need.
		*/
		void loadAssets();

		/**
		* \brief Loads a Level. See level specification for details.
		* \param The number of the level.
		* \return true if level correctly loaded.
		*/
		bool loadLevel(int level);

		/**
		* \brief Sets the flashing sequence of the Rows/columns.
		*/
		bool readFlashSequenceFile();

		/**
		* \brief Reads in the target sequence for training-mode.
		*/
		void readTargetSequenceFile();

		/**
		* \brief Cleans all the matrices of the GameScreen.
		*/
		void cleanMatrices();

		/**
		* \brief Shuffles all the Aliens in the Matrix so that they appear to flash randomly.
		*/
		void ShuffleMatrix();

		/**
		* \brief Flashes the Next Column or Row.
		*/
		void FlashNext();

		/**
		* \brief Unflashes all Flashed Aliens.
		* \return true if there is still a Row or Column to flash before a repetition has completed.
		*/
		bool UnFlash();

		/**
		* \brief  Gets the next time between a flash.
		*/
		double NextISI();

		/**
		* \brief Finds the next target in the training mode.
		* \return True if there is a next target.
		*/
		bool nextTarget();

		/**
		* \brief Processes the VRPN.
		* \return Returns if there was a signal processed.
		*/
		bool processVRPN();

		/**
		* \brief Stores the scores of a VRPN repetition in the correpsonding element of the row table.
		* \param idxVRPN Index of the VRPN column/row
		* \param value The P300-value corresponding with the index.
		*/
		void VRPN_RowColumnFctP300(int idxVRPN, double value);

		/**
		* \brief Processes the score of a flash repetition in the score list.
		*/
		void RowColumnFctP300ManageRepetitionIndex();

		/**
		* \brief Determines the most likely selected element.
		*/
		void DetermineCibleFromTabP300();

		/**
		* \brief Code for shooting a random Alien. Temp code.
		*/
		void shootAlien();

		/**
		* \brief Returns the current Time in milliseconds.
		*/
		int timeInMilliSeconds();

		/**
		* \brief Constructs the overlay (GUI) of the game screen.
		*/
		void setUpOverlay();

		Overlay * overlay;											//!< The Overlay (GUI) of the GameScreen.

		ProgressBar* progressBar;									//!< Bar that shows the progress of a level. Part of the overlay.

		Timer stateTimer;											//!< Timer used to jump to new gameplay state.
		Timer movementTimer;										//!< Timer used to move the Alien blocks.
		Timer levelTimer;

		int currentLevel;											//!< The current Level the game is at.
			
		SoundManager *soundManager;									//!< Manages the sound played by the game.

		CAbstractVrpnPeripheral * m_poVrpnPeripheral;				//!< Peripheral to handle communications through the VRPN.
		
		Alien * alienMatrix[MAXMATRIXSIZE][MAXMATRIXSIZE];			//!< Matrix of all the Aliens in the game.
		AlienBlock * alienBlocks[MAXBLOCKS];						//!< List of all the AlienBlocks in the game.

		double prevTime;											//!< previous Time Since last Update. Used to ignore the avaraging of the Ogre Timestamp.
		double elapsedTime;											//!< Elapsed time in seconds since last reset.
		double flashTime;											//!< Time in seconds a flash should last.
		double ISITime;												//!< Time in seconds the pause between flashes should last.
		double pauseTime;											//!< Time in seconds between reptetitions.

		enum state{													//!< Drawing States.
			BlackScreen,
			SendTargetInfo,
			ShowTarget,
			Flash,
			ISI,
			Pause,
			WinScreen,
			LooseScreen,
			Shoot,
			FinishTrain
		};

		std::deque<int> m_vFlashSequence;							//!< The sequence in which to flash rows and columns

		std::deque<std::pair<int,int> > m_vTargetSequence;			//!< Target sequence for training mode.
		std::pair<int,int> targetAlien;

		Alien* toShoot;												//!< Alien to shoot at the end of a repetition.
		double m_dLastP300Maximum;									//!< Previous maximum of P300 repetition.
		std::vector<double> m_vdTabRowColumnP300;					//!< List of P300 respone for each column/row for a repetition.
		bool m_bResetTabP300;										//!< Determines whether the P300 values should be added to the previous one. For optimizing detection
		std::vector<double> m_vTabP300;								//!< List of scores used to determine the selected item.

		int repetition;												//!< Current repetiton/attempts within level
		int maxRepetitions;											//!< Max repetitions before level fail.

		int score;													//!< Current Player score.

		int currentState;											//!< Current drawing state.
		//bool doRow;													//!< Should Flash Row or Column.
		int currentlyAt;											//!< Current Row/Column to Flash.
		int flashesSinceLastRepetition;

		bool isLoading;												//!< Indicates whether game is loading something.

		bool trainingMode;											//!< True if in training-mode.

		bool allowShuffle;
	};
};
#endif