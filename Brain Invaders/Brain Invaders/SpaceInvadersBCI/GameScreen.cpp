#include "GameScreen.h"
#include "CSpaceInvadersBCI.h"
#include "AlienBlock.h"
using namespace OpenViBEVRDemos;

#include <Ogre.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include <OgreTextAreaOverlayElement.h>
using namespace Ogre;

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sstream>
#include <time.h>
#include <math.h>
#include <vector>
using namespace std;

#include "../tinyxml/tinyxml.h"


GameScreen::GameScreen(CSpaceInvadersBCI *ScrMan, SceneManager *mSM, CAbstractVrpnPeripheral * vrpnPeripheral, bool shuffle) : SpaceInvadersScreen(ScrMan, mSM)
{
	m_poVrpnPeripheral = vrpnPeripheral;

	allowShuffle = shuffle;

	// Make sure the matrices are clean so we dont use any old memory
	cleanMatrices();

	// Load in the assets, makes for a faster loading afterwards.
	loadAssets();

	// Get the sequence in which to flash. Required for old protocol.
	readFlashSequenceFile();
	flashesSinceLastRepetition = 0;
	soundManager = new SoundManager();
	progressBar = new ProgressBar(sceneManager);
	trainingMode = false;

	// Load in the first level, so we can start playing immediately.
	loadLevel(1);
	flashTime = 0.1;
	ISITime = NextISI();
	pauseTime = 2;
	prevTime = 1;
	score = 0;
	setUpOverlay();
	visible(false);
}

GameScreen::~GameScreen(){
	// Delete all elements in the matrix
	for(int i = 0; i < MAXBLOCKS; i++){
		if(alienBlocks[i] != NULL)
			delete alienBlocks[i];
	}
	// Reset the memory of the matrix
	cleanMatrices();

	// Delete remaining classes.
	delete soundManager;
	delete progressBar;
}

int GameScreen::timeInMilliSeconds(){
	int seconds = time(NULL);
	int milli = ((clock() % CLOCKS_PER_SEC) * 1000) / CLOCKS_PER_SEC;
	return (seconds * 1000) + milli;
}

void GameScreen::loadAssets(){
	// Loading in all sprites beforehand makes sure animations dont lag.
	Entity *tempEntity = sceneManager->createEntity("temp","cube.mesh");
	tempEntity->setMaterialName("Spaceinvader/Alien_0_0_U");
	tempEntity->setMaterialName("Spaceinvader/Alien_0_0_F");
	tempEntity->setMaterialName("Spaceinvader/Alien_0_1_U");
	tempEntity->setMaterialName("Spaceinvader/Alien_0_1_F");
	tempEntity->setMaterialName("Spaceinvader/Alien_1_0_U");
	tempEntity->setMaterialName("Spaceinvader/Alien_1_0_F");
	tempEntity->setMaterialName("Spaceinvader/Alien_1_1_U");
	tempEntity->setMaterialName("Spaceinvader/Alien_1_1_F");
	tempEntity->setMaterialName("Spaceinvader/Alien_2_0_U");
	tempEntity->setMaterialName("Spaceinvader/Alien_2_0_F");
	tempEntity->setMaterialName("Spaceinvader/Alien_2_1_U");
	tempEntity->setMaterialName("Spaceinvader/Alien_2_1_F");
	sceneManager->destroyEntity("temp");
}

bool GameScreen::readFlashSequenceFile()
{
	// Get the flashes from this file
	fstream myfile ("Resources/Config/config_P300FlashSequence.txt");
	if ( myfile.fail() )
	{
		std::cout << "P300sequenceFlash file not found" << std::endl;
		return false;
	} 

	m_vFlashSequence.clear();

	std::string line;
	while ( !myfile.eof() )
	{
		getline(myfile, line);
		std::istringstream ss(line);
		int l_iIndexToFlash;
		ss >> l_iIndexToFlash;

		// Lets make sure non of the flashes are out of bounds
		if ( l_iIndexToFlash >= 12 )
		{
			screenManager->LogEvent("ERROR : Bad flash sequence file");
			return false;
		}
		else
		{
			// Add the new flashes to the list.
			m_vFlashSequence.push_back(l_iIndexToFlash);
		}
	}
	return true;
}

