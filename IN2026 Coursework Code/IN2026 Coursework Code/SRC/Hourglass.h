
#ifndef _HOURGLASS_H_
#define _HOURGLASS_H

#include "GameObject.h"
class Hourglass : public GameObject
{
public:
	Hourglass(void);
	~Hourglass(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};



#endif