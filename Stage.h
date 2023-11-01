#pragma once
#include "Engine/GameObject.h"
#include "PerlinNoise.h"
#include <time.h>
#include <Windows.h>
#include <stack>

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
    static const int SIZE_Y{ 10 };

    // Undo�p�̕ύX�����\����
    struct StageChange
    {
        int x;
        int z;
        BOX_TYPE type;
        int height;
    };
}

//�X�e�[�W���Ǘ�����N���X
class Stage : public GameObject
{
    // ���������p�̃C���X�^���X
    PerlinNoise perlin;
    unsigned int seed;

    //�Ђƃ}�X�̏��
    struct Block
    {
        BOX_TYPE type_;
        int height_;
    };

    int hModel_[TYPEMAX];               // ���f��
    Block table_[SIZE_X][SIZE_Z];     // �X�e�[�W�̏��

    int mode_;                          // 0:�グ��   1:������   2:��ޕύX 
    int select_;                        // ���

    // ���f���f�[�^�̓ǂݍ���
    void LoadModels();

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

    //�u���b�N�̎�ނ�ݒ�
    void SetBlockType(int _x, int _z, BOX_TYPE _type);

    //�u���b�N�̍�����ݒ�
    void SetBlockHeight(int _x, int _z, int _height);

    //�_�C�A���O
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    //�Z�[�u�@�\
    void Save();

    //���[�h�@�\
    void Load();

    //�t�@�C������̓ǂݎ��
    bool ReadLineFromFile(HANDLE hFile, char* buffer, DWORD bufferSize);

    //�V�K�쐬
    void ResetStage();


    void GenerateRandomHeightMap();

    void GenerateRandomHeightMap(unsigned int seed);


    // �X�e�[�W�T�C�Y���擾����֐�
    XMFLOAT3 getSize() const
    {
        return XMFLOAT3(static_cast<float>(SIZE_X), static_cast<float>(SIZE_Y), static_cast<float>(SIZE_Z));
    }
};
