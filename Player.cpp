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
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
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
#pragma region Top Collision
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

	//top left collision
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	//top right collision
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
#pragma endregion
//bottom
#pragma region Bottom Collision
void Player::CheckCollisionBottom(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// bottom left
	MapChipField::IndexSet indexSet;
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
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y);

		info.isGroundCollision = true;
	}
}
#pragma endregion
//left
#pragma region Left Collision
void Player::CheckCollisionLeft(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}
}
#pragma endregion
//right
#pragma region Right Collision
void Player::CheckCollisionRight(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
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

void Player::ReflectCollisionResult(const CollisionMapInfo& info) { 
	worldTransform_.translation_ = Add(worldTransform_.translation_, info.move); 
}

void Player::HandleCeilingCollision(const CollisionMapInfo& info) {
	if (info.isCeilingCollision) {
		///DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::SwitchGroundState(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			bool hit = false;

			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
			}

			//bottom left
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kLeftBottom], Vector3(0, -kCollisionEpsilon, 0)));
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
				hit = true;
			}

			//bottom right
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kRightBottom], Vector3(0, -kCollisionEpsilon, 0)));
			if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
				hit = true;
			}
			
			if (!hit) {
				onGround_ = false;
			}

		}
	} else {
		if (info.isGroundCollision) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

const WorldTransform& Player::GetWorldTransform() const { return worldTransform_; }

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }