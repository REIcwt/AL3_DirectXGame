#pragma once
#define NOMINMAX
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <algorithm>

class Player;

class CameraController {

public:
	void Initialize(ViewProjection* viewProjection);
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void SetMovableArea(Rect area) { movableArea_ = area; };

private:

	// ViewProjection
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	// Player
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0.0f, 2.0f, -18.0f};

	// cam move area
	Rect movableArea_ = {0, 154, 0, 154};

	// cameraDestination
	Vector3 cameraDestination;
	static inline const float kInterpolationRate = 0.1f;
	Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
	//
	static inline const float kVelocityBias = 0.1f;
	//
	static inline const Rect kCameraMargins = {-10.0f, 10.0f, -10.0f, 10.0f};

	Vector3 targetPosition;
};