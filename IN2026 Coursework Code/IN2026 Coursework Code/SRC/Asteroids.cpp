#include "Asteroid.h"
#include "Asteroids.h"
#include "Asteroid_Small.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "ScoreKeeper.h"
#include "Hourglass.h"
#include "vector"
#include "Alien.h"
#include "BigAlien.h"


// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	//Animations for game objects
	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	Animation *hourglass_anim = AnimationManager::GetInstance().CreateAnimationFromFile("hourglass", 900, 900, 900, 900, "hourglass_fs.png");
	Animation *alien_anim = AnimationManager::GetInstance().CreateAnimationFromFile("alien", 128, 8192, 128, 128, "enemy_fs.png");

	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	CreateAsteroids(10);
	//Create hourglass
	CreateHourglass();

	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
	setCanShoot(true);
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		//If level is not changing to boss level, player can create bullet
		if (canShoot == true) {
			mSpaceship->Shoot();
			//If in boss level, method to make the alien ship dodge
			if (bossLevel == true) {
				mAlien->dodgeAlien(mSpaceship);
			}
		}
		break;
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		//Create two smaller asteroids
		CreateAsteroids_Small(2, object);
	}
	else if (object->GetType() == GameObjectType("Asteroid_Small"))
	{
		//Create explosion
		shared_ptr<GameObject> explosion = CreateExplosion();
		//Set explosion position to object position
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		//If all asteroids are gone
		if (mAsteroidCount <= 0)
		{
			//Create boss level
			if (mLevel % 3 == 1) {
				SetTimer(500, ACTIVATE_BOSS_LEVEL);
			}
			//Create New Level
			else {
				SetTimer(500, START_NEXT_LEVEL);
			}
		}
	}
	else if (object->GetType() == GameObjectType("Hourglass"))
	{
		//Create timers to run hourglass powerup
		setHourglassPresent(false);
		SetTimer(0, START_HOURGLASS_POWERUP);
		SetTimer(5000, END_HOURGLASS_POWERUP);
	}
	else if (object->GetType() == GameObjectType("BigAlien"))
	{
		//Create explosion
		shared_ptr<GameObject> explosion = CreateExplosion();
		//Set explosion position to object position
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation()); 
		mGameWorld->AddObject(explosion);
		//Start new level
		SetTimer(2000, START_NEXT_LEVEL);
		//Remove alien health label
		mBigAlienLabel->SetVisible(false);
		
	}
	else if (object->GetType() == GameObjectType("Alien"))
	{
		//Create explosion
		shared_ptr<GameObject> explosion = CreateExplosion();
		//Set explosion position to object position
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		//Respawn alien
		if (mLevel % 3 == 2) {
			SetTimer(1000, SPAWN_ALIEN);
		}
	}

}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		//Respawn player spaceship in screen center
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		//Set hourglass timer label to invisible
		mTimerLabel->SetVisible(false);
		//Start booleans to change level
		setBossLevel(false);
		setLevelChange(true);
		//Increment Level
		mLevel++;
		//Spawn asteroids
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
		//Create hourglass powerup if none exist
		if (getHourglassPresent() == false) {
			CreateHourglass();
		}
		//Remove alien from gameworld
		mGameWorld->RemoveObject(mAlien);
	}

	if (value == ACTIVATE_BOSS_LEVEL)
	{
		//Increment level
		mLevel++;
		//Stop user from shooting to prevent bug
		setCanShoot(false);
		//Show label that states boss level is beginning
		mBossLevelLabel->SetVisible(true);
		//Start boss level
		SetTimer(2000, START_BOSS_LEVEL);
		//Remove hourglass if it exists
		if (getHourglassPresent() == true) {
			mGameWorld->RemoveObject(mHourglass);
		}
	}

	if (value == START_BOSS_LEVEL)
	{
		//Boss level state is true
		setBossLevel(true);
		//Hide boss level opening label
		mBossLevelLabel->SetVisible(false);
		//Create an alien
		SetTimer(0, SPAWN_ALIEN);
		//Create a big alien
		CreateBigAlien();	
		//Show alien health
		mBigAlienLabel->SetVisible(true);
		//Check the current lives of alien to adapt health label
		SetTimer(100, CHECK_LIVES);
		//Allow user to shoot
		setCanShoot(true);
		
	}

	if (value == CHECK_LIVES)
	{
		//Update the alien health label with current alien health
		UpdateAlienLives();
		//Call current timer again
		SetTimer(100, CHECK_LIVES);
	}

	if (value == SHOW_GAME_OVER)
	{
		//Show a game over label
		mGameOverLabel->SetVisible(true);
	}

	if (value == START_HOURGLASS_POWERUP)
	{	
		//Record level is changing
		setLevelChange(false);
		//If not a boss level, show hourglass timer
		if (mLevel % 3 != 2) {
			mTimerLabel->SetVisible(true);
		}
		//Set the timer counter
		set_time_left();
		//Store velocities and rotations of asteroids
		for (int i = 0; i < storingAsteroid.size(); i++)
		{
			storingAsteroid[i]->SetVelocity(GLVector3f(0, 0, 0));
			storingAsteroid[i]->SetRotation(0);
		}
		//Store velocities and rotations of small asteroids
		for (int i = 0; i < storingAsteroidSmall.size(); i++)
		{
			storingAsteroidSmall[i]->SetVelocity(GLVector3f(0, 0, 0));
		}
		//Pause animation for all asteroid sprites
		for (int i = 0; i < storingSprite.size(); i++) {
			storingSprite[i]->setPauseCheck(true);
		}
		//Pause animation for all small asteroid sprites
		for (int i = 0; i < storingSpriteSmall.size(); i++) {
			storingSpriteSmall[i]->setPauseCheck(true);
		}
		//Call timer to decrease time every 5 seconds
		SetTimer(000, TIME_DECREASE);
		SetTimer(1000, TIME_DECREASE);
		SetTimer(2000, TIME_DECREASE);
		SetTimer(3000, TIME_DECREASE);
		SetTimer(4000, TIME_DECREASE);
	}

	if (value == END_HOURGLASS_POWERUP)
	{
		//If level is not changing
		if (levelChange == false) {
			//Make hourglass timer invisible
			mTimerLabel->SetVisible(false);
			//Return velocities of each asteroid
			for (int i = 0; i < storingAsteroid.size(); i++)
			{
				storingAsteroid[i]->SetVelocity(storingVelocity[i]);
			}
			//Return velocities of each small asteroid
			for (int i = 0; i < storingAsteroidSmall.size(); i++)
			{
				storingAsteroidSmall[i]->SetVelocity(storingVelocitySmall[i]);
			}
			//Unpause animation for each asteroid sprite
			for (int i = 0; i < storingSprite.size(); i++) {
				storingSprite[i]->setPauseCheck(false);
			}
			//Unpause animation for each small asteroid sprite
			for (int i = 0; i < storingSpriteSmall.size(); i++) {
				storingSpriteSmall[i]->setPauseCheck(false);
			}
		}

	}

	if (value == TIME_DECREASE)
	{
		//Get current time remaining for powerup
		time_left = get_time_left();
		//Print message with new time remaining
		std::ostringstream msg_stream;
		msg_stream << "Time Remaining: " << time_left;
		std::string time_msg = msg_stream.str();
		mTimerLabel->SetText(time_msg);
		//Decrease time left
		decrease_time_left();
	}

	if (value == ALIEN_SHOOTING)
	{
		//Alien shoots at spaceship
		mAlien->Shoot(mSpaceship);
		//Call function to shoot again after 3 seconds
		SetTimer(3000, ALIEN_SHOOTING);
	}

	if (value == ALIEN_MOVING)
	{
		//Alien moves towards spaceship
		mAlien->Thrust(mSpaceship);
		//Call function to move alien after 0.1 seconds
		SetTimer(100, ALIEN_MOVING);
	}

	if (value == SPAWN_ALIEN)
	{
		//Create alien
		CreateAlien();
		//Begin recursive function to have alien shoot
		SetTimer(3000, ALIEN_SHOOTING);
		//Begin recursive function to have alien move
		SetTimer(100, ALIEN_MOVING);
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	//How many asteroids to create in level
	mAsteroidCount = num_asteroids * 2;
	for (uint i = 0; i < num_asteroids; i++)
	{
		//Set animation to object
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		//Create shared pointer to object
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		//Attach bounding sphere to object
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		//Set sprite to object
		asteroid->SetSprite(asteroid_sprite);
		//Set object scale
		asteroid->SetScale(0.2f);
		//Add object to gameworld
		mGameWorld->AddObject(asteroid);
		//Add shared pointers to vector
		AddToVector(asteroid);
		//Store velocity in vector
		storingVelocity.push_back(storingAsteroid[i]->GetVelocity());
		//Store sprite in vector
		storingSprite.push_back(asteroid_sprite);
	}
}

