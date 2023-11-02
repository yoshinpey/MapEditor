#include <string>
#include <cstdio>
#include <iostream>

#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"

#include "Engine/Fbx.h"
#include "Engine/Debug.h"

#include "resource.h"
#include "Stage.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), 
    seed(0), allUpFlag_(false), allDownFlag_(false), perlinFlag_(false), collarFlag_(false),
    mode_(0), select_(0)
{
    // モデル初期化
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    // 構造体初期化
    ResetStage();
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
    // モデルの読み込み
    LoadModels();
}

void Stage::Update()
{

    // ステージの高さマップをランダムに生成
    if (perlinFlag_)
    {
        PerlinEXE();
        perlinFlag_ = false;
    }

    // 全体を上げる処理
    if (allUpFlag_)
    {
        AllUp();
        allUpFlag_ = false;
    }

    // 全体を下げる処理
    if (allDownFlag_)
    {
        AllDown();
        allDownFlag_ = false;
    }

    // ブロックタイプを高さに応じて変更
    if (collarFlag_)
    {
        ChangeBlockTypeByHeight();
        collarFlag_ = false;
    }

    // ----------------以下、レイの判定--------------------
    if (!Input::IsMouseButtonDown(0)) return;  // マウスボタンが押されていない場合は処理しない
    if (Input::IsKey(DIK_LALT)) return;  // ALTキーが押されている場合は処理しない

    // スクリーンサイズ
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);

    XMMATRIX vp =
    {
        w,  0,  0,  0,
        0,  -h, 0,  0,
        0,  0,  1,  0,
        w,  h,  0,  1
    };

    // ビューポート
    XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
    // プロジェクション変換
    XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    // ビュー変換
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    // マウス座標取得
    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    // マウス位置をベクトルに変換
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);
    // マウス位置ベクトルにinvVP、InvProj、invViewをかける
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * invProj * invView);

    // マウス後ろ座標取得
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * invProj * invView);

    // 判定後のXZ
    int XX = -1, ZZ = -1;
    // 適当な大きい値で初期化
    float minDist = 9999999;

    // ステージサイズ
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_+1; y++)
            {
                // マウス位置前ベクトルからマウス位置後ろベクトルにレイを打つ
                RayCastData data{};
                XMStoreFloat4(&data.start, vMousePosFront);
                XMStoreFloat4(&data.dir, vMousePosBack - vMousePosFront);

                Transform blockTrans;
                blockTrans.position_ = {(float)x,(float)y,(float)z};

                Model::SetTransform(hModel_[DEFAULT], blockTrans);
                Model::RayCast(hModel_[DEFAULT],data);

                // レイが当たったとき
                if (data.hit)
                {
                    // 初回ヒット
                    if (minDist > data.dist)
                    {
                        minDist = data.dist;
                        XX = x;
                        ZZ = z;
                    }
                }
            }
        }
    }

    // レイが当たった後
    if (XX >= 0)
    {
        // 編集モード変更
        switch (mode_)
        {
        case 0:
            table_[XX][ZZ].height_++;
            break;
        case 1:
            if (table_[XX][ZZ].height_ > 0)
                table_[XX][ZZ].height_--;
            break;
        case 2:
            table_[XX][ZZ].type_ = static_cast<BOX_TYPE>(select_);
            break;
        }
    }
}

void Stage::Draw()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_ + 1; y++)
            {
                Transform blockTrans;
                blockTrans.position_ = { (float)x,(float)y,(float)z };

                int type = table_[x][z].type_;

                Model::SetTransform(hModel_[type], blockTrans);
                Model::Draw(hModel_[type]);
            }

        }
    }
}

void Stage::Release()
{
}

void Stage::SetBlockType(int _x, int _z, BOX_TYPE _type)
{
    table_[_x][_z].type_ = _type;
}


void Stage::SetBlockHeight(int _x, int _z, int _height)
{
    table_[_x][_z].height_ = _height;
}

void Stage::LoadModels()
{
    // ファイルパス
    std::string failBase = "Assets/";
    std::string modelName[] =
    {
        "BoxDefault.fbx",
        "BoxBrick.fbx",
        "BoxGrass.fbx",
        "BoxSand.fbx",
        "BoxWater.fbx",
    };

    // モデルデータのロード
    for (int i = 0; i < TYPEMAX; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }

    // ブロックタイプ初期化
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }
}