void GameScreen::readTargetSequenceFile()
{
	// Get the target list from the file
	fstream myfile("Resources/Config/config_P300TargetSequence.txt");
	if ( myfile.fail() )
	{
		screenManager->LogEvent("P300targetFlash file not found");
		return;
	} 

	m_vTargetSequence.clear();

	std::string line;
	while( !myfile.eof () )
	{
		getline(myfile,line);

		// Get the targets from the line
		string frag1;
		string frag2;
		size_t pos = line.find(" ");
		if (pos==string::npos)
		{
			continue;
		}
		frag1 = line.substr(0,pos);
		frag2 = line.substr(pos);

		int part1 = -1;
		int part2 = -1;
		std::istringstream ss1( frag1 );
		ss1 >> part1;
		std::istringstream ss2( frag2 );
		ss2 >> part2;

		// Add targets to the list
		m_vTargetSequence.push_back(std::pair<int,int>(part2,part1));
	}
}

void GameScreen::setMode(bool training){
	// Clean old level
	for(int i = 0; i < MAXBLOCKS; i++){
		if(alienBlocks[i] != NULL)
			delete alienBlocks[i];
	}
	cleanMatrices();

	// To ensure that we're flashing in the right order, reset the flash sequence every time a mode is changed.
	readFlashSequenceFile();
	// Set the trainingMode and load the training level.
	trainingMode = training;
	if (trainingMode){
		// We need targets to train on.
		readTargetSequenceFile();
	}
	loadLevel(1);
}

bool GameScreen::loadLevel(int level){
	// This function is pretty hardcoded, meaning that an inproperly declared level will probably crash the program. Can be fixed by a LOT of null-checks making this function at least twice as long. Sort of TODO
	// In essence, the function parses through an XML document and loads all elements from it.
	// For specification of XML document, see documentation outside of code.
	screenManager->LogEvent("Load New Level");
	std::stringstream ss;
	// We load a specific traininglevel if training, the level with the corresponding number otherwise.
	if (trainingMode)
		ss << "Resources/game/Levels/BILevelTraining.xml";
	else
		ss << "Resources/game/Levels/BILevel" << level << ".xml";
	TiXmlDocument doc(ss.str().c_str());
	// Try to load the level.
	if(doc.LoadFile()){
		screenManager->LogEvent(ss.str());
		currentLevel = level;
		TiXmlElement* currentElement = doc.FirstChild("Level")->FirstChild("TotalBlocks")->ToElement();
		// We need the number of blocks for parsing purposes
		int numOfBlocks = atoi(currentElement->GetText());
		// Load in level settings (mostly speed related)
		double startSpeed = strtod(currentElement->NextSibling("StepSpeed")->ToElement()->GetText(),NULL);
		double speedIncrease = strtod(currentElement->NextSibling("SpeedIncrease")->ToElement()->GetText(),NULL);
		double maxSpeed = strtod(currentElement->NextSibling("MaxSpeed")->ToElement()->GetText(),NULL);
		// Process each block.
		for (int i = 0; i < numOfBlocks && i < MAXBLOCKS; i++){
			currentElement = currentElement->NextSibling("Block")->ToElement();
			TiXmlElement* blockElement = currentElement->FirstChild("TotalAliens")->ToElement();
			// We need to know how many aliens we will encounter.
			int totalAliens = atoi(blockElement->GetText());

			// Construct the path for this block
			TiXmlElement * pathElement = blockElement->NextSibling("Path")->FirstChild("NumOfSteps")->ToElement();
			
			// Lets keep track of the path length.
			int pathLength = atoi(pathElement->GetText());
			
			int path[MAXPATHSIZE][2];
			// Contruct the Path
			for(int j = 0; j < MAXPATHSIZE; j++){
				// If we're still within path length, then there must be a step in the XML which we can parse.
				if (j < pathLength){
					pathElement = pathElement->NextSibling("Step")->ToElement();
					path[j][0] = atoi(pathElement->FirstChild("X")->ToElement()->GetText());
					path[j][1] = atoi(pathElement->FirstChild("Y")->ToElement()->GetText());
				}
				else{
					path[j][0] = -1;
					path[j][1] = -1;
				}
			}
			// Construct block
			alienBlocks[i] = new AlienBlock(sceneManager, path, startSpeed, speedIncrease, maxSpeed);

			// Load each Alien
			for(int j = 0; j < totalAliens; j++){
				blockElement = blockElement->NextSibling("Alien")->ToElement();

				// Get info about the Alien.
				string name = blockElement->FirstChild("Name")->ToElement()->GetText();
				int relX = atoi(blockElement->FirstChild("RelPos")->FirstChild("X")->ToElement()->GetText());
				int relY = atoi(blockElement->FirstChild("RelPos")->FirstChild("Y")->ToElement()->GetText());

				int matX = atoi(blockElement->FirstChild("MatPos")->FirstChild("X")->ToElement()->GetText());
				int matY = atoi(blockElement->FirstChild("MatPos")->FirstChild("Y")->ToElement()->GetText());

				int type = atoi(blockElement->FirstChild("Type")->ToElement()->GetText());

				// Construct Alien from info.
				Alien *alien = new Alien(sceneManager, name, type);
				// Add the Alien to the currently parsed block.
				alienBlocks[i]->addAlien(alien, relX, relY);
				// Also keep the Alien in a seperate matrix for flashing purposes.
				alienMatrix[matX][matY] = alien;
			}
		}
		// Suffling the matrix to ensure random flashing and such.
		ShuffleMatrix();
		isLoading = false;
		// Start a new repetition cycle.
		repetition = 0;
		m_bResetTabP300 = true;
		maxRepetitions = 8;
		progressBar->reset(maxRepetitions);
		currentState = BlackScreen;
		return true;
	}
	else{
		return false;
	}
}

