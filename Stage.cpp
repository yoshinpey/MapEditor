#include <string>

#include "Engine/Model.h"

#include "Stage.h"

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{0,0,0,0,0}, width_(15), height_(15)
{
}

//�f�X�g���N�^
Stage::~Stage()
{
}

//������
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
    
    //���f���f�[�^�̃��[�h
    for (int i = 0; i < MODEL_NUM; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_ >= 0);
    }
}

//�X�V
void Stage::Update()
{
}

//�`��
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

//�J��
void Stage::Release()
{
}

//pacman
#if 0
#include "Stage.h"

#include "Engine/Model.h"
#include "Engine/CsvReader.h"

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_{ -1,-1 }, table_(nullptr)
{
    CsvReader csv;
    csv.Load("map_01.csv");

    width_ = csv.GetWidth();    //�P�s�ɉ��f�[�^�����邩
    height_ = csv.GetHeight();   //�f�[�^�����s���邩

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

//�f�X�g���N�^
Stage::~Stage()
{
}

//������
void Stage::Initialize()
{
    const char* fileName[] =
    {
        "Floor.fbx",
        "Wall.fbx"
    };

    //���f���f�[�^�̃��[�h
    for (int i = 0; i < TYPE_MAX; i++)
    {
        hModel_[i] = Model::Load(fileName[i]);
        assert(hModel_[i] >= 0);
    }


}

//�X�V
void Stage::Update()
{
}

//�`��
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

//�J��
void Stage::Release()
{
    for (int x = 0; x < width_; x++)
    {
        delete[] table_[x];
    }
    delete[] table_;
}

//�Ǐ�����
bool Stage::IsWall(int x, int z)
{
    if (table_[x][z] == 0)
    {
        return true;
    }
    return false;
}
#endif