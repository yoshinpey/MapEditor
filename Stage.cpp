#include "Engine/Model.h"
#include "Stage.h"

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), hModel_(-1), width_(15), height_(15)
{
}

//�f�X�g���N�^
Stage::~Stage()
{
}

//������
void Stage::Initialize()
{
    //���f���f�[�^�̃��[�h
    hModel_ = Model::Load("Assets/BoxBrick.fbx");
    assert(hModel_ >= 0);
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