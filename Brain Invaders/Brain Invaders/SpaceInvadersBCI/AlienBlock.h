#ifndef __OpenViBEApplication_ALIENBLOCK_H__
#define __OpenViBEApplication_ALIENBLOCK_H__

#include "Alien.h"
using namespace OpenViBEVRDemos;

#include <OGRE.h>
using namespace Ogre;

#include <string>
using namespace std;

#define MAXBLOCKSIZE 25
#define MAXPATHSIZE 25

namespace OpenViBEVRDemos {

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief A description of Block of moving Aliens in Brain Invaders
	 *
	 * \details A Block of moving Aliens in Brain Invaders, moving through the game as a whole.
	 * 
	 */
	class AlienBlock{

	public:

		/**
		* \brief Constructor.
		* \param mSC Ogre::SceneManger the Block is added to.
		* \param path The path the block of Aliens follows.
		* \param stepSpeed The Speed the Block moves in, in seconds per step.
		* \param speedMultiplier The increase of speed as a fraction of the stepSpeed.
		* \param maxSpeed The maximum speed the block can move in, in seconds.
		*/
		AlienBlock(SceneManager *mSC, int path[MAXPATHSIZE][2], double stepSpeed, double speedMultiplier, double maxSpeed);
		
		/**
		* \brief Destructor
		*/
		~AlienBlock();

		/**
		* \brief Adds an Alien to the block.
		* \param alien The alien to be added.
		* \param relX The relative X position of the alien. Must be between 0 and MAXBLOCKSIZE.
		* \param relY The relative Y position of the alien. Must be between 0 and MAXBLOCKSIZE.
		*/
		void addAlien(Alien* alien, int relX, int relY);

		/**
		* \brief Updates the block and all its Aliens.
		* \param elapsedTime the time in seconds since the last update.
		* \return true if block was updated.
		*/
		bool updateBlock(int milliTime);

		/**
		* \brief Returns the Alien with a given name.
		* \param name The name the requested Alien needs to have.
		* \return The requested Alien or NULL if not found.
		*/
		Alien* getAlien(string name);

		/**
		* \brief Tells whether the Block is at the end of its path.
		* \return True if at end of the path.
		*/
		bool atEndOfPath();

	private:

		/**
		* \brief Makes sure all elements of the Block are set to NULL.
		*/
		void cleanBlock();

		/**
		* \brief Gets the leftmost non-empty Column of the Block.
		* \return The leftmost non-empty Column of the Block.
		*/
		int getLeftColumn();

		/**
		* \brief Gets the rightmost non-empty Column of the Block.
		* \return The rightmost non-empty Column of the Block.
		*/
		int getRightColumn();

		/**
		* \brief Gets the bottom non-empty Row of the Block.
		* \return The bottom non-empty Row of the Block.
		*/
		int getBottomRow();

		SceneManager* sceneManager;							//!< Ogre::SceneManager this Block belongs to.
		SceneNode* blockSceneNode;							//!< Ogre::SceneNode the Block is controlled by.
		Alien * blockMatrix[MAXBLOCKSIZE][MAXBLOCKSIZE];	//!< Matrix of all Aliens within this Block.

		int path[MAXPATHSIZE][2];							//!< Vector list of the path the Block moves in.

		double stepSpeed;									//!< Current speed in seconds of the block.
		double speedMultiplier;								//!< Multiplier of the speed as a fraction.
		double maxSpeed;									//!< Maximal speed the block can achieve.

		double totalElapsedTime;							//!< Total elapsed time.

		int currentPathStep;								//!< Current step of the Path.

	};

};

#endif