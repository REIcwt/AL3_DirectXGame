#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cassert"
#include<numbers>
#include "DeathParticles.h"


class MapChipField;
class Enemy;

class Player {
public:
	Player();
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="textureHandle"></param>
	/// <param name="viewProjection"></param>
	void Initialize(Model* model,ViewProjection* viewProjection,const Vector3& position);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void playerMove();
	void moveInput();

	 const WorldTransform& GetWorldTransform() const;

	 const Vector3& GetVelocity() const { return velocity_; }

	 void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	 const Vector3 GetWorldPosition();
	 const AABB GetAABB();
	 //
	 void OnCollision(const Enemy* enemy);

	bool IsDead() const { return isDead_; }

	bool cameraStopY = false;
	bool cameraStopX = false;

	bool isVisible_ = true;

  private:
	// 3D
	Model* model_ = nullptr;
	// ViewProjection
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;

	//Speed
	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.08f;
	static inline const float kAttenuation = 0.16f;
	static inline const float kLimitRunSpeed = 0.5f;
	//

	//Jump
	bool onGround_ = true;
	//Gravity(down)
	static inline const float kGravityAcceleration = 0.1f;
	//Max down speed
	static inline const float kLimitFallSpeed = 1.5f;
	//jump initialize speed
	static inline const float kJumpAcceleration = 1.2f;

	static inline const float kAttenuationLanding = 0.2f;
	static inline const float kAttenuationWall = 0.2f;
	static inline const float kCollisionEpsilon = 0.1f;

	//direction
	enum class LRDirection {
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotation_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.2f;


	//
	MapChipField* mapChipField_ = nullptr;
	static inline const float kWidth = 1.9f;
	static inline const float kHeight = 1.9f;

	struct CollisionMapInfo {
		bool isCeilingCollision = false;
		bool isLanding = false;
		bool hitWall = false;
		Vector3 move;
	};



	void CheckCollision(CollisionMapInfo& info);

	void CheckCollisionTop(CollisionMapInfo& info);
	void CheckCollisionBottom(CollisionMapInfo& info);
	void CheckCollisionLeft(CollisionMapInfo& info);
	void CheckCollisionRight(CollisionMapInfo& info);

	void SwitchGroundState(const CollisionMapInfo& info);


	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner
	};

	Vector3 CornerPosition(const Vector3& center, Corner corner) const;

	//
	static inline const float kBlank = 1.0f;
	void CollisionResult(const CollisionMapInfo& info);
	void CeilingCollision(const CollisionMapInfo& info);
	void WallCollision(const CollisionMapInfo& info);

	///
	DeathParticles* deathParticles_ = nullptr;
	bool isDead_ = false;
	///

};