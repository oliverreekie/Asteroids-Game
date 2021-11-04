#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid_Small.h"
#include "BoundingShape.h"

Asteroid_Small::Asteroid_Small(void) : GameObject("Asteroid_Small")
{
	//To initialise Asteroid_Small
	mAngle = rand() % 360;
	mRotation = 0;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

Asteroid_Small::~Asteroid_Small(void)
{
}
//Collision settings for object
bool Asteroid_Small::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (o->GetType() == GameObjectType("Asteroid")) return false;
	if (o->GetType() == GameObjectType("Hourglass")) return false;
	if (o->GetType() == GameObjectType("Alien")) return false;
	if (o->GetType() == GameObjectType("AlienBullet")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
//Remove on collision
void Asteroid_Small::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}