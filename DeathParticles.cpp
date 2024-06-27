#include "DeathParticles.h"

void DeathParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	for (auto& deathModel : this->model_) {
		assert(model);
		deathModel = model;
	}
	viewProjection_ = viewProjection;
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};

	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticles::Update() {

	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; i++) {
		Vector3 velocity = {kSpeed, 0, 0};
		float angle = kAngleUnit * i;
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		velocity = Transform(velocity, matrixRotation);
		worldTransforms_[i].translation_ = Add(worldTransforms_[i].translation_, velocity);
	}

	color_.w = std::clamp(color_.w, 0.0f, 1.0f);
	counter_ += timer_;
	color_.w -= timer_ / 2.0f;
	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();

	if (counter_>=kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	for (auto& worldTransform : worldTransforms_) {
		worldTransform.UpdateMatrix();
	}

}

void DeathParticles::Draw() {

	if (isFinished_) {
		return;
	}

	for (size_t i = 0; i < kNumParticles; ++i) {
			model_[i]->Draw(worldTransforms_[i], *viewProjection_, &objectColor_);
	}

}
	