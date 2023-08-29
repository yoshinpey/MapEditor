#pragma once
#include "Engine/GameObject.h"

enum
{
    TYPE_DEFAULT=0,
    TYPE_BRICK,
    TYPE_GRASS,
    TYPE_SAND,
    TYPE_WATER,
    TYPE_MAX
};

//ステージを管理するクラス
class Stage : public GameObject
{
    int hModel_[TYPE_MAX];      //モデル
    int sizeY_, sizeX_, sizeZ_; //ステージサイズ
    int** table_;               //地面
public:
    //コンストラクタ
    Stage(GameObject* parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;
};

//pacman
#if 0
#pragma once
#include "Engine/GameObject.h"

enum
{
    TYPE_FLOOR,
    TYPE_WALL,
    TYPE_MAX,
};

//地面を管理するクラス
class Stage : public GameObject
{
    int hModel_[TYPE_MAX];
    int** table_;
    int height_;
    int width_;
public:
    //コンストラクタ
    Stage(GameObject* parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

    //床と壁を判定するためのゲッター(通れるor通れない)
    //引数     ：ｘ,ｚ        調べる位置
    //戻り値   ：             座標
    bool IsWall(int x, int z);

};
#endif