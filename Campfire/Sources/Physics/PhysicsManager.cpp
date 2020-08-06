#include "Physics/PhysicsManager.h"

btDefaultCollisionConfiguration* PhysicsManager::collisionConfiguration;
btCollisionDispatcher* PhysicsManager::dispatcher;
btBroadphaseInterface* PhysicsManager::overlappingPairCache;
btSequentialImpulseConstraintSolver* PhysicsManager::solver;
btDiscreteDynamicsWorld* PhysicsManager::dynamicsWorld;
btAlignedObjectArray<btCollisionShape*> PhysicsManager::collisionShapes;
//BulletDebugDrawer PhysicsManager::mydebugdrawer;

float PhysicsManager::gravity = -9.81;

void PhysicsManager::Init()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, gravity, 0));

    //dynamicsWorld->setDebugDrawer(&mydebugdrawer);
}

// TODO for when updating rigidbody parameters in GUI
//void PhysicsManager::UpdateObject()
//{
//
//}

//void PhysicsManager::AddObject(GameObject* object)
//{
//    // TODO is this needed?
//    collisionShapes.push_back(object->rigidBody->shape);
//    dynamicsWorld->addRigidBody(object->rigidBody->body);
//}

//void PhysicsManager::UpdateColliders()
//{
//    // FIXME: reorganize somehow
//
//    // First clear lists of old data
//    ClearLists();
//
//    //for (auto objectPtr : shared.objectManager->objectList)
//    for (int i = 0; i < shared.objectManager->objectList.size(); ++i)
//    {
//        GameObject* objectPtr = shared.objectManager->objectList[i];
//        // FIXME: objects should be able to not have a rb
//        if (objectPtr->rigidBody)
//        {
//            // Reinitialize the transforms for all objects in list
//            objectPtr->rigidBody->InitTransform(objectPtr->position, objectPtr->rotation, objectPtr->scale);
//            // hacky way for now by storing index and later retrieving
//            // picked gameObject based on that index
//            objectPtr->rigidBody->body->setUserPointer((void*)i);
//            AddObject(objectPtr);
//        }
//    }
//}

//void PhysicsManager::RemoveObject(GameObject* object)
//{
//    dynamicsWorld->removeRigidBody(object->rigidBody->body);
//}

//GameObject* FindObjectFromShape(btCollisionObject* colObject)
//{
//    for (auto objectPtr : shared.objectManager->objectList)
//    {
//        if (objectPtr->rigidBody->shape->getUserPointer() == colObject)
//        {
//            std::cout << "Found matching gameObject\n";
//            return objectPtr;
//        }
//    }
//    return nullptr;
//}

// ==============================================================
// Primarily for picking objects in the scene with mouse click
// ==============================================================
//bool PhysicsManager::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, int& index)
//{
//    glm::vec3 rayEnd = rayOrigin + rayDir * 10000.0f;
//
//    btVector3 from(rayOrigin.x, rayOrigin.y, rayOrigin.z);
//    btVector3 to(rayEnd.x, rayEnd.y, rayEnd.z);
//
//    btCollisionWorld::ClosestRayResultCallback closestHit(
//        from,
//        to
//    );
//
//    dynamicsWorld->rayTest(
//        from,
//        to,
//        closestHit
//    );
//
//    if (closestHit.hasHit())
//    {
//        // FIXME
//        //index = static_cast<int>(closestHit.m_collisionObject->getUserPointer());
//        return true;
//    }
//
//    return false;
//}

void PhysicsManager::OnUpdate(float dt)
{
    dynamicsWorld->stepSimulation(dt, 10);

    //for (int i = dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i)
//    for (auto objectPtr : shared.objectManager->objectList)
//    {
//        //btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
//        //btRigidBody* body = btRigidBody::upcast(obj);
//
//        btRigidBody* body = objectPtr->rigidBody->body;
//        btTransform trans;
//        if (body && body->getMotionState())
//        {
//            body->getMotionState()->getWorldTransform(trans);
//        }
//        else
//        {
//            //trans = objectPtr->rigidBody->shape->getWorldTransform();
//            //std::cout << "Warning: Object missing motion state\n";
//        }
//
    //    // Update transform
    //    btScalar m[16];
    //    trans.getOpenGLMatrix(m);
    //    // Apply scale separately since bullet doesn't convey that info
    //    glm::mat4 model = glm::make_mat4x4(m);
    //    model = glm::scale(model, objectPtr->scale);
    //    objectPtr->model = model;
    //}
}

//void PhysicsManager::DebugDraw()
//{
//    mydebugdrawer.SetMatrices(
//        shared.renderCamera->GetViewMatrix(),
//        // TODO get resolution from camera
//        shared.renderCamera->GetProjMatrix(1600.0f, 900.0f)
//    );
//
//    dynamicsWorld->debugDrawWorld();
//    // TODO make all lines into one draw call
//    //mydebugdrawer.DebugDraw();
//}

void PhysicsManager::ClearLists()
{
    // remove the rigidbodies from the dynamics world and delete them
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    // Delete collision shapes
    for (int i = 0; i < collisionShapes.size(); ++i)
    {
        btCollisionShape* shape = collisionShapes[i];
        collisionShapes[i] = 0;
        delete shape;
    }
}

void PhysicsManager::Shutdown()
{
    ClearLists();
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    collisionShapes.clear();
}