void GameScreen::cleanMatrices(){
	screenManager->LogEvent("Clean the Matrix");
	// Set the value for all Aliens in the matrix to NULL.
	for(int i = 0; i < MAXMATRIXSIZE; i++){
		for(int j = 0; j < MAXMATRIXSIZE; j++){
			alienMatrix[i][j] = NULL;
		}
	}
	// Set the value to NULL for all blocks.
	for(int i = 0; i < MAXBLOCKS; i++){
		if(alienBlocks[i] != NULL){
			alienBlocks[i] = NULL;
		}
	}
}

void GameScreen::ShuffleMatrix(){
	if (allowShuffle)
		screenManager->LogEvent("Shuffle the Matrix");
	// We want to use a random function.
	srand(timeInMilliSeconds());
	// First Randomize the rows
	for (int i = 0; i < MAXMATRIXSIZE; i++){
		std::vector<Alien*> currentList;
		
		// Get a list of all the elements in the row.
		for (int j = 0; j < MAXMATRIXSIZE; j++){
			if(alienMatrix[j][i] != NULL){
				currentList.push_back(alienMatrix[j][i]);
			}
		}
		
		int alienInRow = currentList.size();
		// We will place all the elements in the row next to each other, preferably shuffled
		for (int j = 0; j < alienInRow; j++){
			// Take a random element from the created list
			int elemToPlace = 0;
			if (allowShuffle)
				elemToPlace = rand() % currentList.size();
			// Place this element next in line.
			alienMatrix[j][i] = currentList.at(elemToPlace);
			std::vector<Alien*> tempList;
			// Remove the added element from the generated list so we wont add it again. We use a temp list, so needs to overwrite currentList at osme point.
			for (int k = 0; k < currentList.size(); k++){	
				if (k != elemToPlace){
					tempList.push_back(currentList.at(k));
				}
			}
			while (currentList.size() > 0)
				currentList.pop_back();
			for (int k = 0; k < tempList.size(); k++){
				currentList.push_back(tempList.at(k));
			}
			//alienMatrix[j][i] = currentList.at(j);
		}
		for (int j = alienInRow; j < MAXMATRIXSIZE; j++)
			alienMatrix[j][i] = NULL;
	}
	// Next randomize the Columns. Essentially the same. For explanation see above.
	for (int i = 0; i < MAXMATRIXSIZE; i++){
		std::vector<Alien*> currentList;
		for (int j = 0; j < MAXMATRIXSIZE; j++){
			if(alienMatrix[i][j] != NULL){
				currentList.push_back(alienMatrix[i][j]);
			}
		}
		
		int alienInCol = currentList.size();
		for (int j = 0; j < alienInCol; j++){
			int elemToPlace = 0;
			if (allowShuffle)
				elemToPlace = rand() % currentList.size();
			alienMatrix[i][j] = currentList.at(elemToPlace);
			std::vector<Alien*> tempList;
			for (int k = 0; k < currentList.size(); k++){	
				if (k != elemToPlace){
					tempList.push_back(currentList.at(k));
				}
			}
			while (currentList.size() > 0)
				currentList.pop_back();
			for (int k = 0; k < tempList.size(); k++){
				currentList.push_back(tempList.at(k));
			}
		}
		for (int j = alienInCol; j < MAXMATRIXSIZE; j++)
			alienMatrix[i][j] = NULL;
	}
	for (int i = 0; i < MAXMATRIXSIZE; i++){
		for(int j = 0; j < MAXMATRIXSIZE; j++){
			
			if (alienMatrix[i][j] != NULL){
				std::stringstream ss;
				ss << "Alien at " << j << ":" << i << " of type " << alienMatrix[i][j]->getType();
				screenManager->LogEvent(ss.str());
				if (alienMatrix[i][j]->getType() == Alien::AlienType::Target){
					targetAlien.first = i;
					targetAlien.second = j;
				}
			}
		}
	}
}

