#include <string>

#include "Engine/Model.h"
#include "Engine/Fbx.h"
#include "Engine/Debug.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"

#include "resource.h"
#include "Stage.h"



Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage")
{
    // 乱数初期化
    srand((unsigned int)time(nullptr));

    // モデル初期化
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    // 構造体初期化
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x,z, DEFAULT);
        }
    }
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
    // テスト用の乱数初期化
    srand((unsigned int)time(nullptr));

    // モデルの読み込み
    LoadModels();

}

void Stage::Update()
{
    // マウスボタン押してないときは早期リターンで計算しない
    if (!Input::IsMouseButtonDown(0)) return;
    // ALTを押しているときは計算しない
    if (Input::IsKey(DIK_LALT)) return;

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
    XMMATRIX InvProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    
    // ビュー変換
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    // マウス座標取得
    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;

    // マウス位置前をベクトルに変換
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);

    // マウス位置ベクトルにinvVP、InvProj、invViewをかける
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * InvProj * invView);

    // マウス後ろ位置をベクトルに変換
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);

    // マウス後ろベクトルにinvVP、InvProj、invViewをかける
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * InvProj * invView);

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
                blockTrans.position_.x = x;
                blockTrans.position_.y = y;
                blockTrans.position_.z = z;

                Model::SetTransform(hModel_[0], blockTrans);
                Model::RayCast(hModel_[0],data);

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
            {
                table_[XX][ZZ].height_--;
            }
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
                blockTrans.position_.x = x;
                blockTrans.position_.z = z;
                blockTrans.position_.y = y;

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
        }
        return FALSE;
    }
    return FALSE;
}

void Stage::Save()
{
    char fileName[MAX_PATH] = "無題.map";  //ファイル名を入れる変数

    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	//名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	//構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	//構造体のサイズ
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")    //ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");               //ファイルの種類
    ofn.lpstrFile = fileName;               	      //ファイル名
    ofn.nMaxFile = MAX_PATH;               	    //パスの最大文字数
    ofn.Flags = OFN_OVERWRITEPROMPT;   		    //フラグ（同名ファイルが存在したら上書き確認）
    ofn.lpstrDefExt = "map";                  	//デフォルト拡張子

    /*
    「フラグ」の部分は
    上書き保存するか確認する：OFN_OVERWRITEPROMPT(保存の時はコレ)
    存在するファイルしか選べない：OFN_FILEMUSTEXIST(開く時はコレ)
    */

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;


    HANDLE hFile;
    hFile = CreateFile
    (
        fileName,                   //ファイル名
        GENERIC_WRITE,              //アクセスモード
        0,                          //共有（なし）
        NULL,                       //セキュリティ属性（継承しない）
        CREATE_ALWAYS,              //作成方法
        FILE_ATTRIBUTE_NORMAL,      //属性とフラグ（設定なし）
        NULL                        //拡張属性（なし）
    );
    /*
    書き込み：GENERIC_WRITE  
    読み込み：GENERIC_READ
    */
    std::string data = "";

    DWORD bytes = 0;
    WriteFile
    (
        hFile,              //ファイルハンドル
        "ABCDEF",          //保存したい文字列
        12,                 //保存する文字数
        &bytes,             //保存したサイズ
        NULL
    );
    CloseHandle(hFile);
}
