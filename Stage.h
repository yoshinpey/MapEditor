#pragma once
#include "Engine/GameObject.h"
#include <time.h>
#include <Windows.h>

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
    static const int SIZE_X{ 15 };
    static const int SIZE_Z{ 15 };
    static const int SIZE_Y{ 0 };
}

//�X�e�[�W���Ǘ�����N���X
class Stage : public GameObject
{
   
    //�Ђƃ}�X�̏��
    struct BoxInfo
    {
        BOX_TYPE type_;
        int height_;
    };

    int hModel_[TYPEMAX];               // ���f��
    BoxInfo table_[SIZE_X][SIZE_Z];     // �X�e�[�W�̏��

    int mode_;                          // 0:�グ��   1:������   2:��ޕύX 
    int select_;                        // ���

    // ���f���f�[�^�̓ǂݍ���
    void LoadModels();

    // ���W�I�{�^���̑���
    void HandleRadioButton(WPARAM wp);

    // �R���{�{�b�N�X�̑���
    void HandleComboBox(WPARAM wp);

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

    //�_�C�A���O
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    // �X�e�[�W�T�C�Y���擾����֐�
    XMFLOAT3 getSize() const
    {
        return XMFLOAT3(static_cast<float>(SIZE_X), static_cast<float>(SIZE_Y), static_cast<float>(SIZE_Z));
    }
};
