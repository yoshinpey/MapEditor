#pragma once
#include "Engine/GameObject.h"

// ステージを管理するクラス
class Control : public GameObject
{

public:
    // コンストラクタ
    Control(GameObject* parent);

    // デストラクタ
    ~Control();

    // 初期化
    void Initialize() override;

    // 更新
    void Update() override;

    // 描画
    void Draw() override;

    // 開放
    void Release() override;

    // マウスで動かす場合
    void UseMouse();

    // キーボードで動かす場合
    void UseKey();
};