void GameScreen::setUpOverlay(){
	// Load the Font
	FontManager &fontMgr = FontManager::getSingleton();
	ResourcePtr font = fontMgr.create("GameFont","General");
	font->setParameter("type","truetype");
	font->setParameter("source","Smirnof.ttf");
	font->setParameter("size","20");
	font->setParameter("resolution","96");
	font->load();

	// Load the overlay
	OverlayManager& overlayMgr = OverlayManager::getSingleton();
	Ogre::OverlayContainer* panel = static_cast<OverlayContainer*>(
    overlayMgr.createOverlayElement("Panel", "PanelName"));
	panel->_setPosition(0.05, 0.925);
	panel->setDimensions(300, 120);

	TextAreaOverlayElement* textArea = static_cast<TextAreaOverlayElement*>(
    overlayMgr.createOverlayElement("TextArea", "TextAreaName"));
	textArea->setMetricsMode(Ogre::GMM_PIXELS);
	textArea->setPosition(0, 0);
	textArea->setDimensions(300, 120);
	textArea->setCharHeight(24);
	textArea->setColour(ColourValue(1,1,0));
	// set the font name to the font resource that you just created.
	textArea->setFontName("GameFont");
	// say something

	std::stringstream ss;
	ss << "Score: " << score;

	textArea->setCaption(ss.str().c_str());

	overlay = overlayMgr.create("OverlayName");
	overlay->add2D(panel);

	panel->addChild(textArea);

}

void GameScreen::finishLevel(){
	// Make sure all blocks and the Matrix are cleaned up.
	for(int i = 0; i < MAXBLOCKS; i++){
		if(alienBlocks[i] != NULL)
			delete alienBlocks[i];
	}
	cleanMatrices();
	// Try to load the next Level;
	if (!loadLevel(currentLevel)){
		// If load unsuccesful load first level and return to main menu.
		screenManager->FinishGame();
	}
}

void GameScreen::visible(bool isVisible){
	// We also want invisibility if we're at a Black Screen.
	bool setAliens = !(currentState == BlackScreen || !isVisible);
	for (int i = 0; i < MAXMATRIXSIZE; i++){
		for(int j = 0; j <MAXMATRIXSIZE; j++){
			if(alienMatrix[i][j] != NULL)
				alienMatrix[i][j]->setVisible(setAliens);
		}
	}
	// Show the progressBar (lets hide it in training Mode)
	progressBar->visible(setAliens && !trainingMode);
	if (setAliens  && !trainingMode)
		overlay->show();
	else
		overlay->hide();
	// Reset the timers, since they probably ran out.
	if (isVisible){
		stateTimer.reset();
		movementTimer.reset();
	}
}

