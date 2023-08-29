#include <string>

#include "Engine/Model.h"

#include "Stage.h"

//コンストラクタ
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{0,0,0,0,0}, width_(15), height_(15)
{
}

//デストラクタ
Stage::~Stage()
{
}

//初期化
void Stage::Initialize()
{
    std::string failBase = "Assets/";
    std::string modelName[] =
    {
        "BoxDefault.fbx",
        "BoxBrick.fbx",
        "BoxGrass.fbx",
        "BoxSand.fbx",
        "BoxWater.fbx"
    };
    
    //モデルデータのロード
    for (int i = 0; i < MODEL_NUM; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_ >= 0);
    }
}

//更新
void Stage::Update()
{
}

//描画
void Stage::Draw()
{
    Transform blockTrans;

    for (int x = 0; x < width_; x++)
    {
        for (int z = 0; z < height_; z++)
        {
            blockTrans.position_.x = x;
            blockTrans.position_.z = z;

            Model::SetTransform(hModel_, blockTrans);
            Model::Draw(hModel_);
        }
    }
}

//開放
void Stage::Release()
{
}

//pacman
#if 0
#include "Stage.h"

#include "Engine/Model.h"
#include "Engine/CsvReader.h"

//コンストラクタ
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1 }, table_(nullptr)
{
    CsvReader csv;
    csv.Load("map_01.csv");

    width_ = csv.GetWidth();    //１行に何個データがあるか
    height_ = csv.GetHeight();   //データが何行あるか

    table_ = new int* [width_];

    for (int x = 0; x < width_; x++)
    {
        table_[x] = new int[height_];
        for (int z = 0; z < height_; z++)
        {
            table_[x][height_ - 1 - z] = csv.GetValue(x, z);
        }
    }
}

//デストラクタ
Stage::~Stage()
{
}

//初期化
void Stage::Initialize()
{
    const char* fileName[] =
    {
        "Floor.fbx",
        "Wall.fbx"
    };

    //モデルデータのロード
    for (int i = 0; i < TYPE_MAX; i++)
    {
        hModel_[i] = Model::Load(fileName[i]);
        assert(hModel_[i] >= 0);
    }


}

//更新
void Stage::Update()
{
}

//描画
void Stage::Draw()
{
    Transform blockTrans;

    for (int x = 0; x < width_; x++)
    {
        for (int z = 0; z < height_; z++)
        {
            blockTrans.position_.x = x;
            blockTrans.position_.z = z;

            int type = table_[x][z];

            Model::SetTransform(hModel_[type], blockTrans);
            Model::Draw(hModel_[type]);
        }
    }

}

//開放
void Stage::Release()
{
    for (int x = 0; x < width_; x++)
    {
        delete[] table_[x];
    }
    delete[] table_;
}

//壁床判定
bool Stage::IsWall(int x, int z)
{
    if (table_[x][z] == 0)
    {
        return true;
    }
    return false;
}
#endif