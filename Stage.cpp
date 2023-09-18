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
    //乱数初期化
    srand((unsigned int)time(nullptr));

    //モデル初期化
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    //構造体初期化
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
    //ファイルパス
    std::string failBase = "Assets/";
    std::string modelName[] =
    {
        "BoxDefault.fbx",
        "BoxBrick.fbx",
        "BoxGrass.fbx",
        "BoxSand.fbx",
        "BoxWater.fbx",
    };

    //モデルデータのロード
    for (int i = 0; i <TYPEMAX; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }

    //モデル読み込み
    for (int x = 0; x < SIZE_X; x++) 
    {
        for (int z = 0; z < SIZE_Z; z++) 
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }

}

void Stage::Update()
{
    if (!Input::IsMouseButtonDown(0)) {
        return;
    }
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);
    //offsetx,y = 0;
    //minZ=0,maxZ =1
    XMMATRIX vp =
    {
        w,  0,  0,  0,
        0,  -h, 0,  0,
        0,  0,  1,  0,
        w,  h,  0,  1
    };
    //ビューポート
    XMMATRIX invVP = XMMatrixInverse(nullptr, vp);

    //プロジェクション変換
    XMMATRIX InvProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    
    //ビュー変換
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;

    //マウス位置前をベクトルに変換
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);
    //それにinvVP、InvProj、invViewをかける
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * InvProj * invView);
    //マウス後ろ位置をベクトルに変換
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);
    //それにinvVP、InvProj、invViewをかける
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * InvProj * invView);

    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_+1; y++)
            {
                //マウス位置前ベクトルからマウス後ろ位置ベクトルにレイを打つ
                RayCastData data{};
                XMStoreFloat4(&data.start, vMousePosFront);
                XMStoreFloat4(&data.dir, vMousePosBack - vMousePosFront);

                Transform blockTrans;
                blockTrans.position_.x = x;
                blockTrans.position_.y = y;
                blockTrans.position_.z = z;

                Model::SetTransform(hModel_[0], blockTrans);
                Model::RayCast(hModel_[0],data);

                //レイが当たったら伸ばす
                if (data.hit)
                {
                    table_[x][z].height_++;
                    break;
                }
            }
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

//ダイアログ
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        //ラジオボタン初期化
        SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);

        //コンボボックス初期化
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, DEFAULT, (LPARAM)"デフォルト");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, BRICK, (LPARAM)"レンガ");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, GRASS, (LPARAM)"草原");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, SAND, (LPARAM)"砂地");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, WATER, (LPARAM)"水");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_SETCURSEL, TYPEMAX, 0);
        return TRUE;
    }
    return FALSE;
}