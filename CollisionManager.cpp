#include "CollisionManager.h"

void CollisionManager::CheckAllCollision() {
	std::list<Collider*>::iterator itrA = colliders_.begin();

	for (; itrA != colliders_.end(); ++itrA) {
		std::list<Collider*>::iterator itrB = itrA;
		++itrB;

		for (; itrB != colliders_.end(); ++itrB) {
			CheckCollisionCircle(*(itrA), *(itrB));
		}
	}
}

//�~�̓����蔻��
void CollisionManager::CheckCollisionCircle(Collider* colliderA, Collider* colliderB) {
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) ||
	    !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return;
	}

	Vector3 posA = colliderA->GetWorldPosition();
	Vector3 posB = colliderB->GetWorldPosition();

	float radA = colliderA->GetRadius();
	float radB = colliderB->GetRadius();

	Vector3 Distance = {
	    (posB.x - posA.x) * (posB.x - posA.x), (posB.y - posA.y) * (posB.y - posA.y),
	    (posB.z - posA.z) * (posB.z - posA.z)};

	if (Distance.x + Distance.y + Distance.z <= (radA + radB) * (radA + radB)) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}



void CollisionManager::CheckCollisionBox(Collider* colliderA, Collider* colliderB,Collider*collderC)
{
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) ||
		!(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return;
	}

	float 


}
