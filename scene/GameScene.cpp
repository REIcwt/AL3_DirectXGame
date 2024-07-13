#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <string>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	delete player_;
	delete enemy_;
	delete modelSkydome_;
	delete mapChipField_;
	delete cameraController_;
	delete deathParticles_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();
	}
}

void GameScene::Initialize() {

	//
	phase_ = Phase::kPlay;
	//

	debugCamera_ = new DebugCamera(1280, 720);

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	/// 2D
	textureHandle_ = TextureManager::Load("cube/cube.jpg");
	/// 3D
	model_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	// mapchip
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// create player
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3,18);
	player_->Initialize(modelPlayer_, &viewProjection_, playerPosition);

	 player_->SetMapChipField(mapChipField_);

	 // Enemy
	 enemy_ = new Enemy;
	 enemyModel_ = Model::CreateFromOBJ("enemy", true);
	 for (int32_t i = 0; i < kEnemyNum; i++) {
		 Enemy* newEnemy = new Enemy;
		 Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10 + (i * 2), 18);
		 newEnemy->Initialize(enemyModel_, &viewProjection_, enemyPosition);

		 enemies_.push_back(newEnemy);
	 }

	 //
	

	//cameraController
	 cameraController_ = new CameraController();
	 cameraController_->Initialize(&viewProjection_);
	 CameraController::Rect area_;
	 area_.left = 0.0f;
	 area_.right = 200.0f;
	 area_.bottom = 0.0f;
	 area_.top = 200.0f;
	 cameraController_->SetMovableArea(area_);
	 cameraController_->SetTarget(player_);
	 cameraController_->Reset();

	//make far view 
	viewProjection_.farZ = 20000.0f;
	viewProjection_.Initialize();
}

void GameScene::Update() {

	ChangePhase();

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		cameraController_->Update();
		viewProjection_.UpdateMatrix();
	}

#endif
	
	  if (phase_ == Phase::kDeath) {
        if (cameraController_ && player_) {
            cameraController_->SetTarget(player_);
            cameraController_->Update();
        }
    }

}


void GameScene::CheckAllCollision() {
#pragma region player to enemy collision
	AABB aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		AABB aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kPlay:
		skydome_->Update();
		player_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		cameraController_->Update();

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (worldTransformBlock) {
					worldTransformBlock->UpdateMatrix();
				}
			}
		}

		CheckAllCollision();

		if (deathParticles_) {
			deathParticles_->Update();
		}

		if (player_->IsDead()) {
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles;
			deathParticlesModel_ = Model::CreateFromOBJ("DeathParticles", true);
			deathParticles_->Initialize(deathParticlesModel_, &viewProjection_, deathParticlesPosition);
			if (player_) {
				playerDead_ = true;
				player_->isVisible_ = false;
				cameraController_->SetTarget(player_);
			}
			phase_ = Phase::kDeath;
		}

		break;

	case GameScene::Phase::kDeath:
		skydome_->Update();
		if (playerDead_ && cameraController_) {
			cameraController_->Update();
		}

		if (enemy_) {
			for (Enemy* enemy : enemies_) {
				enemy->Update();
			}
		}
		if (deathParticles_) {
			deathParticles_->Update();
		}

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				worldTransformBlock->TransferMatrix();
			}
		}

		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_ = true;
		}

		break;

	default:
		break;
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	//draw skydome
	skydome_->Draw();

	//draw blocks
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			model_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	// player
	if (player_) {
		player_->Draw();
	}

	//enemy
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	if (deathParticles_) {
		deathParticles_->Draw();
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}