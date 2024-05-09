#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cassert"

class Player {
public:
	Player();
	~Player();

	
	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	/// texture
	 uint32_t textureHandle_ = 0u;
	/// 3D
	 Model* model_ = nullptr;
	/// ViewProjection
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
};
