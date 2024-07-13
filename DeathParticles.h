#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <numbers>
 #include <algorithm> 
#include "cassert"

class DeathParticles {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	bool IsFinished() const { return isFinished_; }

private:
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;
	//
	bool isFinished_ = false;
	float counter_ = 0.0f;
	float timer_ = 1.0f / 60.0f;
	//
	static inline const float kDuration = 2.0f;
	static inline const float kSpeed = 0.1f;
	static inline const float kAngleUnit = 2 * std::numbers::pi_v<float> / 8;
	//
	ObjectColor objectColor_;
	Vector4 color_;
	//
	std::array<Model*, kNumParticles> model_;
	ViewProjection* viewProjection_ = nullptr;
};