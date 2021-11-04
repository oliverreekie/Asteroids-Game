#include <stdlib.h>
#include "GameUtil.h"
#include "Hourglass.h"
#include "BoundingShape.h"
Hourglass::Hourglass(void) : GameObject("Hourglass")
{
	//Initialise variables for hourglass
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
}
Hourglass::~Hourglass(void)
{
}

//Collsion settings for hourglass
bool Hourglass::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (o->GetType() == GameObjectType("Asteroid")) return false;
	if (o->GetType() == GameObjectType("Asteroid_Small")) return false;
	if (o->GetType() == GameObjectType("Bullet")) return false;
	if (o->GetType() == GameObjectType("Alien")) return false;
	if (o->GetType() == GameObjectType("AlienBullet")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
void Hourglass::OnCollision(const GameObjectList& objects)
{
	//Remove from gameworld
	mWorld->FlagForRemoval(GetThisPtr());
}