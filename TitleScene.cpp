#include "TitleScene.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_ = position_;
	viewProjection_.Initialize();
	model_ = Model::CreateFromOBJ("title", true);

	/// 3D
	model_skydome = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &viewProjection_);
}


void TitleScene::Update() {
	skydome_->Update();
	time_ += speed_ * 1.0f / 60.0f;
	worldTransform_.translation_.y = position_.y + amplitude_ * std::sin(time_);

	worldTransform_.UpdateMatrix();


	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	model_->Draw(worldTransform_, viewProjection_);
	// draw skydome
	skydome_->Draw();
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
}
