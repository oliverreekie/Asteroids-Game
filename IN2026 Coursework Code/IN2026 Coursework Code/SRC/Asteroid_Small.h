#pragma once
#ifndef _ASTEROID_SMALL_H_
#define _ASTEROID_SMALL_H_
#include "GameObject.h"
class Asteroid_Small : public GameObject
{
public:
	Asteroid_Small(void);
	~Asteroid_Small(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};
#endif