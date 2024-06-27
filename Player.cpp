#define NOMINMAX
#include "Player.h"
#include "DebugCamera.h"
#include "Input.h"
#include "MapChipField.h"
#include <algorithm>
#include <array>
#include <numbers>

Player::Player(){};
Player::~Player(){};

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);
	model_ = model;

	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
}

#pragma region player move

void Player::playerMove() { moveInput(); }

void Player::moveInput() {
	Vector3 acceleration = {};

	// left-right
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		if (velocity_.x < 0.0f) {
			velocity_.x *= (1.0f - kAcceleration);
		}
		acceleration.x += kAcceleration;
		if (lrDirection_ != LRDirection::kRight) {
			turnFirstRotation_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
			lrDirection_ = LRDirection::kRight;
		}
	} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		if (velocity_.x > 0.0f) {
			velocity_.x *= (1.0f - kAcceleration);
		}
		acceleration.x -= kAcceleration;
		if (lrDirection_ != LRDirection::kLeft) {
			turnFirstRotation_ = worldTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
			lrDirection_ = LRDirection::kLeft;
		}
	} else {
		if (onGround_) {
			velocity_.x *= (1.0f - kAttenuation);
			if (std::abs(velocity_.x) < 0.01f) {
				velocity_.x = 0.0f;
			}
		}
	}
	//
	velocity_ = Add(velocity_, acceleration);
	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	//
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 0.0166f;
		float destinationRotationYTable[]{
		    std::numbers::pi_v<float> / -2.0f,
		    std::numbers::pi_v<float> / 2.0f,
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		float easing = 1 - turnTimer_ / kTimeTurn;
		worldTransform_.rotation_.y = std::lerp(turnFirstRotation_, destinationRotationY, easing);
	}

	// jump
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}
	} else {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	//
	bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 0.0f) {
			worldTransform_.translation_.y = 0.0f;
			landing = true;
		}
	}
	if (landing) {
		velocity_.x *= (1.0f - kAttenuationLanding);
		velocity_.y = 0.0f;
		onGround_ = true;
	}
}
#pragma endregion

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) const {
	Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f,  -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {kWidth / 2.0f,  kHeight / 2.0f,  0},
        {-kWidth / 2.0f, kHeight / 2.0f,  0}
    };

	return Add(center, offsetTable[static_cast<uint32_t>(corner)]);
}

// top
#pragma region Top Collision
void Player::CheckCollisionTop(CollisionMapInfo& info) {

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}
	if (info.move.y <= 0) {
		return;
	}

	MapChipType mapChipType{};
	bool hit = false;

	// top left collision
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	} else {
		// top right collision
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition({worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y + kHeight / 2.0f, 0});
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight + 0.2f);

		info.isCeilingCollision = true;
	}
}
#pragma endregion
// bottom
#pragma region Bottom Collision
void Player::CheckCollisionBottom(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}
	if (info.move.y >= 0) {
		return;
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;

	// bottom left
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kLeftBottom], Vector3(0, -kCollisionEpsilon, 0)));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// bottom right
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kRightBottom], Vector3(0, -kCollisionEpsilon, 0)));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition({worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y - kHeight / 2.0f, 0});
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kBlank);

		info.isLanding = true;
	} else {
		onGround_ = false;
	}
}

#pragma endregion
// left
#pragma region Left Collision
void Player::CheckCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	/// bool isOnGround = false;

	// Left bottom
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// Left top
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kLeftTop], Vector3(-kAttenuationWall, 0, 0)));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition({worldTransform_.translation_.x - kWidth / 2.0f, worldTransform_.translation_.y - kHeight / 2.0f, 0});
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x + kBlank);

		info.hitWall = true;
	}
}
#pragma endregion
// right
#pragma region Right Collision
void Player::CheckCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	/// bool isOnGround = false;

	// Right bottom
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// Right top
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kRightTop], Vector3(kAttenuationWall, 0, 0)));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition({worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y - kHeight / 2.0f, 0});
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x - kBlank);

		info.hitWall = true;
	}
}
#pragma endregion

void Player::CollisionResult(const CollisionMapInfo& info) { worldTransform_.translation_ = Add(worldTransform_.translation_, info.move); }

void Player::CeilingCollision(const CollisionMapInfo& info) {
	if (info.isCeilingCollision) {
		velocity_.y = 0;
	}
}

void Player::WallCollision(const CollisionMapInfo& info) {
	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::SwitchGroundState(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
			}
			MapChipType mapChipType;
			bool hit = false;
			MapChipField::IndexSet indexSet;

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kLeftBottom], Vector3(0, kCollisionEpsilon, 0)));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kRightBottom], Vector3(0, kCollisionEpsilon, 0)));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				velocity_.y -= kGravityAcceleration;
				velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
			}
		}
	} else {
		if (info.isLanding) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

void Player::CheckCollision(CollisionMapInfo& info) {
	CheckCollisionTop(info);
	CheckCollisionBottom(info);
	CheckCollisionLeft(info);
	CheckCollisionRight(info);

	CeilingCollision(info);
	CollisionResult(info);
	SwitchGroundState(info);
	WallCollision(info);
}

void Player::Update() {
	playerMove();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	CheckCollision(collisionMapInfo);

	worldTransform_.UpdateMatrix();
}

const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }