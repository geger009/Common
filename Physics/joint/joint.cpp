
#include "stdafx.h"
#include "joint.h"

using namespace phys;
using namespace physx;


cJoint::cJoint()
	: m_id(common::GenerateId())
	, m_joint(nullptr)
	, m_type(eJointType::None)
	, m_actor0(nullptr)
	, m_actor1(nullptr)
	, m_referenceMode(false)
	, m_revoluteAxis(Vector3(1,0,0))
{
}

cJoint::~cJoint()
{
	Clear();
}


// reference mode, no remove actor, joint object
// to show joint information with ui
bool cJoint::CreateReferenceMode()
{
	m_referenceMode = true;
	return true;
}


// worldTfm0, worldTfm1 is not same PxFixedJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateFixed(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (worldTfm0.pos + worldTfm1.pos) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, Vector3::Zeroes, localFrame0, localFrame1);

	PxFixedJoint *joint = PxFixedJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	//joint->setProjectionLinearTolerance(0.1f);
	//joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	if (m_breakForce > 0.f)
		joint->setBreakForce(m_breakForce, m_breakForce);

	m_type = eJointType::Fixed;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_origPos = jointPos;
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	return true;
}


// worldTfm0, worldTfm1 is not same PxSphericalJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateSpherical(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (worldTfm0.pos + worldTfm1.pos) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, Vector3::Zeroes, localFrame0, localFrame1);

	PxSphericalJoint *joint = PxSphericalJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	//joint->setProjectionLinearTolerance(0.1f);
	//joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	if (m_breakForce > 0.f)
		joint->setBreakForce(m_breakForce, m_breakForce);

	m_type = eJointType::Spherical;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_origPos = jointPos;
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	return true;
}


// worldTfm0, worldTfm1 is not same PxRevoluteJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateRevolute(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
	, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1
	, const Vector3 &revoluteAxis)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (pivot0 + pivot1) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, revoluteAxis, localFrame0, localFrame1);

	PxRevoluteJoint *joint = PxRevoluteJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	//joint->setProjectionLinearTolerance(0.5f);
	//joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	if (m_breakForce > 0.f)
		joint->setBreakForce(m_breakForce, m_breakForce);

	m_type = eJointType::Revolute;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_revoluteAxis = revoluteAxis;
	m_origPos = jointPos;
	m_rotRevolute.SetRotationArc(Vector3(1,0,0), revoluteAxis);
	m_revoluteAxisLen = pivot1.Distance(pivot0);
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	m_pivots[0].dir = (pivot0 - worldTfm0.pos).Normal();
	m_pivots[0].len = (pivot0 - worldTfm0.pos).Length();
	m_pivots[1].dir = (pivot1 - worldTfm1.pos).Normal();
	m_pivots[1].len = (pivot1 - worldTfm1.pos).Length();
	return true;
}


// return pivot world transform
// actorIndex: 0=actor0, 1=actor1
Transform cJoint::GetPivotWorldTransform(const int actorIndex)
{
	if ((actorIndex != 0) && (actorIndex != 1))
	{
		assert(0);
		return Transform();
	}

	const Transform tfm = (actorIndex == 0) ? m_actorLocal0 : m_actorLocal1;

	Transform ret;
	ret.pos = GetPivotPos(actorIndex);
	ret.scale = tfm.scale;
	ret.rot = tfm.rot;
	return ret;
}


// return pivot pos
Vector3 cJoint::GetPivotPos(const int actorIndex)
{
	if ((actorIndex != 0) && (actorIndex != 1))
		return Vector3::Zeroes;

	Vector3 pivotPos;
	const Transform tfm = (actorIndex == 0) ? m_actorLocal0 : m_actorLocal1;

	if (m_pivots[actorIndex].len != 0.f)
	{
		const Vector3 localPos = m_pivots[actorIndex].dir * tfm.rot
			* m_pivots[actorIndex].len;
		pivotPos = tfm.pos + localPos;
	}
	else
	{
		pivotPos = tfm.pos;
	}
	return pivotPos;
}


// actorIndex: 0=actor0, 1=actor1
// pos : pivot global pos
void cJoint::SetPivotPos(const int actorIndex, const Vector3 &pos)
{
	if ((actorIndex != 0) && (actorIndex != 1))
	{
		assert(0);
		return;
	}

	// change local coordinate system
	const Transform tfm = (actorIndex == 0) ? m_actorLocal0 : m_actorLocal1;
	const Vector3 dir = (pos - tfm.pos);
	m_pivots[actorIndex].dir = dir.Normal() * tfm.rot.Inverse();
	m_pivots[actorIndex].len = dir.Length();
}





// set spherical joint limit configuration
bool cJoint::SetLimitCone(const physx::PxJointLimitCone &config)
{
	if (PxSphericalJoint *p = m_joint->is<PxSphericalJoint>())
		p->setLimitCone(config);
	return true;
}


// Set SphericalJoint Flag
bool cJoint::SetSphericalJointFlag(physx::PxSphericalJointFlag::Enum flag, bool value)
{
	if (PxSphericalJoint *p = m_joint->is<PxSphericalJoint>())
		p->setSphericalJointFlag(flag, value);
	return true;
}


// set revolute joint limit configuration
bool cJoint::SetLimitAngular(const physx::PxJointAngularLimitPair &config)
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		p->setLimit(config);
	return true;
}


// set revolute joint flag
bool cJoint::SetRevoluteJointFlag(physx::PxRevoluteJointFlag::Enum flag, bool value)
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		p->setRevoluteJointFlag(flag, value);
	return true;
}


// set revolute joint drive velocity
bool cJoint::SetDriveVelocity(const float velocity)
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		p->setDriveVelocity(velocity);
	return true;
}


// return drive velocity
float cJoint::GetDriveVelocity()
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		return p->getDriveVelocity();
	return 0.f;
}


// set drive configuration
bool cJoint::SetDrive(const bool enable)
{
	SetRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, enable);
	return true;
}


// is drive?
bool cJoint::IsDrive()
{
	RETV(!m_joint, false);

	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
	{
		const PxRevoluteJointFlags flags = p->getRevoluteJointFlags();
		const bool isDrive = flags.isSet(PxRevoluteJointFlag::eDRIVE_ENABLED);
		return isDrive;
	}
	return false;
}


// set limit configuration
bool cJoint::SetLimit(const bool enable)
{
	SetRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, enable);
	return true;
}


// is limit?
bool cJoint::IsLimit()
{
	RETV(!m_joint, false);

	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
	{
		const PxRevoluteJointFlags flags = p->getRevoluteJointFlags();
		const bool isLimit = flags.isSet(PxRevoluteJointFlag::eLIMIT_ENABLED);
		return isLimit;
	}
	return false;
}


// modify pivot position
bool cJoint::ModifyPivot(cPhysicsEngine &physics
	, const Transform &worldTfm0, const Vector3 &pivot0
	, const Transform &worldTfm1, const Vector3 &pivot1
	, const Vector3 &revoluteAxis)
{
	if (m_actor0)
		m_actor0->RemoveJoint(this);
	if (m_actor1)
		m_actor1->RemoveJoint(this);
	PHY_SAFE_RELEASE(m_joint);

	CreateRevolute(physics, m_actor0, worldTfm0, pivot0
		, m_actor1, worldTfm1, pivot1
		, revoluteAxis);

	//RETV(!m_actor0 || !m_actor1 || !m_joint, false);
	//PxTransform localFrame0, localFrame1;
	//GetLocalFrame(worldTfm0, worldTfm1, revoluteAxis, localFrame0, localFrame1);
	//m_revoluteAxis = revoluteAxis;
	//m_joint->setLocalPose(PxJointActorIndex::eACTOR0, localFrame0);
	//m_joint->setLocalPose(PxJointActorIndex::eACTOR1, localFrame1);

	return true;
}


Vector3 cJoint::CalcJointPos(const Transform &worldTm0, const Transform &worldTm1)
{
	const Vector3 center = (worldTm0.pos + worldTm1.pos) / 2.f;
	return center;
}


// calc localFrame for PxJoint~ Function seriese
// worldTm0 : actor0 world transform
// worldTm1 : actor1 world transform
// revoluteAxis : revolution Axis
//			      if ZeroVector, ignore revolute axis
// out0 : return actor0 localFrame
// out1 : return actor1 localFrame
void cJoint::GetLocalFrame(const Transform &worldTm0, const Transform &worldTm1
	, const Vector3 &jointPos, const Vector3 &revoluteAxis
	, OUT physx::PxTransform &out0, OUT physx::PxTransform &out1)
{
	Transform tfm0 = worldTm0;
	Transform tfm1 = worldTm1;

	Vector3 p0 = jointPos - tfm0.pos;
	Vector3 p1 = jointPos - tfm1.pos;
	Quaternion q0 = tfm0.rot.Inverse();
	Quaternion q1 = tfm1.rot.Inverse();

	if (revoluteAxis != Vector3::Zeroes)
	{
		Quaternion rot(revoluteAxis, Vector3(1, 0, 0));
		tfm0.rot *= rot;
		tfm1.rot *= rot;

		p0 = (tfm0.pos - jointPos) * rot + jointPos;
		p1 = (tfm1.pos - jointPos) * rot + jointPos;
		p0 = jointPos - p0;
		p1 = jointPos - p1;
		q0 = tfm0.rot.Inverse();
		q1 = tfm1.rot.Inverse();
	}

	const PxTransform localFrame0 = PxTransform(*(PxQuat*)&q0) * PxTransform(*(PxVec3*)&p0);
	const PxTransform localFrame1 = PxTransform(*(PxQuat*)&q1) * PxTransform(*(PxVec3*)&p1);

	out0 = localFrame0;
	out1 = localFrame1;
}


void cJoint::Clear()
{
	if (!m_referenceMode)
	{
		PHY_SAFE_RELEASE(m_joint);
		m_actor0 = nullptr;
		m_actor1 = nullptr;
	}
}
