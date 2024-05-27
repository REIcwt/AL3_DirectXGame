#define NOMINMAX
#include "Player.h"
#include <numbers>
#include <algorithm>
#include "Input.h"

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

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }