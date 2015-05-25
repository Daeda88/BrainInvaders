#include "ProgressBar.h"
using namespace OpenViBEVRDemos;

#include <string>
#include <sstream>
using namespace std;

ProgressBar::ProgressBar(Ogre::SceneManager *mSM){
	sceneManager = mSM;
	barNode = sceneManager->getRootSceneNode()->createChildSceneNode("ProgressBarNode", Ogre::Vector3(0,0,0));
	// Create nodes for the maximum amount of repetitions allowed.
	for (int i = 0; i < MAXREPETITIONS; i++){
		std::cout << "Repet " << i << std::endl;
		std::stringstream ss;
		ss << "ProgressBarNode" << i;
		SceneNode* tempNode = barNode->createChildSceneNode(ss.str(), Ogre::Vector3(0,0,0));
		ss.clear();
		ss << "ProgressBarElement" << i;
		barDots[i] = sceneManager->createEntity(ss.str(), "cube.mesh");
		tempNode->attachObject(barDots[i]);
	}
	std::cout << "Done" << std::endl;
}

ProgressBar::~ProgressBar(){
	// Destroy all children, then this.
	barNode->removeAndDestroyAllChildren();
	sceneManager->destroySceneNode(barNode);
}

void ProgressBar::reset(int repetitions){
	// Reset to the next max amound of repetitions.
	maxRepetitions = repetitions;
	currentRepetition = 0;
	// We want to have the elements spread across an equal distance independent of the amount of repetitions, so this spacing is calculated.
	int spacing = 1000/maxRepetitions;
	for (int i = 0; i < MAXREPETITIONS; i++){
		std::stringstream ss;
		ss << "ProgressBarNode" << i;
		SceneNode* tempNode = sceneManager->getSceneNode(ss.str());
		// Place all the nodes within the max amound in the line of the spacing
		if (i < maxRepetitions){
			tempNode->setPosition(Ogre::Vector3(1200 - i*spacing, -900, 0));
			tempNode->setScale(0.5,0.5,0.5);
			tempNode->setVisible(true);
			barDots[i]->setMaterialName("Spaceinvader/FullDot");
		}
		// Make the rest invisible.
		else{
			barDots[i]->setMaterialName("Spaceinvader/EmptyDot");
			tempNode->setVisible(false);
		}
	}
}

void ProgressBar::visible(bool isVisible){
	// Only change the visibility for the elements that can be showed anyway (the rest should be automatically set to invisible)
	for (int i = 0; i < maxRepetitions; i++){
		std::stringstream ss;
		ss << "ProgressBarNode" << i;
		SceneNode* tempNode = sceneManager->getSceneNode(ss.str());
		tempNode->setVisible(isVisible);
	}
}

void ProgressBar::nextRepetition(bool validShot){
	// Make one of the elements empty, since the repetition is done.
	if (currentRepetition < maxRepetitions){
		if (validShot)
			barDots[currentRepetition]->setMaterialName("Spaceinvader/EmptyDot");
		else
			barDots[currentRepetition]->setMaterialName("Spaceinvader/InvalidDot");
		currentRepetition++;
	}
}