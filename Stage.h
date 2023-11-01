#pragma once
#include "Engine/GameObject.h"
#include "PerlinNoise.h"
#include <time.h>
#include <Windows.h>
#include <stack>

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
    static const int SIZE_Y{ 10 };

    // Undo用の変更履歴構造体
    struct StageChange
    {
        int x;
        int z;
        BOX_TYPE type;
        int height;
    };
}

//ステージを管理するクラス
class Stage : public GameObject
{
    // 自動生成用のインスタンス
    PerlinNoise perlin;
    unsigned int seed;

    //ひとマスの情報
    struct Block
    {
        BOX_TYPE type_;
        int height_;
    };

    int hModel_[TYPEMAX];               // モデル
    Block table_[SIZE_X][SIZE_Z];     // ステージの情報

    int mode_;                          // 0:上げる   1:下げる   2:種類変更 
    int select_;                        // 種類

    // モデルデータの読み込み
    void LoadModels();

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

    //ブロックの種類を設定
    void SetBlockType(int _x, int _z, BOX_TYPE _type);

    //ブロックの高さを設定
    void SetBlockHeight(int _x, int _z, int _height);

    //ダイアログ
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //セーブ機能
    void Save();

    //ロード機能
    void Load();

    //ファイルからの読み取り
    bool ReadLineFromFile(HANDLE hFile, char* buffer, DWORD bufferSize);

    //新規作成
    void ResetStage();


    void GenerateRandomHeightMap();

    void GenerateRandomHeightMap(unsigned int seed);


    // ステージサイズを取得する関数
    XMFLOAT3 getSize() const
    {
        return XMFLOAT3(static_cast<float>(SIZE_X), static_cast<float>(SIZE_Y), static_cast<float>(SIZE_Z));
    }
};
