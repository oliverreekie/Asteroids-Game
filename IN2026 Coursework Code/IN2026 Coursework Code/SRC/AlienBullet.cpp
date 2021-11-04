#include "GameWorld.h"
#include "AlienBullet.h"
#include "BoundingSphere.h"

AlienBullet::AlienBullet()
	: GameObject("AlienBullet"), mTimeToLive(2000)
{
}

AlienBullet::AlienBullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl)
	: GameObject("AlienBullet", p, v, a, h, r), mTimeToLive(ttl)
{
}

AlienBullet::AlienBullet(const AlienBullet& b)
	: GameObject(b),
	mTimeToLive(b.mTimeToLive)
{
}

AlienBullet::~AlienBullet(void)
{
}

void AlienBullet::Update(int t)
{
	//Make bullet last for short time
	GameObject::Update(t);
	mTimeToLive = mTimeToLive - t;
	if (mTimeToLive < 0) { mTimeToLive = 0; }
	if (mTimeToLive == 0) {
		if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
	}

}
//Collision settings for AlienBullet
bool AlienBullet::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (o->GetType() == GameObjectType("Hourglass")) return false;
	if (o->GetType() == GameObjectType("Asteroid")) return false;
	if (o->GetType() == GameObjectType("Asteroid_Small")) return false;
	if (o->GetType() == GameObjectType("Alien")) return false;
	if (o->GetType() == GameObjectType("Bullet")) return false;
	if (o->GetType() == GameObjectType("BigAlien")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
//Remove AlienBullet on collision
void AlienBullet::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}
