#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include "vector"
#include "Sprite.h"

class GameObject;
class Spaceship;
class GUILabel;
class Hourglass;
class Alien;
class BigAlien;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char* argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);
	//Recording state of boss level
	void setBossLevel(bool a)
	{
		bossLevel = a;
	}
	//Function to stop and allow the player to shoot
	void setCanShoot(bool a)
	{
		canShoot = a;
	}


private:
	//Shared pointers to game objects
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mTimerLabel;
	shared_ptr<Hourglass> mHourglass;
	shared_ptr<Alien> mAlien;
	shared_ptr<BigAlien> mBigAlien;
	shared_ptr<GUILabel> mBigAlienLabel;
	shared_ptr<GUILabel> mBossLevelLabel;

	//Vectors to store shared pointers, velocities and sprites for hourglass powerup
	std::vector<shared_ptr<GameObject>> storingAsteroid;
	std::vector<GLVector3f> storingVelocity;
	std::vector<shared_ptr<Sprite>> storingSprite;

	std::vector<shared_ptr<GameObject>> storingAsteroidSmall;
	std::vector<GLVector3f> storingVelocitySmall;
	std::vector<shared_ptr<Sprite>> storingSpriteSmall;

	//Recording the level
	uint mLevel;
	//Recording the number of asteroids
	uint mAsteroidCount;


	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	void CreateAsteroids_Small(const uint num_asteroids, shared_ptr<GameObject> object);
	shared_ptr<GameObject> CreateExplosion();
	shared_ptr<GameObject> CreateHourglass();
	
	//Defnitions of all timers
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint START_HOURGLASS_POWERUP = 3;
	const static uint END_HOURGLASS_POWERUP = 4;
	const static uint TIME_DECREASE = 5;
	const static uint ALIEN_SHOOTING = 6;
	const static uint ALIEN_MOVING = 7;
	const static uint START_BOSS_LEVEL = 8;
	const static uint ACTIVATE_BOSS_LEVEL = 9;
	const static uint SPAWN_ALIEN = 10;
	const static uint CHECK_LIVES = 11;

	//Scorekeeper
	ScoreKeeper mScoreKeeper;
	Player mPlayer;

	//How much time is left in powerup
	int time_left;

	//Getter, setter and decreaser for how much time is left in powerup
	int get_time_left() {
		return time_left;
	}
	int decrease_time_left() {
		return time_left--;
	}
	void set_time_left() {
		time_left = 5;
	}

	//Recording the level change
	bool levelChange = false;

	void setLevelChange(bool t) {
		levelChange = t;
	}
	//Functions to add gameobject information to vectors for hourglass powerup
	void AddToVector(shared_ptr<GameObject>);
	void AddToVectorSmall(shared_ptr<GameObject>);
	void AddToVectorSprite(shared_ptr<Sprite>);

	//Update alien health bar with current alien lives
	void UpdateAlienLives();

	//Record if hourglass is present
	bool hourglassPresent;

	//Getter and setter for hourglass presence
	void setHourglassPresent(bool a) {
		hourglassPresent = a;
	}
	
	bool getHourglassPresent() {
		return hourglassPresent;
	}

	//Create an alien
	shared_ptr<GameObject> CreateAlien();
	//Create a big alien
	shared_ptr<GameObject> CreateBigAlien();

	//Record level type
	bool bossLevel;
	//Record if player can shoot
	bool canShoot = true;

};

#endif