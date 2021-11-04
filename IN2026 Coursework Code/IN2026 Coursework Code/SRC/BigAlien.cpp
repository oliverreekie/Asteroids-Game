#include <stdlib.h>
#include "GameUtil.h"
#include "BigAlien.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "Asteroids.h"
#include "Gameworld.h"

BigAlien::BigAlien(void) : GameObject("BigAlien")
{
	//Inititalise variables for big alien
	mPosition.x = 0.0;
	mPosition.y = 60.0;
	mPosition.z = 0.0;
}

BigAlien::~BigAlien(void)
{
}

//Collsion settings for big alien
bool BigAlien::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (o->GetType() == GameObjectType("AlienBullet")) return false;
	if (o->GetType() == GameObjectType("Alien")) return false;
	if (o->GetType() == GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void BigAlien::OnCollision(const GameObjectList& objects)
{
	//If alien has no lives remove from game
	if (alienLives <= 0) {
		mWorld->FlagForRemoval(GetThisPtr());
	}
	//Reduce alien lives
	else {
		reduceAlienLives();
	}

}