#include <string>

#include "Engine/Model.h"
#include "Engine/Debug.h"

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
            table_[x][z] = { DEFAULT, DEFAULT };
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
            SetBlockType(x, z, (BOX_TYPE)(rand() % (int)TYPEMAX));
            SetBlockHeight(x, z, rand() % SIZE_Y);
        }
    }

}

void Stage::Update()
{
}

void Stage::Draw()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_; y++)
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