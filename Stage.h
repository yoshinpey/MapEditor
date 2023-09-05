#pragma once
#include "Engine/GameObject.h"
#include <time.h>
#include <Windows.h>

namespace 
{
    enum BOX_TYPE
    {
        DEFAULT = 0,
        BRICK,
        GRASS,
        SAND,
        WATER,
        TYPEMAX
    };
    const int MODEL_NUM{ TYPEMAX };     //モデル数

    //限界値
    const int SIZE_X{ 15 };
    const int SIZE_Z{ 15 };
    const int SIZE_Y{ 5 };
}

//ステージを管理するクラス
class Stage : public GameObject
{
    //ひとマスの情報
    struct
    {
        BOX_TYPE type_;
        int height_;
    }table_[SIZE_X][SIZE_Z];

    int hModel_[TYPEMAX];      //モデル
    int mode_;      //0:上げる 1:下げる 2:種類を変える 
    int select_;    //種類

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

    //配置するブロックの種類
    void SetBlockType(int _x, int _z, BOX_TYPE _type);
    //配置するブロックの高さ
    void SetBlockHeight(int _x, int _z, int _height);
    //ダイアログ
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //レイキャスト
    //int GetModelHandle() { return hModel_[_type]; }
};
