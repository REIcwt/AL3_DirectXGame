#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <iostream>
#include <numbers>
#include <cmath>
#include <Input.h>

class TitleScene {
public:
	TitleScene();
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }


private:
	DirectXCommon* dxCommon_ = nullptr;

	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	Model* model_ = nullptr;
	static inline const Vector3 position_ = {0, 0.0f, -45.0f};

	bool finished_ = false;

	float time_ = 0.0f;
	static constexpr float amplitude_ = 0.1f;
	static constexpr float speed_ = 5.0f;

};