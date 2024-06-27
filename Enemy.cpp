#include "Enemy.h"
#include <numbers>
#include <algorithm>
#include "Input.h"
#include <array>
#include "MapChipField.h"
#include "DebugCamera.h"

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = float(M_PI_2) * 3;

	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	worldTransform_.translation_.x += velocity_.x;

	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.rotation_.x = std::sin(2 * float(M_PI) * (walkTimer_ / kWalkMotionTime));
	float param = std::sin(2 * float(M_PI) * (walkTimer_ / kWalkMotionTime));
	float radian = kInitialWalkMotionAngle + (kFinalWalkMotionAngle * (param + 1.0f) / 2.0f);
	worldTransform_.rotation_.x = (float(M_PI) * radian) / 180.0f;

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }