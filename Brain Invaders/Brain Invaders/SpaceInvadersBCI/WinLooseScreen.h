#ifndef __OpenViBEApplication_WLScreen_H__
#define __OpenViBEApplication_WLScreen_H__

#include "SpaceInvadersScreen.h"
#include "CSpaceInvadersBCI.h"
#include "GameScreen.h"
using namespace OpenViBEVRDemos;

#include <OGRE.h>
#include <CEGUI.h>
#include <OIS.h>
using namespace Ogre;

namespace OpenViBEVRDemos {

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief Screen to display whether a level was Win or Lost.
	 *
	 * \details Screen that is displayed at the end of each level and tells the player whether he was successful or failed.
	 * 
	 */
	class WinLooseScreen : public SpaceInvadersScreen{

	public:

		/**
		* \brief Constructor.
		* \param ScrMan CSpaceInvadersBCI managing this Screen.
		* \param mSM The Ogre::SceneManager managing all assets.
		* \param mWM The CEGUI::WindowManager managing the Window drawn in.
		* \param m_poSheet The CEGUI::Window to draw the GUI on.
		* \param GS The GameScreen this screen is pausing.
		*/
		WinLooseScreen(CSpaceInvadersBCI *ScrMan, SceneManager *mSM, CEGUI::WindowManager *mWM, CEGUI::Window *m_poSheet, GameScreen *GS);
		
		/**
		* \brief Sets the visibility of this screen
		* \param isVisible Indicates the visibility of the screen.
		*/
		virtual void visible(bool isVisible);

		/**
		* \brief Updates the GUI.
		* \param timeSinceLastUpdate The time in seconds since the GUI was last updated.
		*/
		virtual bool update(double timeSinceLastUpdate);
		
		/**
		* \brief Sets the screens visibility.
		* \param isVisible Indicates whether the screen should be visible.
		*/
		virtual void keyPressed(const OIS::KeyEvent& evt);

		bool winScreen;									//!< True if the previous game was won.

		Timer loadTime;									//!< Timer to ensure a minimum of screen time.

	private:
		CEGUI::WindowManager *m_poGUIWindowManager;		//!< The CEGUI::WindowManager this GUI is drawn on.

		double elapsedTime;								//!< Time elapsed.

		
		bool startedLoading;							//!< Determines whether the next level is being loaded.

		GameScreen *gameScreen;							//!< GameScreen this Screen pauses.

		bool skipScreen;

	};
};
#endif