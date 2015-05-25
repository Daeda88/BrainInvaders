#ifndef __OpenViBEApplication_GUIScreen_H__
#define __OpenViBEApplication_GUIScreen_H__

#include "SpaceInvadersScreen.h"
#include "CSpaceInvadersBCI.h"
using namespace OpenViBEVRDemos;

#include <OGRE.h>
#include <CEGUI.h>
#include <OIS.h>
using namespace Ogre;

namespace OpenViBEVRDemos {

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief The GUI Screen of Brain Invaders.
	 *
	 * \details This is the Graphical User Interface of the Brain Invaders game.
	 * 
	 */
	class GUIScreen : public SpaceInvadersScreen{

	public:

		/**
		* \brief Constructor.
		* \param ScrMan CSpaceInvadersBCI managing this Screen.
		* \param mSM The Ogre::SceneManager managing all assets.
		* \param mWM The CEGUI::WindowManager managing the Window drawn in.
		* \param m_poSheet The CEGUI::Window to draw the GUI on.
		*/
		GUIScreen(CSpaceInvadersBCI *ScrMan, SceneManager *mSM, CEGUI::WindowManager *mWM, CEGUI::Window *m_poSheet);
		
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

	private:
		CEGUI::WindowManager *m_poGUIWindowManager;		//!< The CEGUI::WindowManager this GUI is drawn on.

		double elapsedTime;								//!< Time elapsed.

		bool startMenuVisible;							//!< Indicates the visibility of the start menu text.

	};
};
#endif