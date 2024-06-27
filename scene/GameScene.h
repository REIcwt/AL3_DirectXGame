#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "vector"
#include "DebugCamera.h"
#include "Skydome.h"
#include"MapChipField.h"
#include "TextureManager.h"
#include "CameraController.h"
#include "Enemy.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// creat mapchip blocks
	/// </summary>
	void GenerateBlocks(){
		// num
		uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
		uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

		// world change
		worldTransformBlocks_.resize(numBlockVirtical);
		for (uint32_t i = 0; i < numBlockVirtical; ++i) {
			worldTransformBlocks_[i].resize(numBlockHorizontal);
		}

		// creat
		for (uint32_t i = 0; i < numBlockVirtical; ++i) {
			for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					WorldTransform* worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformBlocks_[i][j] = worldTransform;
					worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				}
			}
		}
	};

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// player
	Player* player_ = nullptr;
	// Enemy
	Enemy* enemy_ = nullptr;
	// skydome
	Skydome* skydome_ = nullptr;

	//mapchip insert
	MapChipField* mapChipField_;

	//
	uint32_t textureHandle_ = 0;
	// 2D
	Sprite* sprite_ = nullptr;
	// 3D
	ViewProjection viewProjection_;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	Model* model_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* enemyModel_ = nullptr;

	//Cam
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	CameraController* cameraController_ = nullptr;


	/// <summary>
	/// ゲームシーン用
	/// </summary>
};