double GameScreen::NextISI(){
	double result = 0;

	srand (timeInMilliSeconds());
	// Find a new ISI time, randomly between 50 ms and 1 s, according to exponential distribution.
	while(result < 0.05 || result > 1.0){
		double x = (double) rand() / (double) RAND_MAX;
		// we try to average around 200 ms, hence the 5 (= 1/0.2).
		result = 5 * std::exp(-5 * x);
	}
	return result;
}

bool GameScreen::update(double timeSinceLastUpdate){
	bool moved = false;
	int milliTime = movementTimer.getMilliseconds();
	// Move all the blocks.
	for (int i = 0; i < MAXBLOCKS; i++){
		
		if(alienBlocks[i] != NULL && (currentState != BlackScreen && currentState != ShowTarget)){
			bool temp = alienBlocks[i]->updateBlock(milliTime);
			moved = moved || temp;
		}
	}
	// If there is a movement, play a sound.
	if (moved){
		screenManager->LogEvent("Moved");
		soundManager->move();
		movementTimer.reset();
	}
	// Switch over GameStates
	switch(currentState){
		case BlackScreen:
			// Go to Pause Time
			if (stateTimer.getMilliseconds() >= 1000*pauseTime){
				// For training we need to select the target.
				if (trainingMode){
					// No target then end training.
					if (!nextTarget()){
						screenManager->communicationHandler->SetBeginProcessing();
						currentState = FinishTrain;
						stateTimer.reset();
						break;
					}
				}				
				stateTimer.reset();
				
				currentState = SendTargetInfo;
				visible(true);
				// Show the current Target
				for (int i = 0; i < MAXMATRIXSIZE; i++)
					for (int j = 0; j < MAXMATRIXSIZE; j++)
						if (alienMatrix[i][j] != NULL)
							alienMatrix[i][j]->ShowTargetMode();
				screenManager->LogEvent("Show Target");
			}
			break;
		case SendTargetInfo:
			// This phase sends the signal that a new repetition is about to be started.
			if (stateTimer.getMilliseconds() >= 150){
				stateTimer.reset();
				screenManager->LogEvent("\n\n======Start New Repetition ======\n");
				std::stringstream ss;
				ss << "Send Target Info\n\tRow: " << targetAlien.second << "\n\tColumn: " << targetAlien.first;
				screenManager->LogEvent(ss.str());
				screenManager->communicationHandler->SetTargetRow(targetAlien.second);
				Sleep(100);
				screenManager->communicationHandler->SetFinalized();
				Sleep(50);
				screenManager->communicationHandler->SetTargetColumn(targetAlien.first);
				Sleep(100);
				screenManager->communicationHandler->SetFinalized();
				currentState = ShowTarget;
			}
			break;
		case ShowTarget:
			// Go to Pause Time
			if (stateTimer.getMilliseconds() >= 1000*pauseTime){
				currentState = Pause;
				screenManager->communicationHandler->SetFinalized();
				stateTimer.reset();
				// Make sure all is unflashed (also stops Target display)
				for (int i = 0; i < MAXMATRIXSIZE; i++)
					for (int j = 0; j < MAXMATRIXSIZE; j++)
						if (alienMatrix[i][j] != NULL)
							alienMatrix[i][j]->UnFlash();
			}
			break;
		case Pause:
			// Set first Flash
			if (stateTimer.getMilliseconds() >= 1000*pauseTime){
				currentState = Flash;
				currentlyAt = 0;
				stateTimer.reset();
				FlashNext();
			}
			break;
		case Flash:
			// Flash Finished, Unflash
			if (stateTimer.getMilliseconds() >= 1000*flashTime){
				stateTimer.reset();
				screenManager->communicationHandler->SetFinalized();
				// If there is a next flash, go to ISI state.
				if (UnFlash())
					currentState = ISI;
				else{
					// Else handle a new repetition
					if (trainingMode){
						// For training we need to go back to Black Screen State (= target selection)
						currentState = ISI;
						repetition++;
						if (repetition >= maxRepetitions){
							repetition = 0;
							currentState = BlackScreen;
						}
					}
					else{
						// Start preparing next repetition.
						currentState = Shoot;
					}
				}

			}
			break;
		case ISI:
			// Flash Next Alien Group
			if (stateTimer.getMilliseconds() >= 1000*ISITime){	
				ISITime = NextISI();
				currentState = Flash;
				stateTimer.reset();
				FlashNext();
			}
			break;
		case Shoot:
			if (stateTimer.getMilliseconds() >= 2000){
				stateTimer.reset();
				// First process the VRPN
				if (processVRPN()){
					currentState = SendTargetInfo;
					screenManager->communicationHandler->SetFinalized();
					// Shoot the selected Alien.
					shootAlien();
				}
				// Start a new repetition, go to LooseScreen if this is the last one.
				repetition++;
				if (repetition >= maxRepetitions && currentState != WinScreen){
					screenManager->LogEvent("Lost Level");
					currentState = LooseScreen;
				}
			}
			break;
		case FinishTrain:
			// Finishes the training of the game.
			if (stateTimer.getMilliseconds() >= 150){
				stateTimer.reset();
				screenManager->communicationHandler->SetFinalized();
				screenManager->FinishGame();
			}
			break;
		case WinScreen:
			// Loads new level and shows winning screen.
			if (stateTimer.getMilliseconds() >= 2000){
				stateTimer.reset();
				currentLevel++;
				screenManager->LoadScreen(true);
			}
			break;
		case LooseScreen:
			// Resets the current level and shortly shows a loosing screen
			if (stateTimer.getMilliseconds() >= 2000){
				stateTimer.reset();
				score -= maxRepetitions*250;
				screenManager->LoadScreen(false);
			}
			break;
		default:
			break;
	}
	return true;
}

