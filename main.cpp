#include"Suine.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	Suine::Initialize();

	Input* input = Input::GetInstance();

	input->Initialize();

	unique_ptr<Model> model = make_unique< Model>();
	unique_ptr<Sprite>sprite = make_unique<Sprite>();


	uint32_t texHandle = TextureManager::LoadTexture("Resource/uvChecker.png");
	uint32_t BlockTexHandle = TextureManager::LoadTexture("Resource/colisionCube/tex.png");

	model->CreateFromObj("colisionCube");
	sprite->Initialize(new SpriteBoxState, { 0,0 }, { 500,200 });
	sprite->SetTexHandle(BlockTexHandle);
	sprite->SetColor({ 1,1,1,1.0f });
	WorldTransform SpriteWorldTransform = {};
	SpriteWorldTransform.Initialize();
	SpriteWorldTransform.translate = { 0,0,0 };
	WorldTransform worldTransform = {};
	worldTransform.Initialize();

	ViewProjection viewProjection;
	viewProjection.Initialize({ 0.2f,-0.6f,0.0f }, { 11.0f,5.0f,-15 });

	BlockTexHandle;
	texHandle;
	Vector3 uvScale = { 1,1,1, };
	Vector3 uvrotate = {};
	Vector3 uvtranslate = {};

	while (WinApp::WinMsg())
	{
		Suine::BeginFlame();

		ImGui::Begin("Debugcamera");
		ImGui::SliderFloat3("trans", &viewProjection.translation_.x, -15, 15);
		ImGui::SliderFloat3("rotate", &viewProjection.rotation_.x, -1, 1);
		ImGui::End();


		ImGui::Begin("Sprite");
		ImGui::DragFloat3("scale", &SpriteWorldTransform.scale.x, 0.1f);
		ImGui::DragFloat3("rotate", &SpriteWorldTransform.rotation.x, 0.1f);
		ImGui::DragFloat3("trans", &SpriteWorldTransform.translate.x, 0.1f);

		ImGui::End();

		ImGui::Begin("colision");
		ImGui::DragFloat3("scale", &worldTransform.scale.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform.rotation.x, 0.1f);
		ImGui::DragFloat3("trans", &worldTransform.translate.x, 0.1f);
		ImGui::Text("uv");
		ImGui::DragFloat3("uvscale", &uvScale.x, 0.1f);
		ImGui::DragFloat3("uvrotate", &uvrotate.x, 0.1f);
		ImGui::DragFloat3("uvTrans", &uvtranslate.x, 0.1f);
		ImGui::End();

		

		model->SetUvRotate(uvrotate);
		model->SetUvScale(uvScale);
		model->SetUvTranslate(uvtranslate);

		worldTransform.UpdateMatrix();
		viewProjection.UpdateMatrix();
		SpriteWorldTransform.UpdateMatrix();
		DebugTools::SetViewProjection(viewProjection);
		DebugTools::Execute(0);

		if (input->PressKey(DIK_UP))
		{
			worldTransform.rotation.x += 2.0f;
		}

		model->Draw(worldTransform, viewProjection);
		sprite->Draw(SpriteWorldTransform);
		Suine::EndFlame();

	}

	Suine::Finalize();

	return 0;
}