void Asteroids::CreateAsteroids_Small(const uint num_asteroids, shared_ptr<GameObject> object)
{
	for (uint i = 0; i < num_asteroids; i++)
	{
		//Set animation to object
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		//Create shared pointer to object
		shared_ptr<GameObject> asteroid_small = make_shared<Asteroid_Small>();
		//Attach bounding sphere to object
		asteroid_small->SetBoundingShape(make_shared<BoundingSphere>(asteroid_small->GetThisPtr(), 5.0f));
		//Set sprite to object
		asteroid_small->SetSprite(asteroid_sprite);
		//Set object scale
		asteroid_small->SetScale(0.1f);
		//Set position of object
		asteroid_small->SetPosition(object->GetPosition());
		//Add object to gameworld
		mGameWorld->AddObject(asteroid_small);
		//Add shared pointers to vector
		AddToVectorSmall(asteroid_small);
		//Store velocity in vector
		storingVelocitySmall.push_back(storingAsteroidSmall[i]->GetVelocity());
		//Store sprite in vector
		storingSpriteSmall.push_back(asteroid_sprite);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
	= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	//Timer for powerup
	mTimerLabel = shared_ptr<GUILabel>(new GUILabel("Time Remaining: 5"));
	//Set horizontal alignment
	mTimerLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	//Set vertical alignment
	mTimerLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	//Initially set to invisible
	mTimerLabel->SetVisible(false);
	//Create shared pointer for label 
	shared_ptr<GUIComponent> timer_component
		= static_pointer_cast<GUIComponent>(mTimerLabel);
	mGameDisplay->GetContainer()->AddComponent(timer_component, GLVector2f(1.0f, 1.0f));

	//Big Alien Health
	mBigAlienLabel = shared_ptr<GUILabel>(new GUILabel("Alien Health: 100"));
	//Set horizontal alignment
	mBigAlienLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	//Set vertical alignment
	mBigAlienLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	//Initially set to invisible
	mBigAlienLabel->SetVisible(false);
	//Create shared pointer for label 
	shared_ptr<GUIComponent> big_alien_component = static_pointer_cast<GUIComponent>(mBigAlienLabel);
	mGameDisplay->GetContainer()->AddComponent(big_alien_component, GLVector2f(1.0f, 0.0f));

	//Boss Level
	mBossLevelLabel = shared_ptr<GUILabel>(new GUILabel("Boss Level!"));
	//Set horizontal alignment
	mBossLevelLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	//Set vertical alignment
	mBossLevelLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	//Create shared pointer for label 
	mBossLevelLabel->SetVisible(false);
	//Create shared pointer for label 
	shared_ptr<GUIComponent> boss_level_component
		= static_pointer_cast<GUIComponent>(mBossLevelLabel);
	mGameDisplay->GetContainer()->AddComponent(boss_level_component, GLVector2f(0.5f, 0.5f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	//Create explosion
	shared_ptr<GameObject> explosion = CreateExplosion();
	//Set explosion position
	explosion->SetPosition(mSpaceship->GetPosition());
	//Set explosion rotation
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);
	//If player has lives remaing then respawn, otherwise end game
	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

void Asteroids::UpdateAlienLives()
{
	//Get current alien lives
	std::ostringstream msg_stream;
	msg_stream << "Alien Health: " << mBigAlien->getAlienLives();
	std::string alien_msg = msg_stream.str();
	//Set text to include new alien lives
	mBigAlienLabel->SetText(alien_msg);
}


shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	//Get animation from animation list
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	//Create shared pointer to object
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	//Set sprite for object
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

shared_ptr<GameObject> Asteroids::CreateHourglass()
{
	//Create instance of hourglass class
	mHourglass = make_shared<Hourglass>();
	//Set bounding sphere to hourglass
	mHourglass->SetBoundingShape(make_shared<BoundingSphere>(mHourglass->GetThisPtr(), 10.0f));
	//Attach animation from snimation list
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("hourglass");
	shared_ptr<Sprite> hourglass_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	//Set sprite for hourglass
	mHourglass->SetSprite(hourglass_sprite);
	//Set scale for hourglass
	mHourglass->SetScale(0.02f);
	//Add hourglass to world
	mGameWorld->AddObject(mHourglass);
	//Record creation of hourglass
	setHourglassPresent(true);
	return mHourglass;
}

void Asteroids::AddToVector(shared_ptr<GameObject> o)
{
	//Add shared pointers to asteroids in vector
	storingAsteroid.push_back(o);
}

void Asteroids::AddToVectorSmall(shared_ptr<GameObject> o)
{
	//Add shared pointers to small asteroids in vector
	storingAsteroidSmall.push_back(o);
}


shared_ptr<GameObject> Asteroids::CreateAlien()
{
	//Create object of type alien
	mAlien = make_shared<Alien>();
	//Set a bounding sphere to alien
	mAlien->SetBoundingShape(make_shared<BoundingSphere>(mAlien->GetThisPtr(), 8.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mAlien->SetBulletShape(bullet_shape);
	//Get animation from file list
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("alien");
	shared_ptr<Sprite> spaceship_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	//Set sprite for alien
	mAlien->SetSprite(spaceship_sprite);
	//Set scale of alien
	mAlien->SetScale(0.15f);
	//Add alien to world
	mGameWorld->AddObject(mAlien);
	//Set position to big alien
	mAlien->SetPosition(mBigAlien->GetPosition());
	return mAlien;
}

shared_ptr<GameObject> Asteroids::CreateBigAlien()
{
	//Create object of type alien
	mBigAlien = make_shared<BigAlien>();
	//Set bounding sphere to big alien
	mBigAlien->SetBoundingShape(make_shared<BoundingSphere>(mBigAlien->GetThisPtr(), 20.0f));
	//Get animation from file list
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("alien");
	shared_ptr<Sprite> spaceship_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	//Set sprite for big alien
	mBigAlien->SetSprite(spaceship_sprite);
	//Set scale of big alien
	mBigAlien->SetScale(0.5f);
	//Add big alien to world
	mGameWorld->AddObject(mBigAlien);
	//Set big alien lives to 100
	mBigAlien->setAlienLives();
	return mBigAlien;
}