void GameScreen::FlashNext(){
	//std::cout<<"Flash Next"<<std::endl<<std::endl<<std::endl<<std::endl;
	flashesSinceLastRepetition++;
	// Get index of what to flash
	int next = m_vFlashSequence.front();
	bool doRow;
	// We split up between rows and columns
	if (next < 6)
		doRow = true;
	else{
		// Correct value for column
		next = next - 6;
		doRow = false;
	}

	if (doRow){
		// In case of flash Row, flash all elements that have the same ending index
		for(int i = 0; i < MAXMATRIXSIZE; i++){
			if (alienMatrix[i][next] != NULL){
				alienMatrix[i][next]->Flash();
				if (alienMatrix[i][next]->getType() == Alien::AlienType::Target)
					// Flash the required average for the target row..
					flashTime = 0.15;
			}
		}
		// Tell the ParallelPort a flash occured
		std::stringstream ss;
		ss << "Flash Row: " << next;
		screenManager->LogEvent(ss.str());
		screenManager->communicationHandler->SetFlashRow(next);
	}
	else{
		// In case of Column, flash all elements with same beginning index
		for(int i = 0; i < MAXMATRIXSIZE; i++){
			if (alienMatrix[next][i] != NULL){
				alienMatrix[next][i]->Flash();
				if (alienMatrix[next][i]->getType() == Alien::AlienType::Target)
					// Flash the target for the average time.
					flashTime = 0.15;
			}
		}
		// Tell the ParallelPort a flash occured.
		std::stringstream ss;
		ss << "Flash Column: " << next;
		screenManager->LogEvent(ss.str());
		screenManager->communicationHandler->SetFlashColumn(next);
	}
}

bool GameScreen::UnFlash(){
	//std::cout<<"Unflash"<<std::endl<<std::endl<<std::endl<<std::endl;
	flashTime = 0.1;
	// Essentially the same as Flash, only now Unflash will be called.
	int next = m_vFlashSequence.front();
	bool doRow;
	if (next < 6)
		doRow = true;
	else{
		next = next % 6;
		doRow = false;
	}

	if (doRow){
		for(int i = 0; i < MAXMATRIXSIZE; i++){
			if (alienMatrix[i][next] != NULL)
				alienMatrix[i][next]->UnFlash();
		}
		std::stringstream ss;
		ss << "Unflash Row: " << next;
		screenManager->LogEvent(ss.str());
	}
	else{
		for(int i = 0; i < MAXMATRIXSIZE; i++){
			if (alienMatrix[next][i] != NULL)
				alienMatrix[next][i]->UnFlash();
		}
		std::stringstream ss;
		ss << "Unflash Column: " << next;
		screenManager->LogEvent(ss.str());
	}

	// Set up the next flash.
	m_vFlashSequence.pop_front();
	// If we've flashed 12 values, we're at the end of the repetition.
	if (flashesSinceLastRepetition >= 12){
		flashesSinceLastRepetition = 0;
		// Tells the game to continue to next repetition.
		return false;
	}
	else{
		return true;
	}
}

bool GameScreen::processVRPN()
{
	if(!m_poVrpnPeripheral->m_vAnalog.empty())
	{
		std::list < double >* l_rVrpnAnalogState = &m_poVrpnPeripheral->m_vAnalog.front();

		// Since we're only interested in the last signal send by the VRPN (which has a refresh for all rows/columns) we skip past the rest.
		int count = 0;
		while ( m_poVrpnPeripheral->m_vAnalog.size()>1 )
		{
			m_poVrpnPeripheral->m_vAnalog.pop_front();
			l_rVrpnAnalogState = &m_poVrpnPeripheral->m_vAnalog.front();
			count++;
			screenManager->LogEvent("WARNING, openvibe sent a shoot that we won't take into account.");
		}

		// Interate through the VRPN state.
		std::list<double>::iterator ite = l_rVrpnAnalogState->begin();

		int i=0;
		while ( ite!=l_rVrpnAnalogState->end() )
		{
			// We want to store the value of each channel in the appropriate index of the list.
			VRPN_RowColumnFctP300(i,*ite);

			i++;
			++ite;
		}

		// We dont need the VRPN state any more.
		m_poVrpnPeripheral->m_vAnalog.pop_front();

		// Compute the score for each channel (ie row/column)
		RowColumnFctP300ManageRepetitionIndex();

		// Get the element with the best response.
		DetermineCibleFromTabP300();

		return true;
	}
	else
	{
		// In case the VRPN is not functioning, we will shoot a random element. This is usefull for demo purposes.
		srand ( time(NULL) );

		screenManager->LogEvent("Error: No working VRPN, shot Alien at 0:0 instead");

		toShoot = NULL;
		return true;
	}
}

void GameScreen::VRPN_RowColumnFctP300(int idxVRPN, double value)
{
	// Prevent Index Out Of Bound.
	if ( idxVRPN<0 || idxVRPN>=12 )
	{
		screenManager->LogEvent("idxVRPN out of bounds");
		return;
	}

	// We need to make sure the matrix is properly sized.
	if ( m_vdTabRowColumnP300.size()<12)
	{
		m_vdTabRowColumnP300.resize(12,0);
	}

	// Store the VRPN value in the matrix.
	m_vdTabRowColumnP300[idxVRPN] = value;
}

void GameScreen::RowColumnFctP300ManageRepetitionIndex()
{
	// Make sure the matrix is properly sized.
	if ( m_vTabP300.size() < 6*6)
	{
		m_vTabP300.resize(6*6, 0);
	}

	// Lets get a response value for all the elements that flashed.
	for ( int i=0; i<6; i++ )
	{
		for ( int j=0; j<6; j++ )
		{
			// warning : P300 trust values are inversely proportional to received values
			// Response equals the response to the row the element occurs in plus the column.
			double dbtmp = - ( m_vdTabRowColumnP300[i] + m_vdTabRowColumnP300[6+j] );
			
			// If we reset the response, overwrite value, else add them together.
			if ( m_bResetTabP300 )
			{
				m_vTabP300[i*6+j] = dbtmp;
			}
			else
			{
				m_vTabP300[i*6+j] += dbtmp;
			}
		}
	}
	// Make sure the next repetition wont include a reset (overwritten if new level is loaded).
	m_bResetTabP300 = false;	
}

