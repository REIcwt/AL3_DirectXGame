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
#include "DeathParticles.h"

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

	void CheckAllCollision();

	bool IsDead() const { return isDead_; }

	//cahmge scene
	bool IsFinished() const { return finished_; }

private: 
	
	enum class Phase {
		kPlay,
		kDeath,
	};
	Phase phase_;	
	
	void ChangePhase();

	// change scene
	bool finished_ = false;
	
	// メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// player
	Player* player_ = nullptr;
	Model* modelPlayer_ = nullptr;
	bool isDead_ = false;
	bool playerDead_ = false;

	// Enemy
	Enemy* enemy_ = nullptr;
	Model* enemyModel_ = nullptr;
	static inline const int32_t kEnemyNum = 1;
	std::list<Enemy*> enemies_;

	//
	DeathParticles* deathParticles_ = nullptr;
	Model* deathParticlesModel_ = nullptr;

	// skydome
	Skydome* skydome_ = nullptr;
	Model* model_ = nullptr;
	Model* modelSkydome_ = nullptr;
	//mapchip insert
	MapChipField* mapChipField_;

	//
	uint32_t textureHandle_ = 0;
	// 2D
	Sprite* sprite_ = nullptr;
	// 3D
	ViewProjection viewProjection_;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	//Cam
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	CameraController* cameraController_ = nullptr;

	// Goal 
	Model* goalModel_ = nullptr;
	WorldTransform goalTransform_;
	bool goalReached_ = false;
};