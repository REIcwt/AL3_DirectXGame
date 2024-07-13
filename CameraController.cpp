#include "CameraController.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Vector3 CameraController::Lerp(const Vector3& start, const Vector3& end, float t) { return {start.x + t * (end.x - start.x), start.y + t * (end.y - start.y), start.z + t * (end.z - start.z)}; }

void CameraController::Initialize(ViewProjection* viewProjection) {
	viewProjection_ = viewProjection; 
}

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	Vector3 targetVelocity = target_->GetVelocity();
	if (target_->cameraStopY == true) {
		targetVelocity.y = 0;
	}
	if (target_->cameraStopX == true) {
		targetVelocity.x = 0;
	}
	targetPosition = Add(Add(targetWorldTransform.translation_, targetOffset_), MultiplyVF(targetVelocity, kVelocityBias));
	viewProjection_->translation_.x = std::lerp(viewProjection_->translation_.x, targetPosition.x, kInterpolationRate);
	viewProjection_->translation_.y = std::lerp(viewProjection_->translation_.y, targetPosition.y, kInterpolationRate);

	  viewProjection_->translation_.x =
	    std::clamp(viewProjection_->translation_.x, targetWorldTransform.translation_.x + kCameraMargins.left, targetWorldTransform.translation_.x + kCameraMargins.right);
	viewProjection_->translation_.y =
	    std::clamp(viewProjection_->translation_.y, targetWorldTransform.translation_.y + kCameraMargins.bottom, targetWorldTransform.translation_.y + kCameraMargins.top);


	// 移動範囲制限
	viewProjection_->translation_.x = std::clamp(viewProjection_->translation_.x, movableArea_.left, movableArea_.right);
	viewProjection_->translation_.y = std::clamp(viewProjection_->translation_.y, movableArea_.bottom, movableArea_.top);

	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {
	// ワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// カメラの座標を計算
	viewProjection_->translation_ = Add(targetWorldTransform.translation_, targetOffset_);
	viewProjection_->translation_.x = std::max(movableArea_.left, std::min(viewProjection_->translation_.x, movableArea_.right));
	viewProjection_->translation_.y = std::max(movableArea_.bottom, std::min(viewProjection_->translation_.y, movableArea_.top));
}