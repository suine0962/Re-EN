#include"Suine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Vector4 color;

	color = { 1,1,1 };

	Suine::Initialize();

	Input* input = Input::GetInstance();

	Particle* particle = new Particle();

	input->Initialize();

	particle->Initialize(color);

	

	unique_ptr<Model> model = make_unique< Model>();
	unique_ptr<Sprite>sprite = make_unique<Sprite>();
	unique_ptr<Sprite>uvSprite = make_unique<Sprite>();

	uint32_t texHandle = TextureManager::LoadTexture("Resource/uvChecker.png");
	uint32_t BlockTexHandle = TextureManager::LoadTexture("Resource/colisionCube/tex.png");

	model->CreateFromObj("colisionCube");
	sprite->Initialize(new SpriteBoxState, { 0,0 }, { 500,200 });
	sprite->SetTexHandle(BlockTexHandle);
	sprite->SetColor({ 1,1,1,1.0f });
	uvSprite->Initialize(new SpriteBoxState, { 800,500 }, { 500,200 });
	uvSprite->SetTexHandle(texHandle);
	uvSprite->SetColor({ 1,1,1,1.0f });

	WorldTransform SpriteWorldTransform = {};
	SpriteWorldTransform.Initialize();
	SpriteWorldTransform.translate = { 0,0,0 };
	WorldTransform worldTransform = {};
	worldTransform.Initialize();
	WorldTransform uvTransform = {};
	uvTransform.Initialize();
	uvTransform.translate = { 0,0,0 };
	WorldTransform ParticleTransform = {};
	ParticleTransform.Initialize();
	ParticleTransform.translate = { 0.0f,0.0f,-50.0f };

	ViewProjection viewProjection;
	viewProjection.Initialize({ 0.2f,-0.6f,0.0f }, { 11.0f,5.0f,-15 });

	BlockTexHandle;
	texHandle;
	Vector3 uvScale = { 1,1,1, };
	Vector3 uvrotate = {};
	Vector3 uvtranslate = {};

	while (true)
	{
		if (WinApp::GetInstance()->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		Suine::BeginFlame();

		input->Update();

		/*ImGui::Begin("Debug camera");
		ImGui::SliderFloat3("trans", &viewProjection.translation_.x, -15, 15);
		ImGui::SliderFloat3("rotate", &viewProjection.rotation_.x, -1, 1);
		ImGui::End();


		ImGui::Begin("Sprite");
		ImGui::DragFloat3("scale", &SpriteWorldTransform.scale.x, 0.1f);
		ImGui::DragFloat3("rotate", &SpriteWorldTransform.rotation.x, 0.1f);
		ImGui::DragFloat3("trans", &SpriteWorldTransform.translate.x, 0.1f);

		ImGui::End();

		ImGui::Begin("collision");
		ImGui::DragFloat3("scale", &worldTransform.scale.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform.rotation.x, 0.1f);
		ImGui::DragFloat3("trans", &worldTransform.translate.x, 0.1f);
		ImGui::Text("uv");
		ImGui::DragFloat3("upscale", &uvScale.x, 0.1f);
		ImGui::DragFloat3("unrotated", &uvrotate.x, 0.1f);
		ImGui::DragFloat3("uvTrans", &uvtranslate.x, 0.1f);
		ImGui::End();*/


		

		

		model->SetUvRotate(uvrotate);
		model->SetUvScale(uvScale);
		model->SetUvTranslate(uvtranslate);

		worldTransform.UpdateMatrix();
		viewProjection.UpdateMatrix();
		SpriteWorldTransform.UpdateMatrix();
		ParticleTransform.UpdateMatrix();
		uvTransform.UpdateMatrix();
		DebugTools::SetViewProjection(viewProjection);
		DebugTools::Execute(0);

		//真ん中のモデルおを動かす処理
		if (input->PushKey(DIK_W))
		{
			worldTransform.translate.y += 0.2f;
		}

		if (input->PushKey(DIK_S))
		{
			worldTransform.translate.y -= 0.2f;
		}

		if (input->PushKey(DIK_A))
		{
			worldTransform.translate.x -= 0.2f;
		}

		if (input->PushKey(DIK_D))
		{
			worldTransform.translate.x += 0.2f;
		}

		//右下uvCheckerSpriteを動かす処理
		if (input->PushKey(DIK_RIGHT))
		{
			uvTransform.translate.x += 0.5f;
		}

		if (input->PushKey(DIK_LEFT))
		{
			uvTransform.translate.x -= 0.5f;
		}

		

		model->Draw(worldTransform, viewProjection);
		sprite->Draw(SpriteWorldTransform);
		uvSprite->Draw(uvTransform);
		particle->Draw(texHandle, color, ParticleTransform);

		Suine::EndFlame();

	}

	Suine::Finalize();

	return 0;
}