// ダイアログ
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"デフォルト");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"レンガ");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"草原");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"砂地");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"水");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_SETCURSEL, 0, 0);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wp))
        {
        case IDC_RADIO_UP:
            mode_ = 0;
            return TRUE;

        case IDC_RADIO_DOWN:
            mode_ = 1;
            return TRUE;

        case IDC_RADIO_CHANGE:
            mode_ = 2;
            return TRUE;

        case IDC_COMBO:
            select_ = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_GETCURSEL, 0, 0);
            return TRUE;

        case IDC_ALL_UP:
            allUpFlag_ = true;
            return TRUE;

        case IDC_ALL_DOWN:
            allDownFlag_ = true;
            return TRUE;

        case IDC_PERLIN:
            perlinFlag_ = true;
            return TRUE;
        case IDC_COLLAR:
            collarFlag_ = true;
        }
        return FALSE;

    }
    return FALSE;
}

// バイナリファイルでセーブロード
void Stage::Save()
{
    char fileName[MAX_PATH] = "無題.map";  // ファイル名を格納する変数

    // 上書き保存ダイアログの設定
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")
        TEXT("すべてのファイル(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;   // 上書き保存の確認ダイアログを表示
    ofn.lpstrDefExt = "map";          // デフォルト拡張子

    // 上書き保存ダイアログを表示
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    // キャンセルした場合は中断
    if (selFile == FALSE) return;

    FILE* file;
    if (fopen_s(&file, fileName, "wb") != 0)
    {
        // ファイルを開けなかった場合エラー
        std::cerr << "ファイルを開けませんでした。" << std::endl;
        return;
    }

    // ステージ情報をバイナリ形式でファイルに書き込む
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2] = { table_[x][z].type_, table_[x][z].height_ };
            if (fwrite(blockInfo, sizeof(int), 2, file) != 2)
            {
                // 書き込みに失敗した場合エラー
                std::cerr << "ファイルへの書き込みに失敗しました。" << std::endl;
                fclose(file);
                return;
            }
        }
    }
    // ファイルを閉じる
    fclose(file);
}

void Stage::Load()
{
    char fileName[MAX_PATH] = "";  // ファイル名を格納する変数

    // 「ファイルを開く」ダイアログの設定
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")
        TEXT("すべてのファイル(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;   // 存在するファイルしか選べないフラグ
    ofn.lpstrDefExt = "map";          // デフォルト拡張子

    // 「ファイルを開く」ダイアログを表示
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    // キャンセルした場合は中断
    if (selFile == FALSE) return;

    FILE* file;
    if (fopen_s(&file, fileName, "rb") != 0)
    {
        // ファイルを開けなかった場合エラー
        std::cerr << "ファイルを開けませんでした。" << std::endl;
        return;
    }

    // ステージ情報をバイナリ形式で読み込む
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2];
            if (fread(blockInfo, sizeof(int), 2, file) != 2)
            {
                // 読み込みに失敗した場合エラー
                std::cerr << "ファイルからの読み込みに失敗しました。" << std::endl;
                fclose(file);
                return;
            }
            table_[x][z].type_ = static_cast<BOX_TYPE>(blockInfo[0]);
            table_[x][z].height_ = blockInfo[1];
        }
    }

    // ファイルを閉じる
    fclose(file);
}

// 平面に戻す
void Stage::ResetStage()
{
    // 構造体初期化
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }
}

// ステージの高さマップをPerlin Noiseを使用して生成
void Stage::GenerateRandomHeightMap(unsigned int seed)
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            // Perlin Noiseを使って高さを計算
            double xCoord = static_cast<double>(x) / static_cast<double>(SIZE_X) * 3.0;
            double zCoord = static_cast<double>(z) / static_cast<double>(SIZE_Z) * 3.0;

            double height = perlin.noise(xCoord, zCoord, 0.0) * 10;

            // 高さを設定
            table_[x][z].height_ = static_cast<int>(height);
        }
    }
}

// 地形全体を上げる
void Stage::AllUp()
{

    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            table_[x][z].height_++;
        }
    }
}

// 地形全体を下げる
void Stage::AllDown()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            if (table_[x][z].height_ > 0)
            {
                table_[x][z].height_--;
            }
        }
    }
}

// パーリンノイズ実行用
void Stage::PerlinEXE()
{
    seed = rand() % 100000;
    GenerateRandomHeightMap(seed);
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            if (table_[x][z].height_ < 0)
                table_[x][z].height_ = 1;
        }
    }
}

// 高さに応じてブロックタイプを変更
void Stage::ChangeBlockTypeByHeight()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int height = table_[x][z].height_;
            BOX_TYPE newBlockType;

            if (height <= 0)
                newBlockType = WATER;
            else if (height <= 2)
                newBlockType = SAND;
            else if (height <= 5)
                newBlockType = BRICK;
            else
                newBlockType = GRASS;

            SetBlockType(x, z, newBlockType);
        }
    }
}