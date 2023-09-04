#pragma once
#include "Engine/GameObject.h"
#include <time.h>

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
    const int MODEL_NUM{ TYPEMAX };     //���f����

    //���E�l
    const int SIZE_X{ 15 };
    const int SIZE_Z{ 15 };
    const int SIZE_Y{ 5 };
}

//�X�e�[�W���Ǘ�����N���X
class Stage : public GameObject
{
    //�Ђƃ}�X�̏��
    struct
    {
        BOX_TYPE type_;
        int height_;
    }table_[SIZE_X][SIZE_Z];

    int hModel_[TYPEMAX];      //���f��

public:
    //�R���X�g���N�^
    Stage(GameObject* parent);
    //�f�X�g���N�^
    ~Stage();
    //������
    void Initialize() override;
    //�X�V
    void Update() override;
    //�`��
    void Draw() override;
    //�J��
    void Release() override;

    //�z�u����u���b�N�̎��
    void SetBlockType(int _x, int _z, BOX_TYPE _type);
    //�z�u����u���b�N�̍���
    void SetBlockHeight(int _x, int _z, int _height);
    //���C�L���X�g
    int GetModelHandle() { return hModel_[_type]; }
};
