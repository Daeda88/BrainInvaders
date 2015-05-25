#ifndef __OpenViBEApplication_SpaceInvadersScreen_H__
#define __OpenViBEApplication_SpaceInvadersScreen_H__

#include "CSpaceInvadersBCI.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>        ////////////////////////difference
#include <OIS.h>
using namespace Ogre;        ////////////////////////difference

namespace OpenViBEVRDemos {

	class CSpaceInvadersBCI;

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief Abstraction of the Screens to be drawn in Brain Invaders.
	 *
	 * \details Abstract class Drawing and Updating a screen in Brain Invaders.
	 * 
	 */
	class SpaceInvadersScreen{

	public:

		SpaceInvadersScreen(CSpaceInvadersBCI *scrMan, SceneManager *mSM);
		
		/**
		* \brief Abstraction of setting the screens visibility.
		* \param isVisible Indicates whether the screen should be visible.
		*/
		virtual void visible(bool isVisible);

		/**
		* \brief Abstraction of updating the Screen.
		* \param timeSinceLastUpdate Time in seconds since last update.
		*/
		virtual bool update(double timeSinceLastUpdate);

		/**
		* \brief Abstraction of handling keyboard events.
		* \param evt The Keyboard event to handle.
		*/
		virtual void keyPressed(const OIS::KeyEvent& evt);

	protected:
		Ogre::SceneManager* sceneManager;							//!< The sceneManager this screen has to draw on.
		OpenViBEVRDemos::CSpaceInvadersBCI* screenManager;			//!< The screenManager this screen is handled by.
		

	private:



	};

};
#endif