
#include "stdafx.h"
#include "rigidactor.h"
#include "../manager/physicsengine.h"

using namespace phys;
using namespace physx;


cRigidActor::cRigidActor()
	: m_actor(nullptr)
	, m_dynamic(nullptr)
	, m_static(nullptr)
	, m_type(eType::None)
	, m_shape(eShape::None)
{
}

cRigidActor::~cRigidActor()
{
	Clear();
}


bool cRigidActor::CreatePlane(cPhysicsEngine &physics
	, const Vector3 &norm)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidStatic* plane = PxCreatePlane(*physics.m_physics
		, PxPlane(*(PxVec3*)&norm, 0), *physics.m_material);
	if (!plane)
		return nullptr;
	physics.m_scene->addActor(*plane);

	m_type = eType::Static;
	m_shape = eShape::Plane;
	m_actor = plane;
	m_static = plane;
	return true;
}


bool cRigidActor::CreateBox(cPhysicsEngine &physics
	, const Transform &tfm
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* box = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot)
		, PxBoxGeometry(*(PxVec3*)&tfm.scale), *physics.m_material, density);
	PX_ASSERT(box);

	box->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	box->setAngularDamping(0.5f);
	box->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physics.m_scene->addActor(*box);
	if (linVel)
		box->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Box;
	m_actor = box;
	m_dynamic = box;
	return true;
}


bool cRigidActor::CreateSphere(cPhysicsEngine &physics
	, const Vector3& pos
	, const float radius
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* sphere = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&pos)
		, PxSphereGeometry(radius), *physics.m_material, density);
	PX_ASSERT(sphere);

	sphere->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	sphere->setAngularDamping(0.5f);
	sphere->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physics.m_scene->addActor(*sphere);

	if (linVel)
		sphere->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Sphere;
	m_actor = sphere;
	m_dynamic = sphere;
	return true;
}


bool cRigidActor::CreateCapsule(cPhysicsEngine &physics
	, const Transform &tfm
	, const float radius
	, const float halfHeight
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	const PxQuat rot = PxQuat(PxIdentity);
	PX_UNUSED(rot);

	PxRigidDynamic* capsule = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot)
		, PxCapsuleGeometry(radius, halfHeight), *physics.m_material, density);
	PX_ASSERT(capsule);

	capsule->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	capsule->setAngularDamping(0.5f);
	capsule->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physics.m_scene->addActor(*capsule);

	if (linVel)
		capsule->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Capsule;
	m_actor = capsule;
	m_dynamic = capsule;
	return true;
}


// change geometry shape
bool cRigidActor::ChangeDimension(cPhysicsEngine &physics, const Vector3 &dim)
{
	RETV(!m_actor, false);

	// 1. remove old shape
	const uint count = m_actor->getNbShapes();
	if (count == 0)
		return false;

	PxShape **shapes = new PxShape*[count];
	const uint result = m_actor->getShapes(shapes, count);
	for (uint i = 0; i < result; ++i)
		m_actor->detachShape(*shapes[i]);
	delete[] shapes;

	// 2. generate new shape
	switch (m_shape)
	{
	case eShape::Box:
		m_actor->createShape(PxBoxGeometry(*(PxVec3*)&dim), *physics.m_material); 
		break;
	case eShape::Sphere:
		m_actor->createShape(PxSphereGeometry(dim.x), *physics.m_material);
		break;
	case eShape::Capsule:
		m_actor->createShape(PxCapsuleGeometry(dim.y, dim.x), *physics.m_material);
		break;

	case eShape::Plane: // not support
	case eShape::Convex: // not support
		assert(0);
		break;
	default: assert(0); break;
	}

	return true;
}


bool cRigidActor::SetKinematic(const bool isKinematic)
{
	RETV(!m_actor, false);
	RETV(m_type != eType::Dynamic, false);

	m_dynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	return true;
}


bool cRigidActor::IsKinematic() const
{
	RETV(!m_actor, false);
	RETV(m_type != eType::Dynamic, false);

	const PxRigidBodyFlags flags = m_dynamic->getRigidBodyFlags();
	const bool isKinematic = flags.isSet(PxRigidBodyFlag::eKINEMATIC);
	return isKinematic;
}


// add joint reference
// joint is this rigid actor connection joint
bool cRigidActor::AddJoint(cJoint *joint)
{
	if (m_joints.end() == std::find(m_joints.begin(), m_joints.end(), joint))
	{
		m_joints.push_back(joint);
		return true;
	}
	return false;
}


// remove joint reference
bool cRigidActor::RemoveJoint(cJoint *joint)
{
	if (m_joints.end() != std::find(m_joints.begin(), m_joints.end(), joint))
	{
		common::removevector(m_joints, joint);
		return true;
	}
	return false;
}


void cRigidActor::Clear()
{
	PHY_SAFE_RELEASE(m_actor);
	m_dynamic = nullptr;
	m_static = nullptr;
}
