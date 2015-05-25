#ifndef __OpenViBEApplication_PROGBAR_H__
#define __OpenViBEApplication_PROGBAR_H__

#include <OGRE.h>
using namespace Ogre;

#include <string>
using namespace std;

#define MAXREPETITIONS 24

namespace OpenViBEVRDemos {

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief A bar to keep track of player progress in a level.
	 *
	 * \details A Bar that displays the amount of shots a player still has left during the level.
	 * 
	 */
	class ProgressBar{

	public:
		
		/**
		* \brief Constructor.
		* \param mSM Pointer to the sceneManager for drawing purposes
		*/
		ProgressBar(Ogre::SceneManager *mSM);

		/**
		* \brief Destructor
		*/
		~ProgressBar();

		/**
		* \brief Sets the visibility of the progress bar.
		* \param isVisible Determines whether the bar should become visible.
		*/
		void visible(bool isVisible);

		/**
		* \brief Tells the bar that a new repetition begins, thus adjusting what is displayed.
		* \brief validShot If true a valid shot was fired. If false, some error in the protocol created an invalid shot.
		*/
		void nextRepetition(bool validShot);

		/**
		* \brief Resets the progress bar to a specified amount of repetitions.
		* \param repetitions The amount of repetitions that the progress bar should display.
		*/
		void reset(int repetitions);

	private:

		SceneManager* sceneManager;					//<! SceneManager for the Ogre settings.
		SceneNode* barNode;							//<! Node that the progress bar elements are linked to.

		int maxRepetitions;							//<! Amount of repetitions the progress bar should display.
		int currentRepetition;						//<! Amount of repetitions in the progress bar already fired.

		Entity* barDots[MAXREPETITIONS];			//<! Target display for each repetition.

	};

};
#endif