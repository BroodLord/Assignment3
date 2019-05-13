// Danny Kavanagh's Assignment 3
// Assignment3.3.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
#include <SFML/Audio.hpp>
using namespace tle;
/******************************************************************************* Function Defatitions ************************************************************/
bool sphereToBox(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, IModel* wall); // This is a function for the collisions between the car and walls
bool sphereToSphere(IModel* checkpoint, IModel* hovercar); // This is a function for the collisions between the car and the checkpoint legs
bool pointToBox(IModel* checkpoint, IModel* hovercar); // This is a function for the collisions between the car and the waypoints
bool sphereToSphereWT(IModel* smallWaterTank, IModel* hovercar);  // This is a function for the collisions between the car and the water tanks
bool wayPointDection(IModel* wayPointDummy, IModel* npcCar); // This is a function used for the waypoint dection
bool sphereToBoxIsles(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, IModel* isles); // This is a function which is used for collision between the car and isles
bool pointToBoxCSD(IModel* cutScenceDummy, IModel* hovercar); // This is a function which is used for the ramp collisions for looking at the dummys
bool sphereToBoxBarrierR(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, float hoverCarCurrentPosY,
	IModel* barrier, float dectionModifyerRX, float dectionModifyerRZ, float dectionModifyerRY); // This is a function which is used for barrier collisons
bool sphereToBoxBarrier(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, float hoverCarCurrentPosY,
	IModel* barrier, float dectionModifyerX, float dectionModifyerZ, float dectionModifyerY); // This function is used for collision between the car and barriers.
bool sphereToSphereNPCCar(IModel* npcCar, IModel* hovercar); // This is a function used for collision between the NPC car and the hover car
bool sphereToBoxSquare(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, IModel* squareCollision);
bool pointToBoxNPC(IModel* checkpoint, IModel* npcCar); // This is a point to box collision box for the hover can and the checkpoints which is used for switching checkpoints

/******************************************************************************* Constant Variables *************************************************************************/
const int kWaterTankAmount = 53;					  // This is the amount of water tanks I need
const int kCutScenceDummy = 4;						  // This is the amount of dummys I need for the ramp
const int kBarrierAmount = 45;						  // This is the amount of barriers I need
const int kWayPointAmount = 35;						  // This is the amount of waypoints I need for the NPC car
const int kRampFloorAmount = 6;						  // This is the amount of ramp floors I need
const int kIslesAmount = 12;                          // This is the amount of isles I will need in the program
const int kWallAmount = 40;       				      // These are the amount of walls I need
const int kCheckpointAmount = 11;					  // This is the amount of checkpoints I need
const int kCheckPointLegAmount = 4;                   // These are the amount of checkpoint legs I have and this will be used in collisions later
const float liftSpeed = 0.005f / 2;                   // This is the speed in which the hover car with bounce
const int rotatedWaterTanks = 3;					  // This is the amount of rotated water tanks that I need
const int kOuterWallAmount = 4;						  // This is the amount of outer walls I need
const int kSquareAmount = 2;						  // This is the amount of squares I need

/**************************************************** ************************* Muscic ************************************************************************/
sf::SoundBuffer impactBuffer;						  // These are sound buffer which are needed for the sound effects of the game (Imoact)
sf::SoundBuffer mainBuffer;							  // These are sound buffer which are needed for the sound effects of the game (Main Music)
sf::SoundBuffer engineBuffer;						  // These are sound buffer which are needed for the sound effects of the game (Engine)
sf::SoundBuffer goBuffer;							  // These are sound buffer which are needed for the sound effects of the game (Go noise)
sf::SoundBuffer finalBuffer;						  // These are sound buffer which are needed for the sound effects of the game (Final start noise)
sf::SoundBuffer warningBuffer;						  // These are sound buffer which are needed for the sound effects of the game (Warning for the boost noise)
sf::Sound warning;									  // These are the actual sounds that will be played in relation to there buffers
sf::Sound goSound;									  // These are the actual sounds that will be played in relation to there buffers
sf::Sound finalSound;								  // These are the actual sounds that will be played in relation to there buffers
sf::Sound mainMusic;								  // These are the actual sounds that will be played in relation to there buffers
sf::Sound impactSound;								  // These are the actual sounds that will be played in relation to there buffers
sf::Sound engineSound;								  // These are the actual sounds that will be played in relation to there buffers

/**************************************************** ************************** EKey's *************************************************************************/
const EKeyCode thrustControl = Key_W;                 // This is a Key which will which will make the hover car go forward in relation to the frame timer
const EKeyCode reverseControl = Key_S;				  // This is a Key which will which will make the hover car go backwards in relation to the frame timer
const EKeyCode rightRotateControl = Key_D;			  // This is a Key which will which will make the hover car turn right at the speed related to the frame timer
const EKeyCode leftRotateControl = Key_A;			  // This is a Key which will which will make the hover car turn left at the speed related to the frame timer
const EKeyCode cameraRoateUp = Key_Up;				  // This is a Key which will rotate the camera behind / in front the hover by a posative X on the xais.
const EKeyCode cameraRoateDown = Key_Down;			  // This is a Key which will rotate the camera behind / in front the hover by a negative X on the xais
const EKeyCode cameraRoateRight = Key_Right;		  // This is a Key which will rotate the camera behind / in front the hover by a posative Y on the xais
const EKeyCode cameraRoateLeft = Key_Left;			  // This is a Key which will rotate the camera behind / in front the hover by a negative Y on the xais
const EKeyCode thirdPersonCam = Key_1;				  // This is a Key which will allows the user to enter the 3rd person camera
const EKeyCode firstPersonCam = Key_2;				  // This is a Key which will allows the user to enter the 1st person camera
const EKeyCode startKey = Key_Space;				  // This is a Key which will allow the user to start the game
const EKeyCode boostyKey = Key_Space;                 // This is a key used to trigger the boost
const EKeyCode resetKey = Key_R;					  // This is a key which will reset the game at the end

/******************************************************************************* Enum State *************************************************************************/
enum eCameraState { thirdPerson, firstPerson };                         // These states are used to switch the camera position from first to third
enum eStartingStates { StandBy, CountDown, Go };                         // These are the states which are used to switch between the standby and Go.
enum eWayPoints { wayPointZero, waypointOne, waypointTwo, wayPointThree, wayPointFour, wayPointFive, wayPointSix, wayPointSeven, wayPointEight, wayPointNine };
// These are used when it comes to changing between different waypoint on the NPC car
enum eCheckpointStates
{
	CheckpointZero, CheckpointOne, CheckpointTwo, CheckpointThree, CheckpointFour, CheckpointFive, CheckpointSix, CheckpointSeven,
	CheckpointEight, CheckpointNine, CheckpointTen, finished
};  // These are checkpoints states which will have certain effects on a leaderboard etc.
enum eLaps
{
	lapZero, lapOne, lapTwo, lapThree, lapFour, lapFive, Finished
	/**** These are used for when it comes to switching between the different laps ****/
};
enum eleaderBoard
{
	playerFirst, playerSecond
	/**** These are used for the leaderboard and saying where the player and NPC are ****/
};
/******************************************************************************** Boolens ***************************************************************************/
bool hovering = false;                                // This is a boolean variable which will determine weather or not to increase the speed or not.
bool goingUp = false;								  // This is a boolean which will make the hover car go up in the cut scence
bool npcGoingUp = false;							  // This is a boolean which will make the npc car go up in the cut scence
bool goingAcross = false;							  // This is a boolean which will make the hover car go across in the cut scence
bool npcGoingAcross = false;						  // This is a boolean which will make the hover car go across in the cut scence
bool lookAtOnce = false;							  // This is a boolean which will make the hover car look at the last dummy
bool goingDown = false;								  // This is a boolean which will make the hover car go down in the cut scence
bool npcGoingDown = false;							  // This is a boolean which will make the npc car go down in the cut scence
bool wayPointChange = false;						  // This is a boolean which will make the npc can the current waypoint
bool npcCarReset = false;                             // This is a boolean to reset the npcs waypoints back to 0
bool boostReset = false;							  // This is a boolean for when the boost is done on the countdown for reset
bool boostRecharge = false;							  // This is a boolean used for when the boost is recharge (countdown)
bool rampCollisions = true;							  // This is a boolean for the collision box of the bottom bit of the ramp
bool crossDead = false;							      // This is a boolean for when the cross is dead/dying
bool checkpointIncrese = false;				          // This is a boolean which is used to increae the checkpoint for the npc
bool raceFinished = false;							  // This is a boolean which will trigger when the race has ended
bool pause = false;									  // This is a boolean which will pause the game when it ends
													  
/******************************************************************************* Structs *************************************************************************/
struct sSquareCollision                                // This is a struct used to hold the squarecollision box model
{
	IModel* squareCollisions;
};
struct SWayPoints									   // This is a struct used to hold the waypoint model
{
	IModel* wayPoints;
};
struct  SouterWalls									   // This is a struct used to hold the outerwall model
{
	IModel* outerWall;
};
struct SrampFloor									   // This is a struct used to hold the ramp floor model
{
	IModel* rampFloor;
};

struct SsmallTanksOne								   // This is a struct used to hold the water tank models
{
	IModel* smallTankOne;
};
struct sDummyCollisions								   // This is a struct used to hold the dummy collisions
{
	IModel* dummyCollisions[3];
};
struct Scheckpoints									   // This struct holds the X and Z positions of the checkpoints
{
	IModel* checkpoints;							   // This is the model I will use for my checkpoints
};
struct player
{
	IModel* hoverCar;								   // This is the Model I will use for the hover car
};
struct SNPCCar										   // This is a struct which will hold the NPC car model
{
	IModel* npcCar;
};
struct Sisles										   // This struct holds the X and Y positions of the Isles
{
	IModel* isles;									   // This is the model I will be using for my isles

};
struct  Swalls										   // This struct holds the X and Z positions of the walls
{
	IModel* wall;									   // This is the Model I will be using for my walls
};
struct SCutScence									   // This is a struct which holds the cutscence dummy model
{
	IModel* cutScenceDummy;
};
struct SBarricade									   // This is a struct which will hold the barrier model
{
	IModel* barrier;
};

/**************************************************** VECTOR CODE ****************************************************************/

