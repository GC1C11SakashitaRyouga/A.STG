#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"ImGuiManager.h"
#include"MathUtilityForText.h"

GameScene::GameScene() {};

GameScene::~GameScene() { 
	delete spriteBG_;//BG
	delete modelStage_;//ステージ
	delete modelPlayer_;//プレイヤー
	delete modelBeam_;//ビーム
}
//初期化
void GameScene::Initialize() { 

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0.0});

	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	 textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();
	
	//ステージの位置を変更
	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	
	//ステージ
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	
	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformBeam_.Initialize();

	
	
	//変更行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	 worldTransformStage_.scale_, 
		worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	//変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();
	
}

void GameScene::Update() { 
	PlayerUpdate();
	BeamUpdate();
	BeamMove();
	BeamBorn();
}

void GameScene::BeamUpdate() {
	
	    worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, 
		worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformBeam_.TransferMatrix();
}

void GameScene::BeamMove() {
		if (BeamFlag == false) {
			worldTransformBeam_.translation_.z += 0.1f;
			worldTransformBeam_.rotation_.x += 0.1f;
			if (worldTransformBeam_.translation_.z > 40) {
				BeamFlag = false;
			}
		}
}

void GameScene::BeamBorn() {
	    BeamMove();
	if (input_->TriggerKey(DIK_SPACE)) {
		    worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
		    worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		    BeamFlag = false;
	    }

}

void GameScene::PlayerUpdate() {
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}   
	// 変換行列を更新
	    worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_,
		worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
}


void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	spriteBG_->Draw();
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
	// ステージ
	/// </summary>
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
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
