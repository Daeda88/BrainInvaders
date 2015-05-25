#include "Alien.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>        ////////////////////////difference
using namespace Ogre; 

#include <string>
#include <sstream>
using namespace std;

Alien::Alien(Ogre::SceneManager *mSM, string pName, int pType){
	name = pName;
	type = pType;
	realType = type;
	alienEntity = mSM->createEntity(name,"cube.mesh");
	
	frame = 0;
	flashing = false;
	alive = true;

	alienEntity->setMaterialName(getMaterialName());
}

Alien::~Alien(){	
	//We want to get rid of all Entities attached to this object.
	SceneNode::ObjectIterator itObject = alienNode->getAttachedObjectIterator();
	// Iterate through all objects and destroy them.
	while ( itObject.hasMoreElements() )
	{
		MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());
		alienNode->getCreator()->destroyMovableObject( pObject );
	}
	//delete alienEntity;
}

void Alien::setSceneNode(SceneNode *pSN){
	alienNode = pSN;
	alienNode->scale(0.65,0.65,0.65);
	alienNode->showBoundingBox(false);
	alienNode->attachObject(alienEntity);
	// Save the scale for resizing purposes
	scale = alienNode->getScale();
}

void Alien::ShowTargetMode(){
	std::stringstream ss;
	ss << "Spaceinvader/Alien_" << type << "_T";
	alienEntity->setMaterialName(ss.str());
}

void Alien::Flash(){
	flashing = true;
	// Adjust the scale if this is a Target Alien
	if (type == Target)
		alienNode->setScale(1.5*scale);
	// We only flash ALiens that are still alive.
	if (alive)
		alienEntity->setMaterialName(getMaterialName());
	//std::cout<<"Flash Alien Fin"<<std::endl;
}

void Alien::UnFlash(){
	flashing = false;
	// Reset scale if this is a Target Alien
	if (type == Target)
		alienNode->setScale(scale);
	// We only (un)flash Aliens that are still alive.
	if (alive)
		alienEntity->setMaterialName(getMaterialName());
	//std::cout<<"Unflash Alien Fin"<<std::endl;
}

void Alien::animate(){
	// We animate if the Alien is alive, else we make it invisible (ensuring an explosion lasts a single animation frame)
	if (alive){
		// NOTE: For more complex animations this function needs to be adjusted to support more frames.
		frame = (frame + 1) % 2;
		alienEntity->setMaterialName(getMaterialName());
	}
	else{
		setVisible(false);
	}
}

string Alien::getMaterialName(){
	std::stringstream ss;
	// Create a string of the form "Spaceinvader/Alien_<int:TypeValue>_<int:FrameValue>_<String:FlashValue (either U or F)>"
	if (flashing)
		ss << "Spaceinvader/Alien_" << type << "_" << frame << "_F";
	else
		ss << "Spaceinvader/Alien_" << type << "_" << frame << "_U";
	return ss.str();
}

void Alien::setVisible(bool visible){
	alienNode->setVisible(visible && alive);
}

int Alien::destroy(){
	if(!alive)
		return -1;
	// Better have a big explosion
	alienNode->setScale(2*scale);
	// Make sure the Alien is dead.
	alive = false;
	// Set texture to explosion.
	alienEntity->setMaterialName("Spaceinvader/Explosion");
	// Return the type so GameScreen knows how to handle points.
	return type;
}

int Alien::getType(){
	return type;
}

void Alien::makeTarget(bool isTarget){
	// Use this to switch between target and normal states for training purposes.
	if (isTarget)
		type = Target;
	else
		type = realType;
}
