#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cassert"
#include<numbers>

class MapChipField;

class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:

	static inline const float kWalkSpeed = 0.05f;

	 //angle from-to 
	 static inline const float kInitialWalkMotionAngle = -15.0f;
	 static inline const float kFinalWalkMotionAngle = 30.0f;
	 //do one round
	 static inline const float kWalkMotionTime = 0.8f;

	 float walkTimer_ = 0.0f;

	Vector3 velocity_ = {};

	//
	MapChipField* mapChipField_ = nullptr;
	//

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;
};

