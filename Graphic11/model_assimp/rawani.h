#pragma once


namespace graphic
{
	using namespace common;

	struct sKeyPos
	{
		float t; // frame ����
		Vector3 p;
	};


	struct sKeyScale
	{
		float t; // frame ����
		Vector3 s;
	};


	struct sKeyRot
	{
		float t; // frame ����
		Quaternion q;
	};


	struct sBoneAni
	{
		Str32 name; // Bone Name
		float start; // seconds unit
		float end; // seconds unit
		vector<sKeyPos> pos; // localTm Animation
		vector<sKeyRot> rot; // localTm Animation
		vector<sKeyScale> scale; // localTm Animation
	};


	struct sRawAni
	{
		Str64 name;
		float start; // seconds unit
		float end; // seconds unit
		vector<sBoneAni> boneAnies; // Bone Animation Array, index = bone index
	};


	// �𵨿��� �ϳ��� �ִϸ��̼� ������ �����ϴ� �ڷᱸ��.
	struct sRawAniGroup
	{
		vector<sRawAni> anies; // Animation Array
	};

}
