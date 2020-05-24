#include "PhysicsManager.h"
#include "Shared.h"
#include <iostream>


void PhysicsManager::Start()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();

    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    overlappingPairCache = new btDbvtBroadphase();

    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, gravity, 0));

    dynamicsWorld->setDebugDrawer(&mydebugdrawer);
}

// TODO for when updating rigidbody parameters in GUI
//void PhysicsManager::UpdateObject()
//{
//
//}

void PhysicsManager::AddObject(GameObject* object)
{
    // TODO is this needed?
    collisionShapes.push_back(object->rigidBody->shape);
    dynamicsWorld->addRigidBody(object->rigidBody->body);
}

void PhysicsManager::UpdateColliders()
{
    // FIXME: reorganize somehow

    // First clear lists of old data
    ClearLists();

    for (auto objectPtr : shared.objectManager->objectList)
    {
        // FIXME: objects should be able to not have a rb
        if (objectPtr->rigidBody)
        {
            // Reinitialize the transforms for all objects in list
            objectPtr->rigidBody->InitTransform(objectPtr->position, objectPtr->rotation, objectPtr->scale);
            AddObject(objectPtr);
        }
    }
}

//void PhysicsManager::RemoveObject(GameObject* object)
//{
//    dynamicsWorld->removeRigidBody(object->rigidBody->body);
//}

// ==============================================================
// Primarily for picking objects in the scene with mouse click
// ==============================================================
GameObject* PhysicsManager::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir)
{
    glm::vec3 rayEnd = rayOrigin + rayDir * 1000.0f;

    btVector3 from(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    btVector3 to(rayEnd.x, rayEnd.y, rayEnd.z);

    btCollisionWorld::ClosestRayResultCallback closestHit(
        from,
        to
    );

    dynamicsWorld->rayTest(
        from,
        to,
        closestHit
    );

    if (closestHit.hasHit())
    {
        //std::cout << closestHit.m_collisionObject << '\n';
        return nullptr; // TODO
    }
    else // Not hit with any gameobjects
    {
        //std::cout << "No object hit from raycast\n";
        return nullptr;
    }
}

void PhysicsManager::Update()
{
    if (false /*GameStatePaused*/) { return; }

    // Fixed update at 60fps
    dynamicsWorld->stepSimulation(1.0f/60.0f, 10);
    //for (int i = dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i)
    for (auto objectPtr : shared.objectManager->objectList)
    {
        //btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        //btRigidBody* body = btRigidBody::upcast(obj);

        btRigidBody* body = objectPtr->rigidBody->body;
        btTransform trans;
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            //trans = objectPtr->rigidBody->shape->getWorldTransform();
            std::cout << "Warning: Object missing motion state\n";
        }

        // Update transform
        btScalar m[16];
        trans.getOpenGLMatrix(m);
        // Apply scale separately since bullet doesn't convey that info
        glm::mat4 model = glm::make_mat4x4(m);
        model = glm::scale(model, objectPtr->scale);
        objectPtr->model = model;
    }
}

void PhysicsManager::DebugDraw()
{
    mydebugdrawer.SetMatrices(
        shared.renderCamera->GetViewMatrix(),
        // TODO get resolution from camera
        shared.renderCamera->GetProjMatrix(1600.0f, 900.0f)
    );

    dynamicsWorld->debugDrawWorld();
    // TODO make all lines into one draw call
    //mydebugdrawer.DebugDraw();
}

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
