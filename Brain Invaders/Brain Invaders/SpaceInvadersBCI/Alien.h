#ifndef __OpenViBEApplication_ALIEN_H__
#define __OpenViBEApplication_ALIEN_H__

#include <OGRE.h>
using namespace Ogre;

#include <string>
using namespace std;

namespace OpenViBEVRDemos {


	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief A description of a single Alien in Brain Invaders
	 *
	 * \details A single Alien Entity in Brain Invaders, handling animation and flashing.
	 * 
	 */
	class Alien{

	public:

		/**
		* \brief Constructor.
		* \param mSM The the Ogre::SceneManager this alien should be added to.
		* \param name The name the alien will have (must be unique).
		* \param type of the Alien. As specified in AlienType.
		*/
		Alien(SceneManager *mSM, string name, int type);

		/**
		* \brief Destructor.
		*/
		~Alien();

		/**
		* \brief Adds Alien to SceneNode
		* \param pSN The SceneNode the Alien has to be added to.
		*/
		void setSceneNode(SceneNode *pSN);

		/**
		* \brief Sets the Alien to its Show Target phase.
		*/
		void ShowTargetMode();

		/**
		* \brief Sets the Alien to its Flashing state.
		*/
		void Flash();

		/**
		* \brief Sets the Alien to its Unflashed state.
		*/
		void UnFlash();

		/**
		* \Brief sets the Alien to its next animated state.
		*/
		void animate();

		/**
		* \brief sets the Aliens visibility.
		* \param visible Indicates whether Alien should be visible.
		*/
		void setVisible(bool visible);

		/**
		* \brief destroys the Alien
		* \return The points earned by killing the Alien.
		*/
		int destroy();

		/**
		* \brief Gives the type of this Alien
		* \return Type of the Alien.
		*/
		int getType();

		/**
		* \brief Sets this Alien to be of a Target type.
		*/
		void makeTarget(bool isTarget);
		
		string name;							//!< The name of the Alien.

		bool alive;								//!< Indicates the Alien has died.

		const enum AlienType{					//!< Enummeration of all the different types of Aliens.
			Regular,
			RegularStrong,
			RegularStrongest,
			Target,
			Distractor,
			Distractor2
		};

	private:

		/**
		* \brief Contructs a string that indicates the current Material Name
		* \return The name of the material
		*/
		string getMaterialName();

		Vector3 scale;							//!< The scale of the Alien Mesh, stored so resizing is possible.

		int type;								//!< The type of the Alien.
		int realType;
		int frame;								//!< The frame of the Alien animation.
		Entity* alienEntity;					//!< The Ogre::Entity of the Alien.
		SceneNode * alienNode;					//!< The Ogre::SceneNode of the Alien.

		bool flashing;							//!< Indicates whether currently flashing;

	};

};
#endif