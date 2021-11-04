#ifndef __BIGALIEN_H__
#define __BIGALIEN_H__

#include "GameObject.h"
#include "Spaceship.h"

class BigAlien : public GameObject
{
public:
	BigAlien(void);
	~BigAlien(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

	//Getter, reducer and setter for alien lives
	int getAlienLives() {
		return alienLives;
	}

	void reduceAlienLives() {
		alienLives -= 5;
	}

	void setAlienLives() {
		alienLives = 100;
	}

private:
	//Current alien lives
	int alienLives;

};

#endif