#pragma once
#include "Player.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cal.h"

class Player;

struct Rect {
	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;
};

/// <summary>
/// CamraController
/// </summary>
class CameraController {
public:
	CameraController();
	~CameraController();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; }
	void Reset();

	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	// ViewProjection
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_;
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
	static inline const float kVelovityBias = 0.1f;
	//
	static inline const Rect kCameraMargins = {-10.0f, 10.0f, -10.0f, 10.0f};
};