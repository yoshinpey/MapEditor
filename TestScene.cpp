#include "Engine/Input.h"
#include "Engine/SceneManager.h"

#include "TestScene.h"
#include "Control.h"
#include "Stage.h"

TestScene::TestScene(GameObject* parent)
	:GameObject(parent, "TestScene")
{
}

void TestScene::Initialize()
{
	Instantiate<Stage>(this);
	Instantiate<Control>(this);
}

void TestScene::Update()
{
	//if (Input::IsKey(DIK_SPACE))
	//{
	//	SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
	//	pSceneManager->ChangeScene(SCENE_ID_TEST);
	//}
}

void TestScene::Draw()
{
}

void TestScene::Release()
{
}