struct sVector								           // This is a vector that holds infomation of a vector like the X,Y,Z
{
	float X;								           // This is the X value of the vector which is used like this facingVector.X
	float Y;								           // This is the Y value of the vector (This isn't needed in the program but will prove usful later in other Assignments)
	float Z;								           // This is the Z value of the vector which is used like this facingVector.Z
	sVector()										   // This will get the vector.
	{
	}
	sVector(float x, float y, float z)		           /*This function is called when the vector is created and set the pos infomation (not needed usless you are setting
													   you are setting objects up with vectors)*/
	{
		X = x;										  // This simplely means that the float X that is above is equal to x
		Y = y;										  // This simplely means that the float Y that is above is equal to Y
		Z = z;										  // This simplely means that the float Z that is above is equal to z
	}
	sVector operator+(const sVector& v1) 		           // This takes in the vector and adds both vectors then return the vector
	{
		sVector v;							           // This is the new vector
		v.X = v1.X + X;						           // This adds the X component
		v.Y = v1.Y + Y;						           // This adds the Y component
		v.Z = v1.Z + Z;						           // This adds the Z component
		return v;							           // This will return the new vector
	}
	sVector operator-(const sVector& v1) 		           // This takes in the vector and takes away the from both then returns the vector
	{
		sVector v;							           // This is the new vector
		v.X = v1.X - X;						           // This takes away the X component
		v.Y = v1.Y - Y;						           // This takes away the Y component
		v.Z = v1.Z - Z;						           // This takes away the Z component
		return v;							           // This will return the new vector
	}
	sVector operator*(const sVector& v1) 		           // This takes in the vector and times the both values and returns the vector
	{
		sVector v;							           // This is the new vector
		v.X = v1.X * X;						           // This times the X component
		v.Y = v1.Y * Y;						           // This times the Y component
		v.Z = v1.Z * Z;						           // This times the Z component
		return v;							           // This will return the new vector
	}
	sVector operator/(const sVector& v1) 		           // This takes in the vector divides both of them and returns the vector
	{
		sVector v;							           // This is the new vector
		v.X = v1.X / X;						           // This divids the X component
		v.Y = v1.Y / Y;						           // This divids the Y component
		v.Z = v1.Z / Z;						           // This divids the Z component
		return v;							           // This will return the new vector
	}
};
struct sNPCVector								           // This is a vector that holds infomation of a vector like the X,Y,Z
{
	float X;								           // This is the X value of the vector which is used like this facingVector.X
	float Y;								           // This is the Y value of the vector (This isn't needed in the program but will prove usful later in other Assignments)
	float Z;								           // This is the Z value of the vector which is used like this facingVector.Z
	sNPCVector()										   // This will get the vector.
	{
	}
	sNPCVector(float x, float y, float z)		           /*This function is called when the vector is created and set the pos infomation (not needed usless you are setting
													   you are setting objects up with vectors)*/
	{
		X = x;										  // This simplely means that the float X that is above is equal to x
		Y = y;										  // This simplely means that the float Y that is above is equal to Y
		Z = z;										  // This simplely means that the float Z that is above is equal to z
	}
	sNPCVector operator+(const sNPCVector& v1) 		           // This takes in the vector and adds both vectors then return the vector
	{
		sNPCVector v;							           // This is the new vector
		v.X = v1.X + X;						           // This adds the X component
		v.Y = v1.Y + Y;						           // This adds the Y component
		v.Z = v1.Z + Z;						           // This adds the Z component
		return v;							           // This will return the new vector
	}
	sNPCVector operator-(const sNPCVector& v1) 		           // This takes in the vector and takes away the from both then returns the vector
	{
		sNPCVector v;							           // This is the new vector
		v.X = v1.X - X;						           // This takes away the X component
		v.Y = v1.Y - Y;						           // This takes away the Y component
		v.Z = v1.Z - Z;						           // This takes away the Z component
		return v;							           // This will return the new vector
	}
	sNPCVector operator*(const sNPCVector& v1) 		           // This takes in the vector and times the both values and returns the vector
	{
		sNPCVector v;							           // This is the new vector
		v.X = v1.X * X;						           // This times the X component
		v.Y = v1.Y * Y;						           // This times the Y component
		v.Z = v1.Z * Z;						           // This times the Z component
		return v;							           // This will return the new vector
	}
	sNPCVector operator/(const sNPCVector& v1) 		           // This takes in the vector divides both of them and returns the vector
	{
		sNPCVector v;							           // This is the new vector
		v.X = v1.X / X;						           // This divids the X component
		v.Y = v1.Y / Y;						           // This divids the Y component
		v.Z = v1.Z / Z;						           // This divids the Z component
		return v;							           // This will return the new vector
	}
};

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media"); // Core models folder
	myEngine->AddMediaFolder("C:\\Users\\danny\\Desktop\\Ayyyyyyyy Random Shit\\Assignment 3\\Assignment3.3\\Assignment 3 Models"); // Core models folder for assignment 3
	myEngine->AddMediaFolder("C:\\Users\\danny\\Desktop\\Ayyyyyyyy Random Shit\\Assignment 3\\Assignment3.3\\Images and UI"); // model folder for images and UI
	myEngine->AddMediaFolder("C:\\Users\\danny\\Desktop\\Ayyyyyyyy Random Shit\\Assignment 3\\Assignment3.3\\Extra Models"); // model folder for extra models

	/**** Set up your scene here ****/
	/***************************************************************************************** State Declarations **************************************************************************************/
	eCameraState cameraState = thirdPerson;                   // This is where the cameraState will be set with it starting in third person
	eStartingStates StartStates = StandBy;					  // This is the starting state before the game starts
	eCheckpointStates checkpointStates = CheckpointZero;	  // This is what chechpoint the player starts at
	eWayPoints wayPointStates = wayPointZero;				  // This is what waypoint the NPC car will start on
	eLaps laps = lapZero;									  // This is what the lap is set to by default
	eleaderBoard leaderBoard = playerFirst;					  // This is what the leaderboard is set to by default

	/******************************************************************************** Music Playing ******************************************************************/
	mainBuffer.loadFromFile("Music and Sound Effects\\A Journey Awaits.ogg");      // This will load a file into the buffer
	mainMusic.setBuffer(mainBuffer);											   // This will set the sound to the buffer
	mainMusic.setVolume(40.0f);													   // This sets the volume of the noise
	mainMusic.setLoop(true);													   // This sets the loop to true so it replays
	sf::Listener::setGlobalVolume(10.0f);										   // This sets the global volume of the noise
	mainMusic.play();															   // This will play the music

	warningBuffer.loadFromFile("Music and Sound Effects\\alert.wav");			   // This will load a file into the buffer
	warning.setBuffer(warningBuffer);											   // This will set the noise to the buffer

	impactBuffer.loadFromFile("Music and Sound Effects\\qubodupImpactMetal.ogg");  // This will load a file into the buffer
	impactSound.setBuffer(impactBuffer);										   // This will set the noise to the buffer

	engineBuffer.loadFromFile("Music and Sound Effects\\enginehum.ogg");		   // This will load a file into the buffer
	engineSound.setBuffer(engineBuffer);										   // This will set the noise to the buffer
	engineSound.setVolume(100.0f);												   // This will set the volume of the noise
	engineSound.setLoop(true);													   // This sets the loop to true so it replays
	engineSound.play();															   // This will play the engine noise

	goBuffer.loadFromFile("Music and Sound Effects\\countdown-a.ogg");			   // This will load a file into the buffer
	goSound.setBuffer(goBuffer);												   // This will set the noise to the buffer
	goSound.setVolume(100.0f);													   // This will set the volume of the noise

	finalBuffer.loadFromFile("Music and Sound Effects\\countdown-b.ogg");		   // This will load in a file into the buffer
	finalSound.setBuffer(finalBuffer);											   // this will set the noise to the buffer
	finalSound.setVolume(100.0f);												   // This will set the volume of the noise

	/******************************************************************************* Struct Calling ******************************************************************/

	SsmallTanksOne smallTanksOne[kWaterTankAmount];			  // This calls the model from the struct and sets the amount I need
	Scheckpoints checkpoints[kCheckpointAmount];              // This calls the checkpoint struct and allows me to pull the X and Z positions from it
	Sisles isles[kIslesAmount];						          // This calls the isles struct and allows me to pull the X and Z positions from it
	Swalls walls[kWallAmount];						          // This calls the wall struct and this allows me to pull the X and Z positions from it
	player playerCar;										  // This calls the playercar struct and the model
	sVector normFacing = { 0, 0, 0 };                         // This is the normilies facing vector for my vector calculations
	sVector facingVector = { 0,0,0 };					      // This is the facing vector for my vector calculations
	sVector dragVector = { 0,0,0 };							  // This is the vector which will preform the drag effect on the car
	sVector normDrag = { 0,0,0 };							  // This is the normilesed drag vector which will be used to get the drag effect calculations
	sVector Momentum = { 0,0,0 };							  // This is the vector I will be using for how much the car will drift after the W Key is let go.
	sNPCVector npcNormFacing = { 0, 0, 0 };                   // This is the normilies facing vector for my vector calculations
	sNPCVector npcFacingVector = { 0,0,0 };					  // This is the facing vector for my vector calculations
	sNPCVector npcDragVector = { 0,0,0 };				      // This is the vector which will preform the drag effect on the car
	sNPCVector npcNormDrag = { 0,0,0 };						  // This is the normilesed drag vector which will be used to get the drag effect calculations
	sNPCVector npcMomentum = { 0,0,0 };						  // This is the vector I will be using for how much the car will drift after the W Key is let go.
	sDummyCollisions dummyCollision;						  // This calls the struct I need
	SrampFloor rampFloor[kRampFloorAmount];					  // This calls the struct and then sets the amount I need
	SCutScence cutScenceDummy[3];							  // This calls the struct and then sets the amount I need
	SBarricade barrier[kBarrierAmount];						  // This calls the struct and then sets the amount I need
	SouterWalls outerwalls[kOuterWallAmount];				  // This calls the struct and then sets the amount I need
	SNPCCar npcCar;											  // This calls the struct I need
	SWayPoints waypoints[kWayPointAmount];					  // This calls the struct I need and then sets the amount I need
	sSquareCollision squareCollisions[kSquareAmount];         // This calls the struct I need and sets the amount of it I need


	/****************************************************************************** Object Detils ********************************************************************/
								   /*** All of the variables below are spawning locations for the object and which one will have a X, Y and Z ***/

	/*** Isles Spawning ***/
	float islesXPos[kIslesAmount] = { -10.0f, -10.0f, 10.0f, 10.0f, 80.0f, 80.0f, 120.0f, 120.0f, 160.0f, 160.0f,
									  190.0f, 190.0f };                                                            

	float islesZPos[kIslesAmount] = { 10.0f, 85.0f, 10.0f, 85.0f, 10.0f, 85.0f, 10.0f, 85.0f, -12.0f, -30.0f,
									  -12.0f, -30.0f };                                                          

	/*** Waypoint Spawning ***/
	float waypointXPos[kWayPointAmount] = { 0.0f, 0.0f, 0.0f, 30.0f, 60.0f, 90.0f, 110.0f, 110.0f, 100.0f,
										   100.0f, 130.0f, 150.0f, 200.0f, 220.0f, 250.0f,
										   250.0f, 250.0f, 250.0f, 250.0f, 420.0f, 420.0f, 420.0f, 420.0f,
										   420.0f, 400.0f, 380.0f, 370.0f, 350.0f, 340.0f, 330.0f,
										   280.0f, 160.0f, 40.0f, 10.0f, 0.0f};

	float waypointYPos[kWayPointAmount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
										   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
										   60.0f, 60.0f, 60.0f, 60.0f, 60.0f, 60.0f, -8.0f,
										   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
										   0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	float waypointZPos[kWayPointAmount] = { 0.0f, 40.0f, 80.0f, 140.0f, 140.0f, 120.0f, 90.0f, 60.0f, 20.0f, 0.0f,
										  -20.0f, -20.0f, -10.0f - 10.0f , 0.0f, 30.0f,
										   50.0f, 115.0f, 250.0f, 330.0f, 330.0f, 250.0f, 265.0f, 180.0f,
										   150.0f, 90.0f, 40.0f, 0.0f, -50.0f, -90.0f, -140.0f,
										   -160.0f, -180.0f, -180.0f, -120.0f, -60.0f };

	/*** RampFloor Spawning ***/
	float rampFloorXPos[kRampFloorAmount] = { 250.0f, 250.0f, 318.0f, 405.0f, 426.0f, 426.0f };

	float rampFloorYPos[kRampFloorAmount] = { -5.0f, 52.0f, 52.0f, 52.0f, 52.0f, -2.0f };

	float rampFloorZPos[kRampFloorAmount] = { 50.0f, 320.0f, 338.0f, 338.0f, 358.0f, 205.0f };

	/*** Square Spawning ***/
	float squareXPos[kSquareAmount] = { 250.0f, 426.0f };

	float squareYPos[kSquareAmount] = { -20.0f, -20.0f };

	float squareZPos[kSquareAmount] = { 70.0f, 215.0f };

	/*** CutScenceDummy Spawning ***/

	float cutScenceDummyXPos[kCutScenceDummy] = { 250.0f, 250.0f, 425.0f, 425.0f };

	float cutScenceDummyYPos[kCutScenceDummy] = { 60.0f, 60.0f, 0.0f, 0.5f };

	float cutScenceDummyZPos[kCutScenceDummy] = { 115.0f, 250.0f, 200.0f, 180.0f };

	/*** outerwalls spawning ***/

	float outerWallXPos[kOuterWallAmount] = { 220.0f, 220.0f , -50.0f, 450.0f, };

	float outerWallZPos[kOuterWallAmount] = { -200.0f, 460.0f , 100.0f, 100.0f };

	/*** Wall Spawning ***/

	float wallXPos[kWallAmount] = { 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, -10.0f,
									10.0f, -10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, -10.0f,
									80.0f, 120.0f, 80.0f, 120.0f, 80.0f, 120.0f, 80.0f, 120.0f,
									80.0f, 120.0f, 80.0f, 120.0f, 80.0f, 120.0f, 80.0f, 120.0f,
									165.0f, 165.0f, 171.9f, 171.9f, 178.8f, 178.8f, 185.7f, 185.7f };			      

	float wallZPos[kWallAmount] = { 15.0f, 15.0f, 24.6f, 24.6f, 34.2f, 34.2f, 43.8f, 43.8f, 53.4f,
									53.4f, 63.0f, 63.0f, 72.6f, 72.6f, 82.2f, 82.2f,
									15.0f, 15.0f, 24.6f, 24.6f, 34.2f, 34.2f, 43.8f, 43.8f, 53.4f,
									53.4f, 63.0f, 63.0f, 72.6f, 72.6f, 82.2f, 82.2f,
									-12.0f, -30.0f, -12.0f, -30.0f, -12.0f, -30.0f, -12.0f, -30.0f };			      

	/*** Barrier Spawning ***/

	float BarrierXPos[kBarrierAmount] = { 230.0f, 230.0f, 230.0f, 230.0f, 230.0f, 230.0f, 230.0f, 230.0f,
										 270.0f, 270.0f, 270.0f, 270.0f, 270.0f,
										 407.0f, 407.0f, 407.0f,
										 445.0f, 445.0f, 445.0f, 445.0f, 445.0f, 445.0f,
										 277.0f, 294.0f, 311.0f, 328.0f, 345.0f, 362.0f, 379.0f, 396.0f,
										 240.0f, 257.0f, 274.0f, 291.0f, 308.0f, 325.0f, 342.0f, 359.0f, 376.0f, 393.0f, 410.0f, 427.0f, 444.0f,
										 240.0f, 258.0f };

	float BarrierYPos[kBarrierAmount] = { 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f,
										 55.0f, 55.0f, 55.0f, 55.0f, 55.0f,
										 55.0f, 55.0f, 55.0f,
										 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f,
										 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f,
										 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f, 55.0f,
										 55.0f, 55.0f };

	float BarrierZPos[kBarrierAmount] = { 240.0f, 256.0f, 273.0f, 290.0f, 307.0f, 324.0f, 341.0f, 357.0f,
										 240.0f, 256.0f, 273.0f, 290.0f, 307.0f,
										 275.0f, 292.0f, 309.0f,
										 275.0f, 292.0f, 309.0f, 326.0f, 343.0f, 359.0f,
										 318.0f, 318.0f, 318.0f, 318.0f, 318.0f, 318.0f, 318.0f, 318.0f,
										 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f, 360.0f,
										 230.0f, 230.0f };

	/*** Checkpoint Spawning ***/
	float checkpointXPos[kCheckpointAmount] = { 0.0f, 0.0f, 100.0f, 100.0f, 250.0f, 250.0f, 425.0f, 425.0f, 332.0f, 0.0f, 600.0f };	                      
	float checkpointYPos[kCheckpointAmount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60.0f, 60.0f, 0.0f, 0.0f, 0.0f, 0.0f -20.0f};

	float checkpointZPos[kCheckpointAmount] = { -10.0f , 100.0f, 100.0f, 0.0f, 50.0f, 250.0f, 265.0f, 180.0f, -120.0f, -120.0f, 600.0f};


	/*** tankSpawning Spawning ***/

	float tanksOneXPos[kWaterTankAmount] = { 20.0f, 40.0f, 60.0f, 80.0f, -10.0f, 20.0f, 50.0f, 70.0f,           
												110.0f, 100.0f, 100.0f, 100.0f, 100.0f, 87.0f, 113.0f, 
												80.0f, 100.0f, 120.0f, 140.0f, 160.0f, 160.0f, 140.0f, 120.0f, 
												440.0f, 430.0f, 420.0f, 410.0f, 400.0f, 390.0f, 380.0f, 370.0f, 360.0f, 350.0f,
												405.0f, 395.0f, 385.0f, 375.0f, 365.0f, 355.0f, 345.0f, 335.0f, 325.0f, 315.0f, 
												300.0f, 240.0f, 160.0f, 80.0f, 
												352.0f, 332.0f, 312.0f, 10.0f , -10.0f , -30.0f };


	float tanksOneZPos[kWaterTankAmount] = { 100.0f, 100.0f, 100.0f, 100.0f, 130.0f, 165.0f, 170.0f, 165.0f,  
												150.0f, 80.0f, 60.0f, 50.0f, 30.0f, 0.0f, 5.0f,	
												-10.0f, -40.0f, -50.0f, -40.0f, -40.0f, 0.0f, 0.0f, 0.0f, 
												160.0f, 130.0f, 100.0f, 70.0f, 40.0f, 10.0f, -20.0f, -50.0f, -80.0f, -110.0f, 
												160.0f, 130.0f, 100.0f, 70.0f, 40.0f, 10.0f, -20.0f, -50.0f, -80.0f, -110.0f  
												- 120.0f, -120.0f, -120.0f, -120.0f, -120.0f, 
												-140.0f, -160.0f, -180.0f, -180.0f, -160.0f, -140.0f };

	/******************************************************************************* Float Varibles ******************************************************************/
	float cameraUpCounter = 0.0f;							  // This used to tell when the camera has reached a certain point in the camera rotation of up and down
	float cameraMax = 10.0f;								  // This is the max the camera can rotate which will relate to the cameraCounter
	float cameraMin = -10.0f;								  // This is the cameras min when it comes to moving the camera left and right
	float cameraMinCounter = -5.0f;							  // This is the min in which the camera can rotate down
	float cameraSideCounter = 0.0f;							  // This is the counter for the left and right and will stop the camera when it reaches it limit
	float floorHeight = 0.0f;                                 // This is the default floor height for the game.
	float hoverCarStartingX = 0.0f;					          // This is the default hover car starting X position
	float hovercarStartingZ = -30.0f;				          // This is the default hover car starting Z position
	float carMatrix[4][4];									  // This is the CarMatrix numbers which are used in the matrix maths later
	float npcCarMatrix[4][4];                                 // This is the carmatrix for the NPC car
	float boostText = 0.0f;                                   // This is what the boostText is set to by default
	float bouncingCounter = 0.0f;							  // This is used to tell when the ball has reached a certin point when the car is hovering up and down
	float bouncingMax = 0.8f;								  // This is the max the car can bounce when its hovering up and down which is related to the bouncingCounter
	float dragValue = -2.0f;								  // This is the value in which the car will slow down by and this wil cause the drag effect
	float currentThrust = 0.0f;								  // This is the current thrust of the car and this will say when the car is moving and when it isn't
	float npcCurrentThrust = 0.0f;                            // This is the NPC's current thrust
	float reverseSpeed = 10000.0f;							  // This is the speed in which the car will travel by when the S key is pressed
	float speedIncrement = 30.0f;							  // This is how much the speed will increment over time
	float cameraMoveSpeed = 100.0f;							  // Camera's movement speed
	float carDummyRotSpeed = 100.0f;						  // The rotation speed of the car
	float countDown = 5.0f;									  // This is how long the boost takes to cool down
	float boostCountDown = 6.0f;                              // This is used for counting down the boost
	float rotateLimitMax = 15.0f;							  // This is the max the camera can rotate
	float rotateLimitCounter = 0.0f;						  // This is a counter for the rotation limit
	float speedGauageCounter = 0.0f;						  // This is a counter for the speed gauge
	float speedGauageMax = 140.0f;							  // This is the max the speed guage can go to
	float speedGuageMin = -50.0f;							  // This is the min the speed guage can go
	float currentHP = 100.0f;								  // This is the cars current health
	float boostIncrement = 10.0f;							  // This is the amount boost will increment the speed
	float npcRampUpDummy = 16.0f;							  // This is used for singling out one of the dummys for the UP cutscence for the NPC
	float npcRampAcrossDummy = 17.0f;						  // This is used for singling out one of the dummys for the Across cutscence for the NPC
	float npcRampFalseAcrossDummy = 18.0f;					  // This is used for singling out one of the dummys for the other across cutscence for the NPC
	float npcRampDownDummy = 21.0f;							  // This is used for singling out one of the dummys for the Down cutscence for the NPC
	float npcEndDummy = 34.0f;								  // This is used for singling out one of the dummys for the End cutscence for the NPC
	float barrierRaised = 55.0f;							  // This is used for the height of the barriers when they are raised
	float barrierLower = 50.0f;								  // This is used for the height of the barriers when they are lowered
	float thrustMax = 100.0f;								  // This is the max speed the hovercar can go
	float pitchEngOn = 2.0f;								  // This is the pitch volume of the engine while on
	float pitchEngOff = 1.0f;								  // This is the pitch volume of the engine while off
	float reverseMax = -50.0f;								  // This is the max speed the car can go in reverse
	float cutscenceSpeed = 50.0f;							  // This is the speed the cars go in the cutscence
	float rotateRightCounter = 0.0f;						  // This is the rotation counter for the lean of the car while turning right
	float rotateLeftCounter = 0.0f;							  // This is the rotation counter for the lean of the car while turning left
	float rotateUpCounter = 0.0f;							  // This is the rotation counter for the lean of the car while going forward
	float rotateDownCounter = 0.0f;							  // This is the rotation counter for the lean of the car while going backwards
	float dectionModifyerX = 2.0f;							  // These are modifyers passed down to the barrier function for collision on the barriers
	float dectionModifyerZ = 6.0f;							  // These are modifyers passed down to the barrier function for collision on the barriers
	float dectionModifyerY = 3.0f;							  // These are modifyers passed down to the barrier function for collision on the barriers
	float dectionModifyerRX = 6.0f;							  // These are modifyers passed down to the barrier function for collision on the barriers
	float dectionModifyerRZ = 2.0f;							  // These are modifyers passed down to the barrier function for collision on the barriers
	float dectionModifyerRY = 3.0f;							  // These are modifyers passed down to the barrier function for collision on the barriers
	float lifeTimeCounter = 0.0f;							  // This is a counter for the life time of the cross
	float lifeTimeMax = 2.0f;								  // This is the max the cross can live for
	float outerWallMaxUp = 458.0f;                            // This is the max the hovercar can go to on the Z axis before hitting the wall
	float outerWallMinDown = -198.0f;						  // This is the min the hovercar can go on the Z axis before hitting the wall
	float outerWallMinLeft = -48.0f;						  // This is the min the hovercar can go on the X axis before hitting the wall
	float outerWallMaxRight = 448.0f;						  // This is the max the hovercar can go on the X axis before htting the wall
	float leanMax = 0.3f;									  // This is the max the hovercar can lean when turning left or right
	float thrustTiltMax = 0.1f;								  // This is the max the hovercar can tilt when going forward or backwards
	float boostMaxThrust = 160.0f;							  // This is the max speed that the boost can go up too
	float boostRechargeMax = 101.0f;						  // This is the max speed while the boost is recharging
	float slowingDown = 30.0f;								  // This will make the hovercar's currentThrust slow down faster
	float barrierNorm = 21.0f;								  // This is used to indicate which barriers haven't been rotated
	float barrierRot = 45.0f;								  // This is used to indicate which barriers have been rotated
	float crossDeadY = -20.0f;								  // This is where the cross will be placed when dead or not used
	float maxHP = 100;										  // This is the max HP of the hovercar
	float cameraminNumber = -5.0f;							  // This is the min the camera can go
	float countDownNumber = 5.0f;							  // This is what the counter down will default too
	float npcCarXPos = 5.0f;								  // This is the X pos of the npc car
	float npcCarZPos = -60.0f;								  // This is the Z pos of the npc car
	float mouseMovementNum = 5.0f;                            // This is a increment to increase the speed the camera rotates at
															  
	/************************************************************** Text X's and Y's ************************************************/
															  
	int winnerNameX = 560;									  // This is the X pos for placing the winners name text
	int winnerNameY = 100;									  // This is the Y pos for placing the winners name text
	int winnerTextX = 560;									  // This is the X pos for placing the winner text
	int winnerTextY = 70;									  // This is the Y pos for placing the winner text
	int restartTextX = 50;									  // This is the X pos for placing the restart text
	int restartTextY = 620;									  // This is the Y pos for placing the restart text
	int boostRecharingTextX = 60;							  // This is the X pos for placing the recharge text
	int boostRecharingTextY = 675;							  // This is the Y pos for placing the recharge text
	int checkpointTextX = 50;								  // This is the X pos for placing the checkpoint text
	int checkpointTextY = 620;								  // This is the Y pos for placing the checkpoint text
	int leaderboardTextX = 560;								  // This is the X pos for placing the leaderboard text
	int leaderboardTextY = 40;								  // This is the Y pos for placing the leaderboard text
	int leaderboardNamesX = 620;							  // This is the X pos for placing the names on the leaderboard
	int firstX = 570;										  // This is the X pos for placing the names on the first part of the leaderboard
	int firstY = 60;										  // This is the Y pos for placing the names on the first part of the leaderboard
	int secondX = 570;										  // This is the X pos for placing the names on the second part of the leaderboard
	int secondY = 80;										  // This is the Y pos for placing the names on the second part of the leaderboard
	int titleX = 420;										  // This is the X pos for placing the title text
	int titleY = 20;										  // This is the Y pos for placing the title text
	int currentSongX = 654;									  // This is the X pos for placing the current song text
	int currentSongY = 114;									  // This is the Y pos for placing the current song text
	int songNameX = 654;									  // This is the X pos for placing the song name text
	int songNameY = 134;									  // This is the Y pos for placing the song name text
	int speedGuageNumbersX = 1180;							  // This is the X pos for placing the speed guage numbers
	int speedGuage0Y = 680;									  // This is the Y pos for placing the 0 text
	int speedGuage25Y = 654;								  // This is the Y pos for placing the 25 text
	int speedGuage50Y = 610;								  // This is the Y pos for placing the 50 text
	int speedGuage75Y = 570;								  // This is the Y pos for placing the 75 text
	int speedGuage100Y = 534;								  // This is the Y pos for placing the 100 text
	int startTextX = 70;									  // This is the X pos for placing the starting text
	int startTextY = 640;									  // This is the X pos for placing the starting text
	int countDownTextY = 640;								  // This is the Y pos for the countDowntext
	int countDownText3X = 120;								  // This is the X pos for the 3 text
	int countDownText2X = 140;								  // This is the X pos for the 2 text
	int countDownText1X = 160;								  // This is the X pos for the 1 text
	int countDownTextGoX = 180;								  // This is the X pos for the Go text
	int currentHPTextX = 50;								  // This is the X pos for the currentHP text
	int currentHPTextY = 600;								  // This is the Y pos for the currentHP text
	int engineOHX = 50;										  // This is the X pos for the engine overheating text
	int engineOHY = 640;									  // This is the Y pos for the engine overheating text
	int speedTextX = 1060;									  // This is the X pos for the speed text
	int speedTextY = 690;									  // This is the Y pos for the speed text
	int engineNotifyerX = 50;								  // This is the X pos for the engine text
	int engineNotifyerY = 660;								  // This is the Y pos for the engine text
	int lapTextX = 425;										  // This is the X pos for the lap text
	int lapTextY = 125;										  // This is the Y pos for the lap text
															  
	/************************************************************** int variables ****************************************************/
															  
	int speedGuageXPos = 1056;								  // This is the X pos for placing the speed guage
	int speedGuageYPos = 687;								  // This is the Y pos for placing the speed guage
	int cutScenceDummy1 = 1;								  // This is used to pick out indival dummys
	int cutScenceDummy2 = 2;								  // This is used to pick out indival dummys
	int cutScenceDummy3 = 3;								  // This is used to pick out indival dummys
	int boostCounterNumber = 6;								  // This is what the boost counter is set to by default
	int crossHeightModifyer = 6;							  // This is the height modifyer for the cross
	int Lap1Max = 11;										  // This is the max amount of checkpoints for lap 1
	int Lap2Max = 21;										  // This is the max amount of checkpoints for lap 2
	int Lap3Max = 31;										  // This is the max amount of checkpoints for lap 3
	int Lap4Max = 41;										  // This is the max amount of checkpoints for lap 4
	int Lap5Max = 51;										  // This is the max amount of checkpoints for lap 5
	int npcCheckpointAmount = 51;							  // This is the max amount of checkpoints for the NPC
	int checkpointSwitch = 10;								  // This will be used to switch the amount of checkpoints back to one
	int randSoundMath100 = 100;								  // This is used when it comes to randomising the noise for the sounds of impact
	int randSoundMath50 = 50;								  // This is used when it comes to randomising the noise for the sounds of impact
	int counterNumber0 = 0;									  // This is used for checking indival numbers
	int counterNumber1 = 1;									  // This is used for checking indival numbers
	int counterNumber2 = 2;									  // This is used for checking indival numbers
	int counterNumber3 = 3;									  // This is used for checking indival numbers
	int npcCheckpointCounterAmount = 0;                       // This is a counter which is used when the npc car goes through a checkpoint
	int npcCheckpointCounter = 0;							  // This is a counter which will go up for each checkpoint it goes through
	int playerCheckpointCounter = 0;						  // This is a counter which will go up for each checkpoint the player goes through 
	int checkpointOne = 1;									  // These are used to indenify which checkpoint will trigger in functions
	int checkpointTwo = 2;									  // These are used to indenify which checkpoint will trigger in functions
	int checkpointThree = 3;								  // These are used to indenify which checkpoint will trigger in functions
	int checkpointFour = 4;									  // These are used to indenify which checkpoint will trigger in functions
	int checkpointFive = 5;									  // These are used to indenify which checkpoint will trigger in functions
	int checkpointSix = 6;									  // These are used to indenify which checkpoint will trigger in functions
	int checkpointSeven = 7;								  // These are used to indenify which checkpoint will trigger in functions
	int checkpointEight = 8;								  // These are used to indenify which checkpoint will trigger in functions
	int checkpointNine = 9;									  // These are used to indenify which checkpoint will trigger in functions
	int matrixXNumber = 2;									  // This is used for the car matrix's in the hovercar and npc car
	int matrixYNumber = 2;									  // This is used for the car matrix's in the hovercar and npc car
	int barrierRLOne = 43;									  // This is one of the barriers which are lowered in the cut scence
	int	barrierRLTwo = 44;									  // This is one of the barriers which are lowered in the cut scence
	int boostCountDownNumber6 = 6;							  // These are used for the boost countdown
	int boostCountDownNumber5 = 5;							  // These are used for the boost countdown
	int boostCountDownNumber4 = 4;							  // These are used for the boost countdown
	int boostCountDownNumber3 = 3;							  // These are used for the boost countdown
	int boostCountDownNumber2 = 2;							  // These are used for the boost countdown
	int boostCountDownNumber1 = 1;							  // These are used for the boost countdown
	int countDownNumber4 = 4;								  // These are used for the count down when space is pressed
	int countDownNumber3 = 3;								  // These are used for the count down when space is pressed
	int countDownNumber2 = 2;								  // These are used for the count down when space is pressed
	int countDownNumber1 = 1;								  // These are used for the count down when space is pressed
	int currentWayPoint = 0;								  // This is the current waypoint for the npc car
	int speedText = 0;										  // This is the text for the speed guage
	int counterText = 0;									  // This is the text for the boost
	int HPText = 0;											  // This is the text for Hp

	/*******************************************************************IMesh's, Loading Meshes && IModels ******************************************************************/

	IMesh* carDummyMesh;									  // This is the mesh I will use for the car dummy model
	IMesh* hoverCarMesh;									  // This is the mesh I will use for the hover car
	IMesh* islesMesh;										  // This is the mesh I will use for the isles
	IMesh* wallMesh;										  // This is the mesh I will be using for the wall
	IMesh* checkpointMesh;									  // This is the mesh I will be using for the checkpoint
	IMesh* groundMesh;										  // This is the mesh I will use for the ground
	IMesh* skyBoxMesh;                                        // This is the mesh I will be using for the skybox
	IMesh* smallTankOneMesh;								  // This is the mesh for the small water tanks
	IMesh* floorMesh;										  // This is the mesh for the floor
	IMesh* rampFloorMesh;									  // This is the mesh for the ramp floor
	IMesh* outerWallMesh;									  // This is the mesh for the outer wall
	IMesh* barrierMesh;										  // This is the mesh for the barriers
	IMesh* npcCarMesh;										  // This is the mesh for the NPC car
	IMesh* wayPointsMesh;									  // This is the mesh for the way points
	IMesh* redCrossMesh;									  // This is the mesh for the red cross
	IMesh* squareCollisionsMesh;							  // This is the mesh for the square collisions
	IModel* redCross;										  // This is the Model I will use for the red cross
	IModel* skyBox;                                           // This is the Model I will use for the skybox
	IModel* ground;											  // This is the Model I will use for the ground
	IModel* carDummy;										  // This is the Model I will be using for the car dummy mesh
	IModel* cameraDummy;									  // This is the Model I will be using for the cameraDummy
	IModel* floor;                                            // This is the Model I will be using for the floor

	redCrossMesh = myEngine->LoadMesh("Cross.x");             // This will load in the mesh for the red cross
	barrierMesh = myEngine->LoadMesh("Barrier.x");			  // This will load in the mesh for the barrier
	smallTankOneMesh = myEngine->LoadMesh("TankSmall1.X");	  // This will load in the mesh for the small water tanks
	rampFloorMesh = myEngine->LoadMesh("skyscraper02.X");	  // This will load in the mesh for the ramp floors
	wayPointsMesh = myEngine->LoadMesh("Dummy.x");			  // This will load in the mesh for the dummys
	floorMesh = myEngine->LoadMesh("Cube.X");				  // This will load in the mesh for the floor
	npcCarMesh = myEngine->LoadMesh("race2.x");				  // This will load in the mesh for the NPC car
	outerWallMesh = myEngine->LoadMesh("skyscraper02.X");	  // This will load in the mesh for the outer walls
	carDummyMesh = myEngine->LoadMesh("Dummy.x");			  // This will load in the mesh for the dummy's
	skyBoxMesh = myEngine->LoadMesh("Skybox 07.x");           // This will load in the mesh for the skybox
	islesMesh = myEngine->LoadMesh("IsleStraight.x");		  // This will load in the mesh for the Isles
	wallMesh = myEngine->LoadMesh("Wall.x");				  // This will load in the mesh for the wall
	checkpointMesh = myEngine->LoadMesh("Checkpoint.x");	  // This will load in the mesh for the checkpoint
	hoverCarMesh = myEngine->LoadMesh("race2.x");			  // This will load in the mesh for the hoverCar
	groundMesh = myEngine->LoadMesh("ground.x");			  // This will load in the mesh for the Ground
	squareCollisionsMesh = myEngine->LoadMesh("Cube.X");      // This will load in the mesh for the squareCollisions

	/****************************************************************************** UI Setup ************************************************************************/
	ISprite* leftUI;                                                            // This is a sprite used for the left UI
	leftUI = myEngine->CreateSprite("Left UI.png", 0, 440);						// This is will create the spite
	ISprite* rightUI;															// This is a sprite used for the right UI
	rightUI = myEngine->CreateSprite("Right UI.png", 880, 338);					// This is will create the spite
	ISprite* upperUI;															// This is a sprite used for the upper UI
	upperUI = myEngine->CreateSprite("Upper UI.png", 225, 0);					// This is will create the spite
	ISprite* speedGauage;														// This is a sprite used for the speed guage
	speedGauage = myEngine->CreateSprite("Speed Gage.png", 1056, 687, 0.5);		// This is will create the spite

	/******************************************************************************* Font loading *****************************************************************/
	IFont* uiFont = myEngine->LoadFont("Comic Sans MS", 24);                    // This is the font I will be using

	/******************************************************************************* Camera Setup******************************************************************/
	ICamera* myCamera;                                        // This will be where the camera is loaded
	myCamera = myEngine->CreateCamera(kManual);               // This creates the camera and set it to a kManual so it doesn't move
	float cameraThirdPersonPos[3] = { 0.0f, -4.0f, -8.0f };	  // These are the X, Y and Z positions of the third person camera
	float cameraFirstPersonPos[3] = { 0.0f, -7.0f, 36.0f };	  // These are the X, Y and Z positions of the first person camera
	int arrayNumberZero = 0;								  // This is the number zero of an array which is used for getting rid of magic numbers
	int arrayNumberOne = 1;									  // This is the number One of an array which is used for getting rid of magic numbers
	int arrayNumberTwo = 2;									  // This is the number Two of an array which is used for getting rid of magic numbers


	/******************************************************************************* Creating Objects ******************************************************************/
	carDummy = carDummyMesh->CreateModel(hoverCarStartingX, floorHeight, hovercarStartingZ);			       // This will create the carDummy at the X, Y and Z positions given 
	cameraDummy = carDummyMesh->CreateModel();															       // This will creat the cameraDummy which will later be attached to the carDummy
	playerCar.hoverCar = hoverCarMesh->CreateModel();								                           // This will make the hovercarMesh at the set X,Y and Z location given
	npcCar.npcCar = npcCarMesh->CreateModel(npcCarXPos, floorHeight, npcCarZPos);                              // This will create the NPC car at the set X, Y and Z pos's given
	npcCar.npcCar->SetSkin("purple.jpg");                                                                      // This will set the skin of the car to purple
	ground = groundMesh->CreateModel(0.0f, 0.0f, 0.0f);                                                        // This will make the groundMesh at the set X,Y, and Z location given.
	skyBox = skyBoxMesh->CreateModel(0.0f, -1000.0f, 0.0f);                                                    // This will make the skyboxMesh at the set X,Y and Z location given.
	playerCar.hoverCar->AttachToParent(carDummy);     						                                   // This attaches the Camera to the hover car to make the chase camera 
	cameraDummy->AttachToParent(carDummy);																       // This attaches the cameraDummy to the carDummy
	myCamera->AttachToParent(cameraDummy);																       // This attaches the camera to the cameraDummy 
	cameraDummy->SetLocalPosition(cameraThirdPersonPos[0], cameraThirdPersonPos[1], cameraThirdPersonPos[2]);  // Set the camera Position for 3rd person
	redCross = redCrossMesh->CreateModel(checkpointXPos[0], crossDeadY, checkpointZPos[0]);                           // This will create the red cross at the given parametters
	redCross->Scale(0.8);																					   // This will scale the cross down slightly

	/**** Checkpoint Legs ****/
	float checkPointLegRadius[kCheckPointLegAmount];														     // This is used for collision on the checkpoint legs

	/**** HoverCar ****/
	float hoverCarRadius = 2;																				     // This is the hover car's radius
	float hoverCarCurrentPosx = carDummy->GetX();															     // This will be used to get the carDummy's current pos on the X
	float hoverCarCurrentPosy = carDummy->GetY();                                                                // This will be used to get the carDummy's current pos on the Y
	float hoverCarCurrentPosz = carDummy->GetZ();		                                                         // This will be used to get the carDummy's current pos on the Z
	float npcCarCurrentPosx = npcCar.npcCar->GetX();															 // This will be used to get the NPC's current X
	float npcCarCurrentPosy = npcCar.npcCar->GetY();															 // This will be used to get the NPC's current Z

	for (int i = 0; i < kSquareAmount; i++)  // This is used for spawning in the squareCollisions
	{
		squareCollisions[i].squareCollisions = squareCollisionsMesh->CreateModel(squareXPos[i], squareYPos[i], squareZPos[i]);
	}

	for (int i = 0; i < kIslesAmount; i++)		                                                                   // This is for spawning in the Isles 													  
	{
		isles[i].isles = islesMesh->CreateModel(islesXPos[i], floorHeight, islesZPos[i]);
	}
	for (int i = 0; i < kWayPointAmount; i++)                                                                      // This is for spawning in the waypoints
	{
		waypoints[i].wayPoints = wayPointsMesh->CreateModel(waypointXPos[i], waypointYPos[i], waypointZPos[i]);
	}
	for (int i = 0; i < kOuterWallAmount; i++)                                                                    // This is for spawning in the outer walls
	{
		outerwalls[i].outerWall = outerWallMesh->CreateModel(outerWallXPos[i], floorHeight, outerWallZPos[i]);
		/*** This is used for scaling down the outer walls ***/
		outerwalls[i].outerWall->ScaleY(0.07);
		outerwalls[i].outerWall->ScaleZ(0.06);
		outerwalls[i].outerWall->ScaleX(8.0f);
		if (i > 1.0f) // This is used to rotate the outer walls and scale up slightly
		{
			outerwalls[i].outerWall->RotateY(90.0f);
			outerwalls[i].outerWall->ScaleX(1.2f);
		}
	}
	for (int i = 0; i < kRampFloorAmount; i++) // This is used for spawning in the ramp floors
	{
		rampFloor[i].rampFloor = rampFloorMesh->CreateModel(rampFloorXPos[i], rampFloorYPos[i], rampFloorZPos[i]);
		/*** This is a mix of rotation and scaling so that the ramps are the way I want them ***/
		rampFloor[i].rampFloor->Scale(0.35);
		rampFloor[i].rampFloor->ScaleY(2);
		rampFloor[i].rampFloor->RotateX(90);
		rampFloor[i].rampFloor->RotateY(180);
		rampFloor[i].rampFloor->ScaleX(2);
		rampFloor[0].rampFloor->RotateX(45);
	}
	rampFloor[0].rampFloor->RotateX(230);
	rampFloor[2].rampFloor->RotateY(90);
	rampFloor[3].rampFloor->RotateY(90);
	rampFloor[5].rampFloor->RotateX(140);

	for (int i = 0; i < kCutScenceDummy; i++) // This is used to spawn in the cutscence dummys
	{
		cutScenceDummy[i].cutScenceDummy = carDummyMesh->CreateModel(cutScenceDummyXPos[i], cutScenceDummyYPos[i], cutScenceDummyZPos[i]);
	}

	for (int i = 0; i < kBarrierAmount; i++) // This is used to spawn in the barriers
	{
		barrier[i].barrier = barrierMesh->CreateModel(BarrierXPos[i], BarrierYPos[i], BarrierZPos[i]);
		if (i > 21) // This is used to rotate the barriers if i is greater then 21
		{
			barrier[i].barrier->RotateLocalY(90);
		}
	}

	for (int i = 0; i < kWaterTankAmount; i++) // This is used for spawning in the water tanks
	{
		smallTanksOne[i].smallTankOne = smallTankOneMesh->CreateModel(tanksOneXPos[i], floorHeight, tanksOneZPos[i]);
	}
	for (int i = 0; i < kWallAmount; i++)	                                                                     // This is for spawning in the Walls							 										  
	{
		walls[i].wall = wallMesh->CreateModel(wallXPos[i], floorHeight, wallZPos[i]);
	}
	for (int i = 0; i < kCheckpointAmount; i++)		                                                             // This is for spawning in the checkpoints														  
	{
		checkpoints[i].checkpoints = checkpointMesh->CreateModel(checkpointXPos[i], checkpointYPos[i], checkpointZPos[i]);
		checkpoints[i].checkpoints->Scale(1.5f);

	}		                                                                                                    // This is for spawning in the checkpoints														  
	{
		floor = floorMesh->CreateModel(200, -249.9f, 130);                                                      // This will create the floor
		floor->Scale(50);																					    // This will scale the floor up
		floor->ScaleZ(1.3);																					    // This will scale the floor up more on the Z																		// This will set the skin of the floor
	}

	/******************************************************* Rotation and scaling ******************************************************/
	checkpoints[4].checkpoints->RotateLocalX(-25.0f);
	checkpoints[4].checkpoints->ScaleY(1.6);

	smallTanksOne[12].smallTankOne->RotateLocalX(30);
	smallTanksOne[12].smallTankOne->SetY(-5);
	smallTanksOne[11].smallTankOne->RotateLocalX(-30);
	smallTanksOne[11].smallTankOne->SetY(-5);
	smallTanksOne[10].smallTankOne->RotateLocalX(30);
	smallTanksOne[10].smallTankOne->SetY(-5);
	smallTanksOne[9].smallTankOne->RotateLocalX(-30);
	smallTanksOne[9].smallTankOne->SetY(-5);
	smallTanksOne[13].smallTankOne->SetLocalY(-3.5);
	smallTanksOne[14].smallTankOne->SetLocalY(-3.5);

	walls[33].wall->RotateY(90);
	walls[32].wall->RotateY(90);
	walls[34].wall->RotateY(90);
	walls[35].wall->RotateY(90);
	walls[36].wall->RotateY(90);
	walls[37].wall->RotateY(90);
	walls[38].wall->RotateY(90);
	walls[39].wall->RotateY(90);

	myEngine->Timer();                          // This tells the engine to get the Timer opertor

	// The main game loop, repeat until engine is stopped												  
	while (myEngine->IsRunning())
	{

		// Draw the scene																				  
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		/***************************************************************************** Text Update ***********************************************************************************/
		uiFont->Draw("Racing Car Game - By Danny Patrick Kavanagh XD", titleX, titleY, kWhite);  // These are texts that will be put on the screen
		uiFont->Draw("* :Current Song: *", currentSongX, currentSongY, kWhite);							  // These are texts that will be put on the screen
		uiFont->Draw("* A Journey Awaits *", songNameX, songNameY, kWhite);							  // These are texts that will be put on the screen
		uiFont->Draw("0    mph", speedGuageNumbersX, speedGuage0Y, kWhite);									  // These are texts that will be put on the screen
		uiFont->Draw("25  mph", speedGuageNumbersX, speedGuage25Y, kWhite);										  // These are texts that will be put on the screen
		uiFont->Draw("50  mph", speedGuageNumbersX, speedGuage50Y, kWhite);										  // These are texts that will be put on the screen
		uiFont->Draw("75  mph", speedGuageNumbersX, speedGuage75Y, kWhite);										  // These are texts that will be put on the screen
		uiFont->Draw("100 mph", speedGuageNumbersX, speedGuage100Y, kWhite);										  // These are texts that will be put on the screen

		if (pause == false)
		{
			float frameTime = myEngine->Timer();															        // This set the value frameTime so it will get the Timer opertor
			hoverCarCurrentPosx = carDummy->GetX();																	// This will get the dummyCar's X ever second for collision purposes
			hoverCarCurrentPosz = carDummy->GetZ();																	// This will get the dummyCar's Z ever second for collision purposes
			hoverCarCurrentPosy = carDummy->GetY();                                                                 // This will get the dummyCar's Y ever second for collision purposes


			/**************************************************************************** Vector Movement ********************************************************************************/

			carDummy->GetMatrix(&carMatrix[0][0]);															  // This will get the car matrixes
			facingVector = sVector(carMatrix[matrixXNumber][0], 0, carMatrix[matrixXNumber][matrixYNumber]);  // This will set the vector for car dummy to carMatrixes on the X and Y xais
			float vectorLength = sqrt((facingVector.X * facingVector.X) + (facingVector.Z * facingVector.Z)); // This float will square root the facingVector of X and Z which first will
																											  // times them together for both X and Z and then add them.  
			normFacing = sVector(facingVector.X / vectorLength, 0, facingVector.Z / vectorLength);			  // This will get the normliesed vector which will be used for the movement

			normFacing.X *= currentThrust * frameTime;										 // This will setup the normfacing vector for X so it can work with the current thurst
			normFacing.Z *= currentThrust * frameTime;										 // This will setup the normfacing vector for Z so it can work with the current thrust			  
			dragVector.X = Momentum.X * dragValue * frameTime;								 //	This will setup the drag vector so that it works with the momentum in the X		  
			dragVector.Z = Momentum.Z * dragValue * frameTime;								 // This will setup the drag vector so that it works with the momentum in the Z		  
			Momentum = Momentum + normFacing + dragVector;									 // This will setup the momentum which will be a combination of the normfacing vector and the drag

			npcCar.npcCar->GetMatrix(&npcCarMatrix[0][0]);									                  // This will get the car matrixes
			npcFacingVector = sNPCVector(npcCarMatrix[matrixXNumber][0], 0, npcCarMatrix[matrixXNumber][matrixYNumber]);     // This will set the vector for car dummy to carMatrixes on the X and Y xais
			float npcVectorLength = sqrt((npcFacingVector.X * npcFacingVector.X) + (npcFacingVector.Z * npcFacingVector.Z)); // This float will square root the facingVector of X and Z which first will
																											  // times them together for both X and Z and then add them.  
			npcNormFacing = sNPCVector(npcFacingVector.X / npcVectorLength, 0, npcFacingVector.Z / npcVectorLength);			  // This will get the normliesed vector which will be used for the movement

			npcNormFacing.X *= npcCurrentThrust * frameTime;										 // This will setup the normfacing vector for X so it can work with the current thurst
			npcNormFacing.Z *= npcCurrentThrust * frameTime;										 // This will setup the normfacing vector for Z so it can work with the current thrust			  
			npcDragVector.X = npcMomentum.X * dragValue * frameTime;								 //	This will setup the drag vector so that it works with the momentum in the X		  
			npcDragVector.Z = npcMomentum.Z * dragValue * frameTime;								 // This will setup the drag vector so that it works with the momentum in the Z		  
			npcMomentum = npcMomentum + npcNormFacing + npcDragVector;									 // This will setup the momentum which will be a combination of the normfacing vector and the drag

			/********************************************************************** Starting States *************************************************************/
			switch (StartStates) // This is a switch statement for starting the game
			{
			case StandBy: // In this state nothing will happen apart from the cars bouncing up and down
			{
				uiFont->Draw("Press Space To Start", startTextX, startTextY, kWhite); // Text to ask the user to start the game

				if (myEngine->KeyHit(startKey)) // When the start key is pressed it will move to the countDown state
				{
					StartStates = CountDown;
				}
			}
			break;
			case CountDown: // This state is for counting down for the game to start
			{
				countDown -= frameTime; // This will make the countDown go down via frame time so that it goes down in seconds
				if (countDown >= countDownNumber4) // If the countdown is greater or equal to four it will play the following code
				{
					if (finalSound.getStatus() == finalSound.Stopped) // This checks to see if the sound has been played
					{
						finalSound.play(); // This will play the final sound
					}
					else
					{

					}
					uiFont->Draw("3", countDownText3X, countDownTextY, kWhite); // Simple text to display the count down
				}
				else if (countDown >= countDownNumber3) // If the count down is greater or equal to 3 the following code with play
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has been played
					{
						finalSound.play(); // This will play the final sound
					}
					else
					{

					}
					uiFont->Draw("2", countDownText2X, countDownTextY, kWhite); // Simple text to display the countdown
				}
				else if (countDown >= countDownNumber2) // If the countdown is greater or equal to 2 the following code will play
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sounds has been played
					{
						finalSound.play(); // This will play the final sound
					}
					else
					{

					}
					uiFont->Draw("1", countDownText1X, countDownTextY, kWhite); // simple text to display the countdown
				}
				else if (countDown >= countDownNumber1) // if the count down is greater or equal to one the following code will play
				{
					if (finalSound.getStatus() == finalSound.Stopped) // This will check to see if sound has been played
					{
						finalSound.play(); // This will play the final sound
					}
					else
					{

					}
					uiFont->Draw("Go!", countDownTextGoX, countDownTextY, kWhite); // Simple text to let the user know they can go
				}
				else
				{
					finalSound.play(); // This will play the final sound
					StartStates = Go; // Switchs to the go state
				}
			}
			break;
			case Go: // In the Go state all the main piece of code will play so the user can play the game
			{
				/******************************************************************* Text in Main State *************************************************************************************/
				HPText = currentHP;                                      // This will set the HP text to the current HP
				stringstream currentHPText;								 // This will be the text that will be displayed on screen
				currentHPText << "CurrentHP: " << HPText;				 // This will set the currentHPText so that it will out the HP Text
				uiFont->Draw(currentHPText.str(), currentHPTextX, currentHPTextY, kWhite);		 // This will output the HP text

				counterText = boostText;                              // This sets the counterText to countUp
				stringstream counter;								// This will display the text to the screen
				counter << "Engine Overheating: " << counterText;	// This will set counter the text given and counterText
				uiFont->Draw(counter.str(), engineOHX, engineOHY, kWhite);		// This will then display counter text


				speedText = currentThrust;                            // This will set the speed text to the current Thrust
				stringstream carSpeed;						          // This will display the text on the screen
				carSpeed << "Speed: " << speedText;			          // This will set the carSpeed to the text and speed text
				uiFont->Draw(carSpeed.str(), speedTextX, speedTextY, kWhite);	  // This will output the speed text onto the screen

				/************************************************************* NPC waypoint System ****************************************************************************/
				npcCar.npcCar->LookAt(waypoints[currentWayPoint].wayPoints);               // This will make the npc car look at the current dummy
				if (wayPointDection(waypoints[currentWayPoint].wayPoints, npcCar.npcCar))  // This will check to see if the npc car and the current dummy have made contact
				{
					wayPointChange = true;						 						   // This will set the waypoint change to true
					npcCar.npcCar->LookAt(waypoints[currentWayPoint].wayPoints);		   // This will make the npc car look at the next dummy
				}
				if (currentWayPoint == npcRampUpDummy)									   // This checks to see if the current waypoint is equal to 16
				{
					npcGoingUp = true;													   // This will make the npc car go into the ramp cutscence
				}
				if (currentWayPoint == npcRampAcrossDummy)							       // This checks to see if the current waypoint is equal to 17
				{
					npcGoingAcross = true;												   // This will make the npc go into the next part of the cutscence
				}
				if (currentWayPoint == npcRampFalseAcrossDummy)			                   // This will check to see if the current waypoint is equal to 18
				{
					npcGoingAcross = false;                                                // This will make the npc car come out of the cutscence
					barrier[barrierRLOne].barrier->SetLocalY(barrierRaised);               // This will raise the barrier so the car can't get out
					barrier[barrierRLTwo].barrier->SetLocalY(barrierRaised);               // This will	raise the barrier so the car can't get out
				}
				if (currentWayPoint == npcRampDownDummy)                                   // This will check to see if the current way point is equal to 21
				{
					npcGoingDown = true;												   // This make the npc car go into the next part of the cutscence
				}
				if (currentWayPoint == npcEndDummy)										   // This will check to see if the current waypoint is equal to 34
				{
					npcCarReset = true;													   // This will set the npc car reset to true
				}

				/****************************************************************** Outerwalls collision  ************************************************************************/

				if (carDummy->GetX() >= outerWallMaxUp)
				{
					Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
					Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
				}
				if (carDummy->GetX() <= outerWallMinLeft)
				{
					Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
					Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
				}
				if (carDummy->GetZ() <= outerWallMinDown)
				{
					Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
					Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
				}
				if (carDummy->GetZ() >= outerWallMaxRight)
				{
					Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
					Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
				}

				/******************************************************************* Important Keys *****************************************************************************************/

				float mouseMovement = myEngine->GetMouseMovementX(); // This will get the mousements so that you can rotate the camera with the mouse pad
				cameraDummy->RotateLocalY(mouseMovement * (frameTime * mouseMovementNum)); // This will allow the user to rotate around the car with the mouse pad
				if (goingUp == false || goingAcross == false || goingDown == false || lookAtOnce == false)
					// This is a check to make sure that none of the cut scence booleans are true
				{
					if (myEngine->KeyHeld(thrustControl)) // This will make the hover car go forward
					{
						if (rotateUpCounter <= thrustTiltMax) // This will check to see if the counter is greater then the max
						{
							playerCar.hoverCar->RotateLocalX(-carDummyRotSpeed * frameTime); // This will rotate the hovercar forwards
							rotateUpCounter += frameTime; // This will increase the counter
						}

						if (currentThrust <= thrustMax) // This is the limit of how fast the car can go
						{
							currentThrust += speedIncrement * frameTime; // This is the movement of the car and is moved via frametime
						}
						engineSound.setPitch(pitchEngOn); // This sets the pitch of the noise while the engine is running

						if (speedGauageCounter <= speedGauageMax) // This checks to see if the speedgaugecounter is less then the max if so the code will follow
						{
							speedGauage->MoveY(-speedIncrement * frameTime); // This will move the speed gauage up in the Y
							speedGauageCounter += speedIncrement * frameTime; // This will incress the speed gauage counter
						}
					}
					else if (rotateUpCounter >= 0) // This will check to see if the counter is greater or equal to 0
					{
						playerCar.hoverCar->RotateLocalX(carDummyRotSpeed * frameTime); // This will rotate the car back to its default rotation
						rotateUpCounter -= frameTime; // This will subtract off the counter
					}
					else if (myEngine->KeyHeld(reverseControl)) // This will make the hover car be able to go backwards
					{
						if (rotateDownCounter >= -thrustTiltMax) // This will check to see if the hovercars rotation is greater then the min
						{
							playerCar.hoverCar->RotateLocalX(carDummyRotSpeed * frameTime); // This will rotate the car backwards
							rotateDownCounter -= frameTime; // This will subtract to the counter
						}

						if (currentThrust >= reverseMax) // This checks to see if the speed is still above the min if so the code will follow
						{
							currentThrust -= speedIncrement * frameTime; // This will make the hovercar go backwards via frametime
						}
					}
					else if (rotateDownCounter <= 0) // This will check to see if the counter is greater or equal to 0
					{
						playerCar.hoverCar->RotateLocalX(-carDummyRotSpeed * frameTime); // This will rotate the car back to its rotation
						rotateDownCounter += frameTime; // This will plus the counter
					}
					else
					{
						if (currentThrust > 0) // This will make the car slow down if its not moving and above 0
						{
							currentThrust -= speedIncrement * frameTime; // This will slow down the car via frametime
						}
						if (currentThrust < 0) // This will make the car slowly stop if its not moving and below 0
						{
							currentThrust += speedIncrement * frameTime; // This will make the car slowly stop via frametime
						}

						engineSound.setPitch(pitchEngOff); // This lowers the pitch when the car isn't being moved by the W key
						if (speedGauageCounter >= speedGuageMin) // This will check to see if the speedGuageCounter is greater or equal then the min
						{
							speedGauage->MoveY(speedIncrement * frameTime); // This will move the speedGuage go down
							speedGauageCounter -= speedIncrement * frameTime; // This will make the speedGuage counter go down
						}
					}
					if (myEngine->KeyHeld(rightRotateControl)) // When this button is pressed it will rotate to the right
					{
						carDummy->RotateY(carDummyRotSpeed * frameTime); // This will rotate the car to the right via frametime
						if (rotateRightCounter <= leanMax) // This will check to see if the counter is greater or equal to the max
						{
							playerCar.hoverCar->RotateLocalZ(-carDummyRotSpeed * frameTime); // This will rotate the hovercar to the right
							rotateRightCounter += frameTime; // This will plus on the counter
						}
					}
					else if (rotateRightCounter >= 0) // This will check to see if the counter is less or equal to 0
					{
						playerCar.hoverCar->RotateLocalZ(carDummyRotSpeed * frameTime); // This will rotate the hovercar back to its rotation
						rotateRightCounter -= frameTime; // This will subtract on the counter
					}

					if (myEngine->KeyHeld(leftRotateControl)) // When this button is pressed it will roate to the left
					{
						carDummy->RotateY(-carDummyRotSpeed * frameTime); // When this button is pressed it will rotate to the left via frametime
						if (rotateLeftCounter >= -leanMax) // This checks to see if the counter is greater or equal to the min
						{
							playerCar.hoverCar->RotateLocalZ(carDummyRotSpeed * frameTime); // This will rotate the hovercar to the left
							rotateLeftCounter -= frameTime; // This will subtract on the counter
						}
					}
					else if (rotateLeftCounter <= 0) // This will check to see if the counter is less or equal to 0
					{
						playerCar.hoverCar->RotateLocalZ(-carDummyRotSpeed * frameTime); // This will rotate the hovercar back to its rotation
						rotateLeftCounter += frameTime; // This will plus on the counter
					}
					if (StartStates = Go) // If the current state is the go state the following code will run
					{
						if (npcCurrentThrust <= thrustMax) // This sets a max the NPC car can go
						{
							npcCurrentThrust += speedIncrement * frameTime; // This will move the car forward via frametime so it will slowly gain speed
						}
					}
					if (npcGoingUp == false && npcGoingDown == false) // This will check to see if either or both booleans are false
					{
						npcCar.npcCar->Move(npcMomentum.X * frameTime, 0, npcMomentum.Z * frameTime); // This will move the NPC car forward via momentum and frametime
					}
					if (boostRecharge == false) // This checks to see if the boolean is false
					{
						if (myEngine->KeyHeld(boostyKey)) // This key will triger the boost
						{
							if (warning.getStatus() == warning.Stopped) // This will check to see if sound has been played
							{
								warning.play(); // This will play the final sound
							}
							if (currentThrust <= boostMaxThrust) // This checks to see if the boost is less or equal to the most boost
							{
								currentThrust += boostIncrement * frameTime; // This will increase the speed using the boost
								boostText += frameTime; // This will increase the boost text via frametime
							}
						}
					}
					if (counterText == counterNumber0) // This checks to see if the counter text is equal to 0
					{
						uiFont->Draw(("ENGINE STABLE"), engineNotifyerX, engineNotifyerY, kWhite);		// This will then display counter text
					}
					if (counterText == counterNumber1) // This checks to see if the counter text is equal to 1
					{
						uiFont->Draw(("ENGINE OVERHEATING"), engineNotifyerX, engineNotifyerY, kWhite);		// This will then display counter text
					}
					if (counterText == counterNumber2) // This checks to see if the counter text is equal to 2
					{
						uiFont->Draw(("ENGINE AT CRITICAL"), engineNotifyerX, engineNotifyerY, kWhite);		// This will then display counter text
					}
					if (counterText == counterNumber3) // This checks to see if the counter text is equal to 3
					{
						if (currentThrust > boostRechargeMax) // This checks to see if the currentThrust is greater then the max
						{
							currentThrust -= slowingDown * frameTime; // This will decrease the speed quicker when the boost has overheated
						}
						uiFont->Draw(("ENGINE OVERHEATED"), engineNotifyerX, engineNotifyerY, kWhite);		// This will then display counter text
						boostRecharge = true; // This sets the boolean to true
					}
				}
				if (myEngine->KeyHit(firstPersonCam)) // When the firstpersonCam button is pressed the following code will run
				{
					cameraState = firstPerson; // Changes to the first person state
					cameraDummy->SetLocalPosition(cameraFirstPersonPos[arrayNumberZero], cameraFirstPersonPos[arrayNumberOne], cameraFirstPersonPos[arrayNumberTwo]); // Set the camera Position for 3rd person
					cameraUpCounter = 0; // Resets the cameraUp counter
					cameraSideCounter = 0; // Resets the cameraSide counter
				}
				if (myEngine->KeyHit(thirdPersonCam)) // When the thrid person button is pressed the following code will run
				{
					cameraState = thirdPerson; // Changes the state to the third person state
					cameraDummy->SetLocalPosition(cameraThirdPersonPos[arrayNumberZero], cameraThirdPersonPos[arrayNumberOne], cameraThirdPersonPos[arrayNumberTwo]); // Set the camera Position for 3rd person
					cameraUpCounter = 0; // Resets the cameraUp counter
					cameraSideCounter = 0; // Resets the cameraSide counter
				}
				/**************************************************************************************** Collision has Occur **************************************************************************/

				if (rampCollisions == true) // This will check to see if the boolean for ramp collision is true
				{
					for (int i = 0; i < kSquareAmount; i++) // This will check for all the squares
					{
						/*** This is a function to see if there is a collision between the squaresCollisions and the hoverCar ***/
						if (sphereToBoxSquare(hoverCarRadius, hoverCarCurrentPosx, hoverCarCurrentPosz, squareCollisions[i].squareCollisions))
						{
							if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
							{
								impactSound.play(); // This will play the final sound
								impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
								currentHP--; // This will subtract from the current HP amount
							}
							Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
							Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
						}
					}
				}

				for (int i = 0; i < kWallAmount; i++) // This will check for all the walls
				{

					if (sphereToBox(hoverCarRadius, hoverCarCurrentPosx, hoverCarCurrentPosz, walls[i].wall)) // This is a function call to check collision with the car and walls
					{
						if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
						{
							impactSound.play(); // This will play the final sound
							impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
							currentHP--; // This will subtract from the current HP amount
						}
						Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
						Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					}
				}

				if (sphereToSphereNPCCar(npcCar.npcCar, playerCar.hoverCar))
				{
					if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
					{
						impactSound.play(); // This will play the final sound
						impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
						currentHP--; // This will subtract from the current HP amount
					}
					Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
					Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					npcMomentum.X = -npcMomentum.X; // This will make the car bounce back off in the current momentum in the X
					npcMomentum.Z = -npcMomentum.Z; // This will make the car bounce back off in the current momentum in the Z
				}

				for (int i = 0; i < barrierNorm; i++) // This will check all the barriers less then 21
				{
					if (sphereToBoxBarrier(hoverCarRadius, hoverCarCurrentPosx, hoverCarCurrentPosz, hoverCarCurrentPosy,
						barrier[i].barrier, dectionModifyerX, dectionModifyerZ, dectionModifyerY)) // This is a function to test collision between the car and barriers
					{
						if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
						{
							impactSound.play(); // This will play the final sound
							impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
							currentHP--; // This will subtract from the current HP amount
						}
						Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
						Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					}
				}

				for (int i = barrierNorm; i < barrierRot; i++) // This will check all the barriers
				{
					if (sphereToBoxBarrierR(hoverCarRadius, hoverCarCurrentPosx, hoverCarCurrentPosz, hoverCarCurrentPosy,
						barrier[i].barrier, dectionModifyerRX, dectionModifyerRZ, dectionModifyerRY)) // This is a function to test collision between the car and barriers
					{
						if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
						{
							impactSound.play(); // This will play the final sound
							impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
							currentHP--; // This will subtract from the current HP amount
						}
						Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
						Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					}
				}

				for (int i = 0; i < kWaterTankAmount; i++) // This will check all the water tanks
				{
					if (sphereToSphereWT(smallTanksOne[i].smallTankOne, playerCar.hoverCar)) // This is a function to test collision between the hovercar and small tanks
					{
						if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
						{
							impactSound.play(); // This will play the final sound
							impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
							currentHP--; // This will subtract from the current HP amount
						}
						Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
						Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					}

				}
				for (int i = 0; i < kIslesAmount; i++) // This will check all the isles
				{
					if (sphereToBoxIsles(hoverCarRadius, hoverCarCurrentPosx, hoverCarCurrentPosz, isles[i].isles)) // This is function to test collisions between the car and isles
					{
						if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
						{
							impactSound.play(); // This will play the final sound
							impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
							currentHP--; // This will subtract from the current HP amount
						}
						Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
						Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					}
				}

				for (int i = 0; i < kCheckpointAmount; i++) // This will check all the checkpoints
				{
					if (sphereToSphere(checkpoints[i].checkpoints, playerCar.hoverCar)) // This is a function to test collision between the car and checkpoint legs
					{
						if (impactSound.getStatus() == impactSound.Stopped) // This will check to see if sound has been played
						{
							impactSound.play(); // This will play the final sound
							impactSound.setVolume(rand() % randSoundMath100 + randSoundMath50); // This will set the volume to a random amount
							currentHP--; // This will subtract from the current HP amount
						}
						Momentum.X = -Momentum.X; // This will make the car bounce back off in the current momentum in the X
						Momentum.Z = -Momentum.Z; // This will make the car bounce back off in the current momentum in the Z
					}
				}
				if (goingUp == false && goingDown == false) // This checks to see if the booleans for going up and down in the cutscences are false if so the code will follow
				{
					carDummy->Move(Momentum.X * frameTime, 0, Momentum.Z * frameTime); // This will move the car normal via momentum and frametime
				}
				for (int i = 0; i < kCheckpointAmount; i++) // This will check all the checkpoints
				{
					/*** This is a function to test point to box collision between the checkpoints and the npc ***/
					if (pointToBoxNPC(checkpoints[npcCheckpointCounterAmount].checkpoints, npcCar.npcCar))
					{
						npcCheckpointCounterAmount++; // This will increase the counter
						checkpointIncrese = true; // This will make the checkpoint increase to true
					}
				}
				if (npcCheckpointCounterAmount == checkpointSwitch) // This checks to see if the counter is greater or equal to the last checkpoint
				{
					npcCheckpointCounterAmount = 0; // This will set the counter back to 0
				}

				/***************************************************************  Checkpoint lap system **************************************************/
				switch (laps) // This is a switch statement for the laps
				{
				case lapZero: // This is a case for the first lap
				{
					uiFont->Draw("Current Lap = 1/5", lapTextX, lapTextY, kWhite);	 // This will display the current lap
					if (playerCheckpointCounter == Lap1Max) // This checks to see if the first lap has been completed
					{
						laps = lapOne; // This will change the lap to the next
					}
				}
				break;
				case lapOne: // This is a case for the second lap
				{
					uiFont->Draw("Current Lap = 2/5", lapTextX, lapTextY, kWhite);	 // This will display the current lap
					if (playerCheckpointCounter == Lap2Max) // This checks to see if the second lap has been completed
					{
						laps = lapTwo; // This will change the lap to the next
					}
				}
				break;
				case lapTwo: // This is a case for the third lap
				{
					uiFont->Draw("Current Lap = 3/5", lapTextX, lapTextY, kWhite);	 // This will display the current lap
					if (playerCheckpointCounter == Lap3Max) // This checks to see if the third lap has been completed
					{
						laps = lapThree; // this will change the lap to the next
					}
				}
				break;
				case lapThree: // This is a case for the fourth lap
				{
					uiFont->Draw("Current Lap = 4/5", lapTextX, lapTextY, kWhite);	 // This will display the current lap
					if (playerCheckpointCounter == Lap4Max) // This checks to see if the fourth lap is complete
					{
						laps = lapFour; // This will change the lap to the next
					}
				}
				break;
				case lapFour: // This is a case for the firth lap
				{
					uiFont->Draw("Current Lap = 5/5", lapTextX, lapTextY, kWhite);	 // This will display the current lap
					if (playerCheckpointCounter == Lap5Max) // This will check to see if the firth lap has been completed
					{
						laps = lapFive; // This will change the lap to the next
					}
				}
				break;
				case lapFive: // This is a case for the finish
				{
					uiFont->Draw("Current Lap = Finished", lapTextX, lapTextY, kWhite);	 // This will display the current lap
					if (playerCheckpointCounter >= Lap5Max) // this checks to see if the final lap has been crossed
					{
						laps = Finished; // this will change the lap to the next
					}
				}
				break;
				case Finished: // This is a case for what happens when the game has finished
				{
					if (playerCheckpointCounter >= Lap5Max) // This checks to see if the final lap has been crossed
					{
						raceFinished = true; // This will make the race Finished boolean to true
					}
				}
				break;
				}
				if (npcCheckpointCounter >= npcCheckpointAmount) // This will check to see if the npc Car has reached its last checkpoint
				{
					raceFinished = true; // This will make the race finished boolean to true
				}

				/************************************************************************** Switch Statement with Important keys ***********************************************************************************/
				
				switch (cameraState) // This is a switch statement for the camera changes
				{
				case thirdPerson: // This case statement is for the thirdperson camera 
				{
					if (myEngine->KeyHeld(cameraRoateUp)) // When this button is pressed the camera will move forward
					{
						if (cameraDummy->GetLocalZ() <= cameraMax) // This checks to see that the camera hasn't meet its max if so the code will run
						{
							cameraDummy->MoveLocalZ(cameraMoveSpeed * frameTime); // This will move the camera forward in the Z xais
							cameraUpCounter += cameraMoveSpeed * frameTime; // This will increase the camera counter
						}
					}
					if (myEngine->KeyHeld(cameraRoateLeft)) // Whenn this button is pressed it will move the camera to the left
					{
						if (cameraDummy->GetLocalX() >= cameraMin) // checks to see if the camera hasn't meet its min yet if so the code will run
						{
							cameraDummy->MoveLocalX(-cameraMoveSpeed * frameTime); // Moves the camera towards the left in the x xais
							cameraSideCounter -= -cameraMoveSpeed * frameTime; // This decreases the cameraside counter
						}

					}
					if (myEngine->KeyHeld(cameraRoateRight)) // When this button is pressed it will move the camera to the right
					{
						if (cameraDummy->GetLocalX() <= cameraMax) // This checks to see if the camera hasn't meet its max yet if so the following code will run
						{
							cameraDummy->MoveLocalX(cameraMoveSpeed * frameTime); // This will move the camera to the right in the x xais
							cameraSideCounter += cameraMoveSpeed * frameTime; // This will increase the side counter
						}
					}
					if (myEngine->KeyHeld(cameraRoateDown)) // When this button is pressed it will move the camera backwards
					{
						if (cameraDummy->GetLocalZ() >= cameraMinCounter) // this checks to see if the camera hasn't meet its min yet if so the following code will run
						{
							cameraDummy->MoveLocalZ(-cameraMoveSpeed * frameTime); // This will move the camera backwards in the Z xis
							cameraUpCounter -= cameraMoveSpeed * frameTime; // This will decrease the camera counter
						}
					}
					break;
				}
				case firstPerson: // This is a case for the first person camera
				{
					cameraDummy->ResetOrientation(); // This will reset the orientation of the camera
					break;
				}
				}
			}
			break;
			}
			/******************************************************************** LeaderBoard ********************************************************/
			uiFont->Draw("*LeaderBoard*", leaderboardTextX, leaderboardTextY, kWhite); // Displays text to the screen
			uiFont->Draw("1st =", firstX, firstY, kWhite);         // Displays text to the screen
			uiFont->Draw("2st =", secondX, secondY, kWhite);			// Displays text to the screen

			if (playerCheckpointCounter > npcCheckpointCounter) // This checks to see if the player's checkpoint value is highter then the npc's
			{
				leaderBoard = playerFirst; // This will set the player to be in first place
			}
			if (playerCheckpointCounter < npcCheckpointCounter) // This checks to see if the player's checkpoint value is less then the npc's
			{
				leaderBoard = playerSecond; // This will set the player to be in last place
			}

			switch (leaderBoard) // Switch statement used for the leader board
			{
			case playerFirst: // This is a case for when the player is first
			{
				uiFont->Draw("PLAYER", leaderboardNamesX, firstY, kWhite); // Displays text to the screen
				uiFont->Draw("NPC", leaderboardNamesX, secondY, kWhite); // Displays text to the screen

			}
			break;
			case playerSecond: // This is a case for when the player is last
			{
				uiFont->Draw("PLAYER", leaderboardNamesX, secondY, kWhite); // Displays text to the screen
				uiFont->Draw("NPC", leaderboardNamesX, firstY, kWhite); // Displays text to the screen
			}
			break;
			}

			switch (checkpointStates) // These are states for the checkpoints
			{
			case CheckpointZero: // This case is for the first checkpoint
			{
				uiFont->Draw("Current Checkpoint = 0", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[0].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[0], checkpointYPos[0] + crossHeightModifyer, checkpointZPos[0]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointOne; // This will move the checkpointstate to checkpoint One
					}
				}
				break;
			}
			case CheckpointOne: // This is a case for the second checkpoint
			{
				uiFont->Draw("Current Checkpoint = 1", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointOne].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointOne], checkpointYPos[checkpointOne] + crossHeightModifyer, checkpointZPos[checkpointOne]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointTwo; // This will move the checkpointstate to checkpoint Two
					}
				}
				break;
			}
			case CheckpointTwo: // This is case for the thrid checkpoint
			{
				uiFont->Draw("Current Checkpoint = 2", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointTwo].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointTwo], checkpointYPos[checkpointTwo] + crossHeightModifyer, checkpointZPos[checkpointTwo]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointThree; // This will move the checkpointstate to checkpoint Three
					}
				}
				break;
			}
			case CheckpointThree: // This is a case for the fourth checkpoint
			{
				uiFont->Draw("Current Checkpoint = 3", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointThree].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointThree], checkpointYPos[checkpointThree] + crossHeightModifyer, checkpointZPos[checkpointThree]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointFour; // This will move the checkpointstate to checkpoint fourth
					}
				}
				break;
			}
			case CheckpointFour: // This is a case of the firth checkpoint
			{
				uiFont->Draw("Current Checkpoint = 4", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointFour].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					carDummy->ResetOrientation(); // This resets the orientation of the hovercar
					playerCar.hoverCar->ResetOrientation(); // resets the hoverCar orientation
					rampCollisions = false; // This is turned false to make sure the collision don't occur
					goingUp = true; // This sets the going up boolean for the cut scence to true
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointFour], checkpointYPos[checkpointFour] + crossHeightModifyer, checkpointZPos[checkpointFour]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointFive; // This will move the checkpointstate to checkpoint Five
					}
				}
				break;
			}
			case CheckpointFive: // This is a case for the sixth checkpoint
			{
				uiFont->Draw("Current Checkpoint = 5", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointFive].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					carDummy->ResetOrientation(); // This resets the hovers cars orientation
					playerCar.hoverCar->ResetOrientation();
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointFive], checkpointYPos[checkpointFive] + crossHeightModifyer, checkpointZPos[checkpointFive]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointSix; // This will move the checkpointstate to checkpoint Six
					}
				}
				break;
			}
			case CheckpointSix: // This is a case for the seventh checkpoint
			{
				goingAcross = false; // This sets the going across boolean for the cut scence to false
				goingUp = false; // This sets the going across boolean for the cut scence to false
				barrier[barrierRLOne].barrier->SetLocalY(barrierRaised); // This will raise the barriers so that the user can't get out of the ramp
				barrier[barrierRLTwo].barrier->SetLocalY(barrierRaised); // This will raise the barriers so that the user can't get out of the ramp
				uiFont->Draw("Current Checkpoint = 6", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointSix].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					goingDown = true; // This will set the goingDown boolean to true for the cut scence 
					carDummy->ResetOrientation(); // This will reset the hover cars orientation
					playerCar.hoverCar->ResetOrientation();
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointSix], checkpointYPos[checkpointSix] + crossHeightModifyer, checkpointZPos[checkpointSix]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointSeven; // This will move the checkpointstate to checkpoint Seven
					}
				}
				break;
			}
			case CheckpointSeven: // This is a case for the eighth checkpoint
			{
				uiFont->Draw("Current Checkpoint = 7", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointSeven].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					lookAtOnce = false; // This sets the look at once boolean for the end of the cut scence to false
					rampCollisions = true; // This is turned true so that collisions can occur again
					playerCar.hoverCar->ResetOrientation();
					if (finalSound.getStatus() == finalSound.Stopped) // This checks to see if the sound has been played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointSeven], checkpointYPos[checkpointSeven] + crossHeightModifyer, checkpointZPos[checkpointSeven]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointEight; // This will move the checkpointstate to checkpoint Eight
					}
				}
				break;
			}
			case CheckpointEight: // This is a case for the nineth checkpoint
			{
				uiFont->Draw("Current Checkpoint = 8", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointEight].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					lookAtOnce = false; // This sets the look at once boolean for the end of the cut scence to false
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointEight], checkpointYPos[checkpointEight] + crossHeightModifyer, checkpointZPos[checkpointEight]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointNine; // This will move the checkpointstate to checkpoint Nine
					}
				}
				break;
			}
			case CheckpointNine: // This is a case for the tenth checkpoint
			{
				uiFont->Draw("Current Checkpoint = 9", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[checkpointNine].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[checkpointNine], checkpointYPos[checkpointNine] + crossHeightModifyer, checkpointZPos[checkpointNine]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointTen; // This will move the checkpointstate to checkpoint Ten
					}
				}
				break;
			}
			case CheckpointTen: // This is a case for the eleventh checkpoint
			{
				uiFont->Draw("Current Checkpoint = 10", checkpointTextX, checkpointTextY, kWhite); // Displays text to let the user know what checkpoint they are on
				if (pointToBox(checkpoints[0].checkpoints, playerCar.hoverCar)) // This is a function call to check collision between the checkpoint and the hovercar
				{
					if (finalSound.getStatus() == finalSound.Stopped) // checks to see if the sound has be played
					{
						finalSound.play(); // This plays the selcted sound
						redCross->SetPosition(checkpointXPos[0], checkpointYPos[0] + crossHeightModifyer, checkpointZPos[0]); // repostions the red cross to the current cross
						lifeTimeCounter = 0; // This will reset the lifeCounter
						crossDead = true; // This will make the lifeTime of the cross trigger
						playerCheckpointCounter++; // This will increase the playerCounter
						checkpointStates = CheckpointOne; // This will reset the checkpoint back to one
					}
				}
				break;
			}
			}

			if (crossDead == true)								  // This will check to see if the cross has begun dying
			{													  
				lifeTimeCounter += frameTime;					  // This will plus on the counter via frametime
			}													   
			if (lifeTimeCounter >= lifeTimeMax)					  // This will check to see if the counter is greater or equal to the max
			{													  
				redCross->SetPosition(0, crossDeadY, 0);				  // This will move the position of the cross so it looks to be dead
				crossDead = false;								  // This will set the boolean to false
			}													  
																  
			if (wayPointChange == true)                           // This will check to see if waypoint change is equal to true
			{
				currentWayPoint++;								  // This will increase the current waypoint by one
				wayPointChange = false;							  // This will set the waypoint change to false
			}

			if (npcCarReset == true)							  // This will check to see if the npc car reset is equal to true
			{
				currentWayPoint = 0;							  // This will make the current waypoint equal to 0
				npcCarReset = false;							  // This will set the npc car reset back to false
			}

			if (boostRecharge == true)
			{
				boostCountDown -= frameTime; // This will make the countDown go down via frame time so that it goes down in seconds
				if (boostCountDown >= boostCountDownNumber6) // If the countdown is greater or equal to four it will play the following code
				{
					uiFont->Draw("Boost Recharging = 5", boostRecharingTextX, boostRecharingTextY, kWhite); // Simple text to display the count down
				}
				else if (boostCountDown >= boostCountDownNumber5) // If the count down is greater or equal to 3 the following code with play
				{
					uiFont->Draw("Boost Recharging = 4", boostRecharingTextX, boostRecharingTextY, kWhite); // Simple text to display the countdown
				}
				else if (boostCountDown >= boostCountDownNumber4) // If the countdown is greater or equal to 2 the following code will play
				{
					uiFont->Draw("Boost Recharging = 3", boostRecharingTextX, boostRecharingTextY, kWhite); // simple text to display the countdown
				}
				else if (boostCountDown >= boostCountDownNumber3) // if the count down is greater or equal to one the following code will play
				{
					uiFont->Draw("Boost Recharging = 2", boostRecharingTextX, boostRecharingTextY, kWhite); // Simple text to let the user know they can go
				}
				else if (boostCountDown >= boostCountDownNumber2) // if the count down is greater or equal to one the following code will play
				{
					uiFont->Draw("Boost Recharging = 1", boostRecharingTextX, boostRecharingTextY, kWhite); // Simple text to let the user know they can go
				}
				else if (boostCountDown >= boostCountDownNumber1) // if the count down is greater or equal to one the following code will play
				{
					boostRecharge = false; // This will turn the boolean false when the countdown is done
					boostReset = true;	   // This will turn the boolean true when the countdown is done
				}
			}

			if (raceFinished == true)        // This will check to see if the boolean is true
			{								 
				pause = true;				 // This will set the pause boolean to true which will pause the game
			}								 
											 
			if (checkpointIncrese == true)	 // This will check to see if the checkpointIncrease is true
			{								 
				npcCheckpointCounter++;		 // This will add to the counter
				checkpointIncrese = false;	 // This will set it back to false
			}								 
											 
			if (boostReset == true)			 // This will check to see if the boostReset is true
			{								 
				boostText = 0;				                     // This will reset the boost text back to 0
				boostCountDown = boostCounterNumber;			 // This will reset the boostCountDown
				boostReset = false;			                     // This will set the boolean back to false;
			}

			if (goingUp == true) // This checks to see if goingUp boolean for the cut scence is true
			{
				carDummy->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the hovercar forward while in the cut scence
				carDummy->LookAt(cutScenceDummy[0].cutScenceDummy); // This will get the hover car to look at the first dummy
				if (pointToBoxCSD(playerCar.hoverCar, cutScenceDummy[0].cutScenceDummy)) // This is a function to test collisions between the dummy and hovercar
				{
					rotateLeftCounter = 0;   // This will reset the counter back to 0
					rotateRightCounter = 0;	 // This will reset the counter back to 0
					rotateUpCounter = 0;	 // This will reset the counter back to 0
					rotateDownCounter = 0;	 // This will reset the counter back to 0
					goingAcross = true; // This will making the going across boolean true
					goingUp = false; // This will set the goingUp boolean to false
				}
			}
			if (npcGoingUp == true) // This checks to see if goingUp boolean for the cut scence is true
			{
				npcCar.npcCar->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the hovercar forward while in the cut scence
				npcCar.npcCar->LookAt(waypoints[currentWayPoint].wayPoints); // This will get the hover car to look at the first dummy
				if (wayPointDection(waypoints[currentWayPoint].wayPoints, npcCar.npcCar)) // This is a function to test collisions between the dummy and hovercar
				{
					npcGoingAcross = true; // This will making the going across boolean true
					npcGoingUp = false; // This will set the goingUp boolean to false
				}
			}
			if (npcGoingAcross == true) // This will check to see if the goingAcross boolean is true
			{
				npcCar.npcCar->LookAt(waypoints[currentWayPoint].wayPoints); // This will make the hover car look at the dummy
				npcCar.npcCar->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the car forward
				barrier[barrierRLOne].barrier->SetLocalY(barrierLower); // This will lower the barriers so that car can get through
				barrier[barrierRLTwo].barrier->SetLocalY(barrierLower); // This will lower the barriers so that car can get through

			}
			if (goingDown == true) // This checks to see if the going down boolean is true
			{
				carDummy->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the hover car forward in the cut scence
				carDummy->LookAt(cutScenceDummy[cutScenceDummy2].cutScenceDummy); // This will make the hover car look at the dummy
				if (pointToBoxCSD(playerCar.hoverCar, cutScenceDummy[cutScenceDummy2].cutScenceDummy)) // This is a function to test collision between the hover car and dummy
				{
					rotateLeftCounter = 0;   // This will reset the counter back to 0
					rotateRightCounter = 0;	 // This will reset the counter back to 0
					rotateUpCounter = 0;	 // This will reset the counter back to 0
					rotateDownCounter = 0;	 // This will reset the counter back to 0
					lookAtOnce = true; // This will make the look at once dummy true
					goingDown = false; // This will make the going down boolean false
				}
			}
			if (npcGoingDown == true) // This checks to see if the going down boolean is true
			{
				npcCar.npcCar->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the hover car forward in the cut scence
				npcCar.npcCar->LookAt(waypoints[currentWayPoint].wayPoints); // This will make the hover car look at the dummy
				if (wayPointDection(npcCar.npcCar, waypoints[currentWayPoint].wayPoints)) // This is a function to test collision between the hover car and dummy
				{
					npcGoingDown = false; // This will make the going down boolean false
				}
			}
			if (lookAtOnce == true) // This will check to see if the look at once boolean is true
			{
				rotateLeftCounter = 0;   // This will reset the counter back to 0
				rotateRightCounter = 0;	 // This will reset the counter back to 0
				rotateUpCounter = 0;	 // This will reset the counter back to 0
				rotateDownCounter = 0;	 // This will reset the counter back to 0
				carDummy->LookAt(cutScenceDummy[cutScenceDummy3].cutScenceDummy); // This will make the car look at the dummy
				carDummy->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the car forwards 
			}
			if (goingAcross == true) // This will check to see if the goingAcross boolean is true
			{
				rotateLeftCounter = 0;   // This will reset the counter back to 0
				rotateRightCounter = 0;	 // This will reset the counter back to 0
				rotateUpCounter = 0;	 // This will reset the counter back to 0
				rotateDownCounter = 0;	 // This will reset the counter back to 0
				carDummy->LookAt(cutScenceDummy[cutScenceDummy1].cutScenceDummy); // This will make the hover car look at the dummy
				carDummy->MoveLocalZ(cutscenceSpeed * frameTime); // This will move the car forward
				barrier[barrierRLOne].barrier->SetLocalY(barrierLower); // This will lower the barriers so that car can get through
				barrier[barrierRLTwo].barrier->SetLocalY(barrierLower); // This will lower the barriers so that car can get through
				playerCar.hoverCar->ResetOrientation();
			}

			if ((bouncingCounter < bouncingMax) && (hovering == true)) // This will check to see if the bounce counter is less then the max and if hovering equals true
			{
				playerCar.hoverCar->Move(0, liftSpeed, 0); // This will move the car up
				npcCar.npcCar->Move(0, liftSpeed, 0); // This will move the NPC car up
				bouncingCounter += liftSpeed; // This will increase the counter
			}
			else if ((bouncingCounter >= bouncingMax) && (hovering == true)) // This will check to see if the counter is greater or equal to the max and that hovering is equal to true
			{
				hovering = false; // This will set hovering to false
			}

			if ((bouncingCounter > floorHeight) && (hovering == false)) // This will check to see if the bounce counter is greater then the floor and hover is equal to false
			{
				playerCar.hoverCar->Move(0, -liftSpeed, 0); // This will make the car move down
				npcCar.npcCar->Move(0, -liftSpeed, 0); // This will make the NPC car move down
				bouncingCounter -= liftSpeed; // This will decrease the bounce counter
			}
			else if ((bouncingCounter <= -floorHeight) && (hovering == false)) // This will check to see if the bounce counter is less or equal to the min and hovering is equal to false
			{
				hovering = true; // This will set hovering to true
			}
		}
		if (pause == true) // This will check to see if the game has finished
		{
			uiFont->Draw("Press R To Restart the Game", restartTextX, restartTextY, kWhite); // Displays text
			uiFont->Draw("THE WINNER IS...", winnerTextX, winnerTextY, kWhite); // Displays text

			if (leaderBoard == playerFirst) // if the race finished with playerFirst then the winner will be the player
			{
				uiFont->Draw("PLAYER", winnerNameX, winnerNameY, kWhite); // Displays text
			}
			if (leaderBoard == playerSecond) // if the race finished with playerSecond then the winner will be the NPC
			{
				uiFont->Draw("NPC", winnerNameX, winnerNameY, kWhite); // Displays text
			}
			
			if (myEngine->KeyHit(resetKey)) // When this key is hit it will reset all the counters, enums and positions so that the game can reset
			{
				/*** Variables being reset ***/
				StartStates = StandBy;
				wayPointStates = wayPointZero;
				cameraState = thirdPerson;
				checkpointStates = CheckpointZero;
				laps = lapZero;
				speedGauage->SetPosition(speedGuageXPos, speedGuageYPos);
				npcCheckpointCounter = 0;
				npcCheckpointCounterAmount = 0;
				currentWayPoint = 0;
				playerCheckpointCounter = 0;
				speedText = 0;
				boostText = 0;
				counterText = 0;
				HPText = 0;
				currentHP = maxHP;
				cameraUpCounter = 0.0f;
				cameraMinCounter = cameraminNumber;
				cameraSideCounter = 0.0f;
				boostText = 0.0f;
				currentThrust = 0.0f;
				countDown = countDownNumber;
				boostCountDown = boostCounterNumber;
				rotateDownCounter = 0.0f;
				rotateUpCounter = 0.0f;
				rotateLeftCounter = 0.0f;
				rotateRightCounter = 0.0f;
				rotateLimitCounter = 0.0f;
				speedGauageCounter = 0.0f;
				npcCurrentThrust = 0;
				carDummy->SetPosition(hoverCarStartingX, floorHeight, hovercarStartingZ);
				npcCar.npcCar->SetPosition(npcCarXPos, floorHeight, npcCarZPos);
				npcCar.npcCar->ResetOrientation();
				playerCar.hoverCar->ResetOrientation();
				carDummy->ResetOrientation();
				raceFinished = false;
				pause = false;
			}
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
/**** This is a function to test collisions for the cut scence dummys, this is a point to box collision ****/
bool pointToBoxCSD(IModel* cutScenceDummy, IModel* hovercar)
{
	float dectionModifyer = 1.0f;

	/**** These lines of code will check to see if the hover car is within the collision box and if so it will return true, if not it will return false ****/
	if (hovercar->GetX() < cutScenceDummy->GetX() + dectionModifyer && hovercar->GetX() > cutScenceDummy->GetX() - dectionModifyer
		&& hovercar->GetZ() < cutScenceDummy->GetZ() + dectionModifyer && hovercar->GetZ() > cutScenceDummy->GetZ() - dectionModifyer)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool pointToBox(IModel* checkpoint, IModel* hovercar) // This is a point to box collision box for the hover can and the checkpoints which is used for switching checkpoints
{
	float dectionModifyer = 9.0f;

	/**** This will check to see if the hovercar is within the collision box of the set parameters if so it will return true, it not it will return false ****/
	if (hovercar->GetX() < checkpoint->GetX() + dectionModifyer && hovercar->GetX() > checkpoint->GetX() - dectionModifyer
		&& hovercar->GetZ() < checkpoint->GetZ() + dectionModifyer && hovercar->GetZ() > checkpoint->GetZ() - dectionModifyer)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool pointToBoxNPC(IModel* checkpoint, IModel* npcCar) // This is a point to box collision box for the hover can and the checkpoints which is used for switching checkpoints
{
	float dectionModifyer = 9.0f;

	/**** This will check to see if the hovercar is within the collision box of the set parameters if so it will return true, it not it will return false ****/
	if (npcCar->GetX() < checkpoint->GetX() + dectionModifyer && npcCar->GetX() > checkpoint->GetX() - dectionModifyer
		&& npcCar->GetZ() < checkpoint->GetZ() + dectionModifyer && npcCar->GetZ() > checkpoint->GetZ() - dectionModifyer)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool wayPointDection(IModel* wayPointDummy, IModel* npcCar) // This is a function to test collision between the NPC car and the dummys for guiding the car on the track
{
	float dectionModifyer = 10.0f;

	/**** This will check to see if the NPC car is within the collision box of the waypoints given parameters if so it will return true, if not it will return false ****/
	if (npcCar->GetX() < wayPointDummy->GetX() + dectionModifyer && npcCar->GetX() > wayPointDummy->GetX() - dectionModifyer
		&& npcCar->GetZ() < wayPointDummy->GetZ() + dectionModifyer && npcCar->GetZ() > wayPointDummy->GetZ() - dectionModifyer)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool sphereToSphereWT(IModel* smallWaterTank, IModel* hovercar) // This is a function to test collision between the small water tanks and the hover car
{
	float collisionRadius = 4;

	sVector vecBetween; // This will be used for the get distance between the two vectors
	float distance; // This will be the outcome of the two distances and this will be used to check for the collision box of the water tanks

	vecBetween.X = (smallWaterTank->GetX()) - hovercar->GetX();                                              //Vector between two models on X-axis.
	vecBetween.Z = smallWaterTank->GetZ() - hovercar->GetZ();                                                    //Vector between two models on Z-axis.

	distance = sqrt((vecBetween.X * vecBetween.X) + (vecBetween.Z * vecBetween.Z));                            //Length between the two.

	if (distance < collisionRadius) // This checks to see if the distance is less then the amount if so it will return true, if not it will return false
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**** This is a function to test collision between the hover car and the isles, this is a sphere to box collision ****/
bool sphereToBoxIsles(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, IModel* isles)
{
	float dectionModifyerX = 2.0f;
	float dectionModifyerZ = 4.0f;

	/**** These are min and maxs for the isles and this effectivly makes the collision box ****/
	float islesMinX = isles->GetX() - dectionModifyerX - hoverCarRadius;
	float islesMaxX = isles->GetX() + dectionModifyerX + hoverCarRadius;
	float islesMinZ = isles->GetZ() - dectionModifyerZ - hoverCarRadius;
	float islesMaxZ = isles->GetZ() + dectionModifyerZ + hoverCarRadius;

	/**** This will check to see if the hovercars current X and Z are within the collision box if so it will return true, if not it will return false ****/
	if (hoverCarCurrentPosX > islesMinX && hoverCarCurrentPosX < islesMaxX
		&& hoverCarCurrentPosZ > islesMinZ && hoverCarCurrentPosZ < islesMaxZ)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool sphereToSphere(IModel* checkpoint, IModel* hovercar) // This is function to test sphere to sphere collision between the checkpoints legs and the hovercar
{
	sVector vecBetween; // This will be used for the get distance between the two vectors
	float distance; // This will be the outcome of the two distances and this will be used to check for the collision box of the checkpoint legs
	float dectionModifyer = 12.0f;
	float collisionRadius = 0.3f + 3;

	vecBetween.X = (checkpoint->GetX() - dectionModifyer) - hovercar->GetX();                                //Vector between two models on X-axis.
	vecBetween.Z = checkpoint->GetZ() - hovercar->GetZ();                                                    //Vector between two models on Z-axis.

	distance = sqrt((vecBetween.X * vecBetween.X) + (vecBetween.Z * vecBetween.Z));                          //Length between the two.

	// This will check to see if the hover car is within the collision box if so it will return true
	if (distance < collisionRadius)
	{
		return true;
	}


	vecBetween.X = (checkpoint->GetX() + dectionModifyer) - hovercar->GetX();                                            // Vector between the two models on X-axis
	vecBetween.Z = checkpoint->GetZ() - hovercar->GetZ();													// Vector between the two models on Z-axis 

	distance = sqrt((vecBetween.X * vecBetween.X) + (vecBetween.Z * vecBetween.Z));							// Length between the two

	// This will check to see if the hover car is within the collision box if so it will return true
	if (distance < collisionRadius)
	{
		return true;
	}


	/*** If nither of the above are true it will return false ***/
	return false;

}

/**** This is a function to test sphere to box collision between the hover car and the walls ****/
bool sphereToBox(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, IModel* wall)
{
	float dectionModifyerX = 1.0f;
	float dectionModifyerZ = 2.0f;


	/**** These are the min and maxs of the walls and are effectivly the walls collision box ****/
	float wallMinX = wall->GetX() - dectionModifyerX - hoverCarRadius;
	float wallMaxX = wall->GetX() + dectionModifyerX + hoverCarRadius;
	float wallMinZ = wall->GetZ() - dectionModifyerZ - hoverCarRadius;
	float wallMaxZ = wall->GetZ() + dectionModifyerZ + hoverCarRadius;

	/**** This checks to see if the hover car is with the collision box of the wall if so it will return true, if not it will return true ****/
	if (hoverCarCurrentPosX > wallMinX && hoverCarCurrentPosX < wallMaxX
		&& hoverCarCurrentPosZ > wallMinZ && hoverCarCurrentPosZ < wallMaxZ)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/**** This is a function to test sphere to box collision between the hover car and the barriers ****/
bool sphereToBoxBarrier(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, float hoverCarCurrentPosY,
	IModel* barrier, float dectionModifyerX, float dectionModifyerZ, float dectionModifyerY)
{


	/**** These are the barriers min and max's, these are effectivly the barriers collision box ****/
	float barrierMinX = barrier->GetX() - dectionModifyerX - hoverCarRadius;
	float barrierMaxX = barrier->GetX() + dectionModifyerX + hoverCarRadius;
	float barrierMinZ = barrier->GetZ() - dectionModifyerZ - hoverCarRadius;
	float barrierMaxZ = barrier->GetZ() + dectionModifyerZ + hoverCarRadius;
	float barrierMinY = barrier->GetY() - dectionModifyerY - hoverCarRadius;
	float barrierMaxY = barrier->GetY() + dectionModifyerY + hoverCarRadius;

	/**** This checks to see if the hovercar is within the barriers collision box if so it will return true, if not it will return false ****/
	if (hoverCarCurrentPosX > barrierMinX && hoverCarCurrentPosX < barrierMaxX
		&& hoverCarCurrentPosZ > barrierMinZ && hoverCarCurrentPosZ < barrierMaxZ
		&& hoverCarCurrentPosY > barrierMinY && hoverCarCurrentPosY < barrierMaxY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**** This is a function to test sphere to box collision between the hover car and the barriers ****/
bool sphereToBoxBarrierR(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, float hoverCarCurrentPosY,
	IModel* barrier, float dectionModifyerRX, float dectionModifyerRZ, float dectionModifyerRY)
{


	/**** These are the barriers min and max's, these are effectivly the barriers collision box ****/
	float barrierMinX = barrier->GetX() - dectionModifyerRX - hoverCarRadius;
	float barrierMaxX = barrier->GetX() + dectionModifyerRX + hoverCarRadius;
	float barrierMinZ = barrier->GetZ() - dectionModifyerRZ - hoverCarRadius;
	float barrierMaxZ = barrier->GetZ() + dectionModifyerRZ + hoverCarRadius;
	float barrierMinY = barrier->GetY() - dectionModifyerRY - hoverCarRadius;
	float barrierMaxY = barrier->GetY() + dectionModifyerRY + hoverCarRadius;

	/**** This checks to see if the hovercar is within the barriers collision box if so it will return true, if not it will return false ****/
	if (hoverCarCurrentPosX > barrierMinX && hoverCarCurrentPosX < barrierMaxX
		&& hoverCarCurrentPosZ > barrierMinZ && hoverCarCurrentPosZ < barrierMaxZ
		&& hoverCarCurrentPosY > barrierMinY && hoverCarCurrentPosY < barrierMaxY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**** This is a function to test sphere to box collision between the hover car and the squares ****/
bool sphereToBoxSquare(float hoverCarRadius, float hoverCarCurrentPosX, float hoverCarCurrentPosZ, IModel* squareCollision)
{
	float dectionModifyerX = 15.0f;
	float dectionModifyerZ = 15.0f;


	/**** These are the barriers min and max's, these are effectivly the squares collision box ****/
	float squareCollisionMinX = squareCollision->GetX() - dectionModifyerX - hoverCarRadius;
	float squareCollisionMaxX = squareCollision->GetX() + dectionModifyerX + hoverCarRadius;
	float squareCollisionMinZ = squareCollision->GetZ() - dectionModifyerZ - hoverCarRadius;
	float squareCollisionMaxZ = squareCollision->GetZ() + dectionModifyerZ + hoverCarRadius;

	/**** This checks to see if the hovercar is within the barriers collision box if so it will return true, if not it will return false ****/
	if (hoverCarCurrentPosX > squareCollisionMinX && hoverCarCurrentPosX < squareCollisionMaxX
		&& hoverCarCurrentPosZ > squareCollisionMinZ && hoverCarCurrentPosZ < squareCollisionMaxZ)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool sphereToSphereNPCCar(IModel* npcCar, IModel* hovercar) // This is a function to test collision between the small water tanks and the hover car
{
	sVector vecBetween; // This will be used for the get distance between the two vectors
	float distance; // This will be the outcome of the two distances and this will be used to check for the collision box of the water tanks
	float collisionRadius = 2.0f;

	vecBetween.X = (npcCar->GetX()) - hovercar->GetX();                                              //Vector between two models on X-axis.
	vecBetween.Z = npcCar->GetZ() - hovercar->GetZ();                                                    //Vector between two models on Z-axis.

	distance = sqrt((vecBetween.X * vecBetween.X) + (vecBetween.Z * vecBetween.Z));                            //Length between the two.

	if (distance < collisionRadius) // This checks to see if the distance is less then the amount if so it will return true, if not it will return false
	{
		return true;
	}
	else
	{
		return false;
	}
}