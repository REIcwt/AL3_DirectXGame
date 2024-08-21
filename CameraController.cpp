#include "CameraController.h"
#include "Player.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cal.h"
#include <algorithm>

CameraController::CameraController(){};
CameraController::~CameraController(){};

Vector3 CameraController::Lerp(const Vector3& start, const Vector3& end, float t) { return {start.x + t * (end.x - start.x), start.y + t * (end.y - start.y), start.z + t * (end.z - start.z)}; }

void CameraController::Initialize(ViewProjection* viewProjection, const Vector3& position) {
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	cameraDestination = Add3(targetWorldTransform.translation_, targetOffset_, MultiplyVF(target_->GetVelocity(), kVelovityBias));

	viewProjection_->translation_ = Lerp(viewProjection_->translation_, cameraDestination, kInterpolationRate);

	viewProjection_->translation_.x = std::clamp(viewProjection_->translation_.x, movableArea_.left + kCameraMargins.left, movableArea_.right + kCameraMargins.right);

	viewProjection_->translation_.y = std::clamp(viewProjection_->translation_.y, movableArea_.bottom + kCameraMargins.bottom, movableArea_.top + kCameraMargins.top);

	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	worldTransform_.translation_ = Add(targetWorldTransform.translation_, targetOffset_);
}
