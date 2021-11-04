#include <stdlib.h>
#include "GameUtil.h"
#include "Alien.h"
#include "BoundingShape.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "BoundingSphere.h"
#include "AlienBullet.h"

Alien::Alien(void) : GameObject("Alien")
{
	//Values to initialise the Alien
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD * mAngle);
}

Alien::~Alien(void)
{
}
//Method to shoot a bullet at player
void Alien::Shoot(shared_ptr<GameObject> player)
{
	if (!mWorld) return;
	float bullet_speed = 0.5;
	//Calculating the velocity as the difference between player and alien position
	GLVector3f player_position(player->GetPosition());
	GLVector3f alien_position(mPosition);
	GLVector3f calculate(player_position - alien_position);
	GLVector3f alienbullet_velocity = calculate * bullet_speed;
	//Creating an alienbullet with a velocity directing it at the player.
	shared_ptr<GameObject> alienbullet
	(new AlienBullet(mPosition, alienbullet_velocity, mAcceleration, mAngle, 0, 2000));
	alienbullet->SetBoundingShape(make_shared<BoundingSphere>(alienbullet->GetThisPtr(), 2.0f));
	alienbullet->SetShape(mBulletShape);
	mWorld->AddObject(alienbullet);
}
//Method to move the alien ship towards the player
void Alien::Thrust(shared_ptr<GameObject> player)
{
	GLVector3f player_position(player->GetPosition());
	//Finding the direction of the player from the alien
	GLVector3f direction;
	direction.x = player_position.x - mPosition.x;
	direction.y = player_position.y - mPosition.y;
	//Normalising the vector
	int hypotenuse = sqrt((direction.x * direction.x) + (direction.y * direction.y));

	direction.x = direction.x / hypotenuse;
	direction.y = direction.y / hypotenuse;
	//Increasing the velocity in the player direction
	mVelocity.x += direction.x * 0.7;
	mVelocity.y += direction.y * 0.7;
	
}
//Method to dodge a bullet
void Alien::dodgeAlien(shared_ptr<GameObject> player)
{
	GLVector3f player_position = player->GetPosition();
	//Working out the quadrant around the player than the alien is in
	int quadrant;
	if (mPosition.x >= player_position.x && mPosition.y <= player_position.y) {
		quadrant = 1;
	}
	else if (mPosition.x > player_position.x && mPosition.y > player_position.y) {
		quadrant = 2;
	}
	else if (mPosition.x < player_position.x && mPosition.y < player_position.y) {
		quadrant = 3;
	}
	else {
		quadrant = 4;
	}
	//Setting the velocity based on the alien's position around the player
	if (quadrant == 1) {
		mVelocity.x = 15;
		mVelocity.y = 5;
	}
	else if (quadrant == 2) {
		mVelocity.x = 5;
		mVelocity.y = -15;
	}
	else if (quadrant == 3) {
		mVelocity.x = -15;
		mVelocity.y = -5;
	}
	else {
		mVelocity.x = -5;
		mVelocity.y = 15;
	}


}
//Collision set up for the alien ship
bool Alien::CollisionTest(shared_ptr<GameObject> o)
{
	if (GetType() == o->GetType()) return false;
	if (o->GetType() == GameObjectType("Asteroid")) return false;
	if (o->GetType() == GameObjectType("Asteroid_Small")) return false;
	if (o->GetType() == GameObjectType("AlienBullet")) return false;
	if (o->GetType() == GameObjectType("BigAlien")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
//Remove alien on collision
void Alien::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
	
}


