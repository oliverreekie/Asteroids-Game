#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"

Asteroid::Asteroid(void) : GameObject("Asteroid")
{
	//To initialise Asteroid
	mAngle = rand() % 360;
	mRotation = 0;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;

}

Asteroid::~Asteroid(void)
{
}
//Collision settings for bullet 
bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (o->GetType() == GameObjectType("Asteroid_Small")) return false;
	if (o->GetType() == GameObjectType("Hourglass")) return false;
	if (o->GetType() == GameObjectType("Alien")) return false;
	if (o->GetType() == GameObjectType("AlienBullet")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
//Remove object on collision
void Asteroid::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