void GameScreen::DetermineCibleFromTabP300()
{
	// Make sure the list is properly sized to preven Out of Bounds.
	if ( m_vTabP300.size() < 6*6)
	{
		screenManager->LogEvent("vector m_vTabP300 size error");
		return;
	}

	pair<int, int> pickedTarget;

	// Find the max value of the response values of all the elements
	for(int i=0; i<6; i++)
	{
		for(int j=0; j<6; j++)
		{
			double dbtmp=m_vTabP300[i*6+j];
			
			// A dead alien should not be shot, so set value low.
			if ( !alienMatrix[j][i]->alive )
			{
				dbtmp=-9999;
			}
			// If new maximum, set new target.
			if(dbtmp>m_dLastP300Maximum) 
			{
				pickedTarget.first = j;
				pickedTarget.second = i;
				m_dLastP300Maximum=dbtmp;
			}
		}
	}
	toShoot = alienMatrix[pickedTarget.first][pickedTarget.second];
	std::stringstream ss;
	ss << "Shoot Alien at\n\tRow: " << pickedTarget.second << "\n\tColumn: " << pickedTarget.first;
	screenManager->LogEvent(ss.str());
	m_dLastP300Maximum=-9999;
}

void GameScreen::shootAlien(){

	// If we do not have a proper target, pick a random one and tell the progress bar we did a false shot.
	if (toShoot == NULL){
		progressBar->nextRepetition(false);
		int randX;
		int randY;
		while(toShoot == NULL || !toShoot->alive){
			randX = rand() % MAXMATRIXSIZE;
			randY = rand() % MAXMATRIXSIZE;
			toShoot = alienMatrix[randY][randX];
		}
		std::stringstream ss;
		ss << "Shoot Random Alien at\n\tRow: " << randX << "\n\tColumn: " << randY;
		screenManager->LogEvent(ss.str());
	}
	else
		progressBar->nextRepetition(true);

	// Shoot the alien.
	int points = toShoot->destroy();
	// Add some points to the score and give an exploding sound.
	if (points != -1){
		score += 250;
		soundManager->explode();
	}
	// Target hit
	if (points == Alien::AlienType::Target){
		
		// Get points dependent on the amount of attempts.
		score += 16000 * std::pow(0.5, (double)repetition);

		// Go to the winning State.
		currentState = WinScreen;
	}

	// Update the Score Value.
	std::stringstream ss;
	ss << "Score: " << score;

	overlay->getChild("PanelName")->getChild("TextAreaName")->setCaption(ss.str().c_str());
}

bool GameScreen::nextTarget(){
	std::pair<int,int> newTarget;
	// Get the next target from the list
	if ( !m_vTargetSequence.empty() )
	{
		newTarget = m_vTargetSequence.front();
		m_vTargetSequence.pop_front();
		// Reset all the elements to normal except for the new target, which is set to Target Mode.
		for (int i = 0; i < MAXMATRIXSIZE; i++)
			for (int j = 0; j < MAXMATRIXSIZE; j++)
				if (alienMatrix[i][j] != NULL)
					alienMatrix[i][j]->makeTarget(i == newTarget.first && j == newTarget.second);

		// Communicate the target.
		
		targetAlien.first = newTarget.first;
		targetAlien.second = newTarget.second;
		return true;
	}
	else{
		return false;
	}
}

void GameScreen::resetTimer(){
	levelTimer.reset();
}

void GameScreen::keyPressed(const OIS::KeyEvent& evt){
	if ( evt.key == OIS::KC_ESCAPE){
		// Go to main menu if Escape is hit.
		screenManager->PauseGame();
	}
	if ( evt.key == OIS::KC_RBRACKET){
		// Cheat Button to load the next level. Usefull for level testing.

		// We don't want to skip flashes, so we reload the sequence for safety reasons.
		readFlashSequenceFile();
		currentState = WinScreen;
	}
}