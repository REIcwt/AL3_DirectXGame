#define NOMINMAX
#include "Player.h"
#include <numbers>
#include <algorithm>
#include "Input.h"
#include <array>
#include "MapChipField.h"
#include "DebugCamera.h"

Player::Player(){};
Player::~Player(){};

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);
	model_ = model;

	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	CheckCollision(collisionMapInfo);
	ReflectCollisionResult(collisionMapInfo);
	HandleCeilingCollision(collisionMapInfo);

	//control move
	if (onGround_) {
		// LEFT RIGHT
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			//
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				// Move
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}
				acceleration.x += kAcceleration;

				// direction
				if (lrDirection_ != LRDirection::kRight) {
					turnFirstRotation_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
					lrDirection_ = LRDirection::kRight;
				}

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				// Move
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}
				acceleration.x -= kAcceleration;

				// direction
				if (lrDirection_ != LRDirection::kLeft) {
					turnFirstRotation_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
					lrDirection_ = LRDirection::kLeft;
				}
			}

			// acceleration
			velocity_ = Add(velocity_, acceleration);
			// Max speed
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

			if (turnTimer_ > 0.0f) {
				turnTimer_ -= 0.0166f;

				// change direction
				float destinationRotationYTable[]{
				    std::numbers::pi_v<float> / -2.0f,
				    std::numbers::pi_v<float> / 2.0f,
				};

				float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
				float easing = 1 - turnTimer_ / kTimeTurn;
				float nowRotationY = std::lerp(turnFirstRotation_, destinationRotationY, easing);
				worldTransform_.rotation_.y = nowRotationY;
			}
		} else {
			velocity_.x *= (1.0f - kAttenuation);
			if (velocity_.x * velocity_.x <= 0.01f) {
				velocity_.x = 0.0f;
			}
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ = Add(velocity_, Vector3(0, kJumpAcceleration, 0));
		}
	} else {
		// down speed
		velocity_ = Add(velocity_,Vector3(0,-kGravityAcceleration,0));
		//down limit
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	//
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	//
	bool landing = false;
	//if player touch ground
	if (velocity_.y<0) {
		if (worldTransform_.translation_.y<=2.0f) {
			landing = true;
		}
	}

	//
	if (onGround_) {
		if (velocity_.y>0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 2.0f;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}	

	worldTransform_.UpdateMatrix();
}

void Player::CheckCollision(CollisionMapInfo& info) {
	CheckCollisionTop(info);
	CheckCollisionBottom(info);
	CheckCollisionLeft(info);
	CheckCollisionRight(info);
}

//top
void Player::CheckCollisionTop(CollisionMapInfo& info) {
	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_ , info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	//left top collision
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	//right top collision
	 indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	 if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y);

		info.isCeilingCollision = true;
	}
}
//bottom
void Player::CheckCollisionBottom(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}
}
//left
void Player::CheckCollisionLeft(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}
}
//right
void Player::CheckCollisionRight(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) const {
	Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f,  -kHeight / 2.0f, 0},
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0},
	    {kWidth / 2.0f,  kHeight / 2.0f,  0},
	    {-kWidth / 2.0f, kHeight / 2.0f,  0}
	};

	return Add(center, offsetTable[static_cast<uint32_t>(corner)]);
}

void Player::ReflectCollisionResult(const CollisionMapInfo& info) { 
	worldTransform_.translation_ = Add(worldTransform_.translation_, info.move); 
}

void Player::HandleCeilingCollision(const CollisionMapInfo& info) {
	if (info.isCeilingCollision) {
		///DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }