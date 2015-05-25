#include "AlienBlock.h"
#include "Alien.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>        ////////////////////////difference
using namespace Ogre; 

#include <string>
#include <sstream>
using namespace std;

AlienBlock::AlienBlock(SceneManager *mSC, int pPath[MAXPATHSIZE][2], double pStepSpeed, double pSpeedMultiplier, double pMaxSpeed){
	sceneManager = mSC;
	// Make sure we have a perfectly clean matrix.
	cleanBlock();
	for(int i = 0; i < MAXPATHSIZE; i++){
		path[i][0] = pPath[i][0];
		path[i][1] = pPath[i][1];
	}
	stepSpeed = pStepSpeed;
	speedMultiplier = pSpeedMultiplier;
	maxSpeed = pMaxSpeed;
	totalElapsedTime = 0;
	currentPathStep = 1;
	std::stringstream ss;
	// There can only be one Alien on any starting place, so this seems like a good Node Name.
	ss << "BlockNode" << path[0][0] << ":" << path[0][1];
	string name = ss.str();
	blockSceneNode = sceneManager->getRootSceneNode()->createChildSceneNode(name, Ogre::Vector3(0,0,0));
	blockSceneNode->translate(path[0][0],path[0][1],0);
}
AlienBlock::~AlienBlock(){
	// Make sure all Aliens are deleted first
	for(int i = 0; i < MAXBLOCKSIZE; i++){
		for(int j = 0; j < MAXBLOCKSIZE; j++){
			if (blockMatrix[i][j] != NULL){
				delete blockMatrix[i][j];
			}
		}
	}
	// Delete all attached objects.
	blockSceneNode->removeAndDestroyAllChildren();
	sceneManager->destroySceneNode(blockSceneNode);
	//blockSceneNode->
	//delete blockSceneNode;
}

void AlienBlock::cleanBlock(){
	// For all elements of the block set the value to NULL.
	for(int i = 0; i < MAXBLOCKSIZE; i++)
		for(int j = 0; j < MAXBLOCKSIZE; j++)
			blockMatrix[i][j] = NULL;
}
void AlienBlock::addAlien(Alien* alien, int relX, int relY){
	// If accidental double declaration on this point, delete
	if (blockMatrix[relX][relY] != NULL)
		delete blockMatrix[relX][relY];
	// Place the alien within the block.
	alien->setSceneNode(
		blockSceneNode->createChildSceneNode(alien->name, Ogre::Vector3(relX * 150, - relY * 150, 0))
	);
	blockMatrix[relX][relY] = alien;
}
bool AlienBlock::updateBlock(int milliTime){

	// Only move if enough time has passed and there still is a direction to go
	if (milliTime> 1000*stepSpeed && !(atEndOfPath())){

		int rightCol = getRightColumn();
		int leftCol = getLeftColumn();
		//int bottomRow = getBottomRow();

		// This case should not happen with properly declared blocks, but better safe than sorry
		if (rightCol == -1 || leftCol == -1)
			return false;

		// Since we want the absolute position, calculate it
		int currentX = blockSceneNode->getPosition().x;
		int currentY = blockSceneNode->getPosition().y;

		// Check if at the end of the path. Since block size can varry over time, this is a relatively complex if-statement.
		if(((path[currentPathStep][0] - path[currentPathStep-1][0] >= 0 && currentX + rightCol*150 >= path[currentPathStep][0])
			|| (path[currentPathStep][0] - path[currentPathStep-1][0] <= 0 && currentX + leftCol*150 <= path[currentPathStep][0]))
			&& ((path[currentPathStep][1] - path[currentPathStep-1][1] <= 0 && currentY <= path[currentPathStep][1]) 
			|| (path[currentPathStep][1] - path[currentPathStep-1][1] >= 0 && currentY >= path[currentPathStep][1]))){
			
			// Immediately take the next step.
			currentPathStep++;
			// If there is no next step, we stop moving.
			if(atEndOfPath())
				return false;
		}
		// Translate according to the direction of the path.
		if(path[currentPathStep][0] - path[currentPathStep-1][0] < 0){
			blockSceneNode->translate(-25, 0, 0);
		}else if(path[currentPathStep][0] - path[currentPathStep-1][0] > 0){
			blockSceneNode->translate(25,0,0);
		}else if(path[currentPathStep][1] - path[currentPathStep - 1][1] < 0){
			blockSceneNode->translate(0,-25,0);
		}else
			blockSceneNode->translate(0,25,0);
	}
	// Animate all aliens within block.
	if (milliTime> 1000*stepSpeed){
		for(int i = 0; i < MAXBLOCKSIZE; i++){
			for(int j = 0; j < MAXBLOCKSIZE; j++){
				if (blockMatrix[i][j] != NULL)
					blockMatrix[i][j]->animate();
			}
		}
		// Update the stepping speed.
		stepSpeed = stepSpeed * speedMultiplier;
		if (stepSpeed < maxSpeed)
			stepSpeed = maxSpeed;
		return true;
	}
	return false;
}

bool AlienBlock::atEndOfPath(){
	return path[currentPathStep][0] == -1 && path[currentPathStep][1] == -1;
}

int AlienBlock::getLeftColumn(){
	// Approaching from the left, find the first column that contains at least one element.
	for(int i = 0; i < MAXBLOCKSIZE; i++){
		for(int j = 0; j < MAXBLOCKSIZE; j++){
			if (blockMatrix[i][j] != NULL)
				return i;
		}
	}
	// Should never be returned as there should never be an empty matrix, but better be sure.
	return -1;
}

int AlienBlock::getRightColumn(){
	// Approaching from the right, find the first column that contains at least one element.
	for(int i = MAXBLOCKSIZE-1; i >= 0; i--){
		for(int j = 0; j < MAXBLOCKSIZE; j++){
			if (blockMatrix[i][j] != NULL)
				return i;
		}
	}
	// Should never be returned as there should never be an empty matrix, but better be sure.
	return -1;
}

int AlienBlock::getBottomRow(){
	// Approaching from the bottom, find the first row that contains at least one element.
	for(int i = MAXBLOCKSIZE-1; i >= 0; i--){
		for(int j = 0; j < MAXBLOCKSIZE; j++){
			if (blockMatrix[j][i] != NULL)
				return i;
		}
	}
	// Should never be returned as there should never be an empty matrix, but better be sure.
	return -1;
}

Alien* AlienBlock::getAlien(string name){
	// Make sure we at least return NULL.
	Alien* alien = NULL;
	for(int i = 0; i < MAXBLOCKSIZE; i++){
		for(int j = 0; j < MAXBLOCKSIZE; j++){
			if (blockMatrix[i][j] != NULL){
				// If the names match, this is our alien.
				if (blockMatrix[i][j]->name.compare(name) == 0)
					alien = blockMatrix[i][j];
			}
		}
	}
	return alien;
}