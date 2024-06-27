#pragma once
#include "MapChipField.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "cassert"
#include<numbers>

class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle);

	void Update();

	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };


private:

	static inline const float kWalkSpeed = 0.05f;

	Vector3 velocity_ = {};

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	ViewProjection* viewProjection_ = nullptr;
};

