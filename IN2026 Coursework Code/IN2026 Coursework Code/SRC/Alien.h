#ifndef __ALIEN_H__
#define __ALIEN_H__

#include "GameObject.h"
#include "Spaceship.h"

class Alien : public GameObject
{
public:
	Alien(void);
	~Alien(void);
	//Methods to control alien ship
	virtual void Shoot(shared_ptr<GameObject> player);
	void SetBulletShape(shared_ptr<Shape> bullet_shape) { mBulletShape = bullet_shape; }

	void dodgeAlien(shared_ptr<GameObject> player);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
	void Thrust(shared_ptr<GameObject> player);

private:
	shared_ptr<Shape> mBulletShape;
};

#endif
