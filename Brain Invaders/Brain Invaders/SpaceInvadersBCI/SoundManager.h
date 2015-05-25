#ifndef __OpenViBEApplication_SoundManager_H__
#define __OpenViBEApplication_SoundManager_H__

#pragma comment(lib,"openal32.lib")
#pragma comment(lib,"alut.lib")

#include <al.h>
#include <alc.h>

#define NUMBERSOUNDS 6

namespace OpenViBEVRDemos {

	/*!
	 * \author Gijs van Veen (Gipsa)
	 * \date 2012-03-19
	 * \brief Manages all sounds in Brain Invaders
	 *
	 * \details Class that loads and plays all sounds used by Brain Invaders.
	 * 
	 */
	class SoundManager{

	public:
		/**
		* \brief Contructor.
		*/
		SoundManager();

		/**
		* \brief Destructor.
		*/
		~SoundManager();

		/**
		* \brief Plays the next movement sound
		*/
		void move();

		/**
		* \brief Plays the explosion sound.
		*/
		void explode();

	private:

		int currentSound;					//!< Keeps track of which movement sound has been played.

		ALCdevice* device;					//!< Audio Device.
		ALCcontext* context;				//!< Audio Context.

		ALuint source[NUMBERSOUNDS];		//!< All the sounds that can be played.
		ALuint buffer[NUMBERSOUNDS];		//!< Sound Buffers.

		ALsizei size,freq;					//!< Some settings to configure sound manager.
		ALenum 	format;
		ALvoid 	*data;

		enum{								//!< List of sounds present.
			move0,
			move1,
			move2,
			move3,
			mothership,
			explosion
		};

	};

};

#endif