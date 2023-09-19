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
    static const int SIZE_X{ 15 };
    static const int SIZE_Z{ 15 };
    static const int SIZE_Y{ 0 };
}

//ステージを管理するクラス
class Stage : public GameObject
{
   
    //ひとマスの情報
    struct BoxInfo
    {
        BOX_TYPE type_;
        int height_;
    };

    int hModel_[TYPEMAX];               // モデル
    BoxInfo table_[SIZE_X][SIZE_Z];     // ステージの情報

    int mode_;                          // 0:上げる   1:下げる   2:種類変更 
    int select_;                        // 種類

    // モデルデータの読み込み
    void LoadModels();

    // ラジオボタンの操作
    void HandleRadioButton(WPARAM wp);

    // コンボボックスの操作
    void HandleComboBox(WPARAM wp);

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

    // ステージサイズを取得する関数
    XMFLOAT3 getSize() const
    {
        return XMFLOAT3(static_cast<float>(SIZE_X), static_cast<float>(SIZE_Y), static_cast<float>(SIZE_Z));
    }
};
