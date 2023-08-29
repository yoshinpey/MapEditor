#pragma once
#include "Engine/GameObject.h"

enum
{
    TYPE_DEFAULT=0,
    TYPE_BRICK,
    TYPE_GRASS,
    TYPE_SAND,
    TYPE_WATER,
    TYPE_MAX
};

//�X�e�[�W���Ǘ�����N���X
class Stage : public GameObject
{
    int hModel_[TYPE_MAX];      //���f��
    int sizeY_, sizeX_, sizeZ_; //�X�e�[�W�T�C�Y
    int** table_;               //�n��
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
};

//pacman
#if 0
#pragma once
#include "Engine/GameObject.h"

enum
{
    TYPE_FLOOR,
    TYPE_WALL,
    TYPE_MAX,
};

//�n�ʂ��Ǘ�����N���X
class Stage : public GameObject
{
    int hModel_[TYPE_MAX];
    int** table_;
    int height_;
    int width_;
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

    //���ƕǂ𔻒肷�邽�߂̃Q�b�^�[(�ʂ��or�ʂ�Ȃ�)
    //����     �F��,��        ���ׂ�ʒu
    //�߂�l   �F             ���W
    bool IsWall(int x, int z);

};
#endif