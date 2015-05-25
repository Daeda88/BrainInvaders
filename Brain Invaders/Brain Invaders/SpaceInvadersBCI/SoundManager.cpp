#include "SoundManager.h"
using namespace OpenViBEVRDemos;

#include <al.h>
#include <alc.h>
#include <AL/alut.h>
#include <stdio.h>
#include <iostream>

SoundManager::SoundManager(){
	std::cout<<"Sound Manager Constructor"<<std::endl<<std::endl<<std::endl<<std::endl;
	// Initialisation of the alut stuff. See their documentation for full explanation.
	currentSound = 0;

	alutInit(0, NULL);

	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Initialize"<<std::endl;

	// We dont do 3d sounds, so just place everything at 0.0.0
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	ALfloat listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};
	alListenerfv(AL_ORIENTATION,listenerOri);


	alGenBuffers(NUMBERSOUNDS, buffer);

	// Load in all sounds.
	buffer[move0] = alutCreateBufferFromFile("Resources/game/Sounds/move0.wav");
	buffer[move1] = alutCreateBufferFromFile("Resources/game/Sounds/move1.wav");
	buffer[move2] = alutCreateBufferFromFile("Resources/game/Sounds/move2.wav");
	buffer[move3] = alutCreateBufferFromFile("Resources/game/Sounds/move3.wav");

	buffer[explosion] = alutCreateBufferFromFile("Resources/game/Sounds/explosion.wav");
	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Buff Fill"<<std::endl;

	alGenSources(NUMBERSOUNDS, source);
	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Source Generator"<<std::endl;

	ALfloat sourcePosition[] = {0.0, 0.0, 0.0};
	ALfloat sourceVelocity[] = {0.0, 0.0, 0.0};

	// Set the source for all sounds.
	alSourcef(source[move0],AL_PITCH,1.0f);
    alSourcef(source[move0],AL_GAIN,1.0f);
    alSourcefv(source[move0],AL_POSITION,sourcePosition);
    alSourcefv(source[move0],AL_VELOCITY,sourceVelocity);
    alSourcei(source[move0],AL_BUFFER,buffer[move0]);
    alSourcei(source[move0],AL_LOOPING,AL_FALSE);
	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Move0"<<std::endl;

    alSourcef(source[move1],AL_PITCH,1.0f);
    alSourcef(source[move1],AL_GAIN,1.0f);
    alSourcefv(source[move1],AL_POSITION,sourcePosition);
    alSourcefv(source[move1],AL_VELOCITY,sourceVelocity);
    alSourcei(source[move1],AL_BUFFER,buffer[move1]);
    alSourcei(source[move1],AL_LOOPING,AL_FALSE);
	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Move1"<<std::endl;

    alSourcef(source[move2],AL_PITCH,1.0f);
    alSourcef(source[move2],AL_GAIN,1.0f);
    alSourcefv(source[move2],AL_POSITION,sourcePosition);
    alSourcefv(source[move2],AL_VELOCITY,sourceVelocity);
    alSourcei(source[move2],AL_BUFFER,buffer[move2]);
    alSourcei(source[move2],AL_LOOPING,AL_FALSE);
	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Move2"<<std::endl;

	alSourcef(source[move3],AL_PITCH,1.0f);
    alSourcef(source[move3],AL_GAIN,1.0f);
    alSourcefv(source[move3],AL_POSITION,sourcePosition);
    alSourcefv(source[move3],AL_VELOCITY,sourceVelocity);
    alSourcei(source[move3],AL_BUFFER,buffer[move3]);
    alSourcei(source[move3],AL_LOOPING,AL_FALSE);
	if(alGetError() != AL_NO_ERROR) 
    	std::cout<<"Error at Move3"<<std::endl;

	alSourcef(source[explosion],AL_PITCH,1.0f);
    alSourcef(source[explosion],AL_GAIN,1.0f);
    alSourcefv(source[explosion],AL_POSITION,sourcePosition);
    alSourcefv(source[explosion],AL_VELOCITY,sourceVelocity);
    alSourcei(source[explosion],AL_BUFFER,buffer[explosion]);
    alSourcei(source[explosion],AL_LOOPING,AL_FALSE);
}

SoundManager::~SoundManager(){
	// Delete all sounds and exit.
	for (int i = 0; i < NUMBERSOUNDS; i++)
		alSourceStop(source[i]);
	alutExit();
}

void SoundManager::move(){
	// Play the next move sound.
	alSourcePlay(source[currentSound]);
	// We currently loop between four sounds.
	currentSound = (currentSound + 1) % 4;
}

void SoundManager::explode(){
	// Play explosion sound.
	alSourcePlay(source[explosion]);
}
