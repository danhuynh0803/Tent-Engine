#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>

#include "Physics/BulletDebugDrawer.h"
#include "Scene/Entity.h"

class PhysicsManager
{
public:
    static void Init();
    static void OnUpdate(float dt);
    static void Shutdown();

    static void SubmitEntity(Entity& entity);
    //static void UpdateColliders();
    static void ClearLists();

    //static void DebugDraw();

    // TODO Move to general picking or maybe make my own
    //static bool Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, int& index);

public:
    //static bool isBoundingBoxOn = false;
    static float gravity;

private:
    // TODO make uniquePtrs for these
    static btDefaultCollisionConfiguration* collisionConfiguration;
    static btCollisionDispatcher* dispatcher;
    static btBroadphaseInterface* overlappingPairCache;
    static btSequentialImpulseConstraintSolver* solver;
    static btDiscreteDynamicsWorld* dynamicsWorld;
    static btAlignedObjectArray<btCollisionShape*> collisionShapes;
    //static BulletDebugDrawer mydebugdrawer;
};

#endif // PHYSICS_MANAGER_H
