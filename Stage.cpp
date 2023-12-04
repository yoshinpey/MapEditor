#include <string>
#include <cstdio>
#include <iostream>

#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"

#include "Engine/Fbx.h"
#include "Engine/Debug.h"

#include "resource.h"
#include "Stage.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), 
    seed(0), allUpFlag_(false), allDownFlag_(false), perlinFlag_(false), collarFlag_(false),
    mode_(0), select_(0)
{
    // ���f��������
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    // �\���̏�����
    ResetStage();
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
    // ���f���̓ǂݍ���
    LoadModels();
}

void Stage::Update()
{

    // �X�e�[�W�̍����}�b�v�������_���ɐ���
    if (perlinFlag_)
    {
        PerlinEXE();
        perlinFlag_ = false;
    }

    // �S�̂��グ�鏈��
    if (allUpFlag_)
    {
        AllUp();
        allUpFlag_ = false;
    }

    // �S�̂������鏈��
    if (allDownFlag_)
    {
        AllDown();
        allDownFlag_ = false;
    }

    // �u���b�N�^�C�v�������ɉ����ĕύX
    if (collarFlag_)
    {
        ChangeBlockTypeByHeight();
        collarFlag_ = false;
    }

    // ----------------�ȉ��A���C�̔���--------------------
    if (!Input::IsMouseButtonDown(0)) return;  // �}�E�X�{�^����������Ă��Ȃ��ꍇ�͏������Ȃ�
    if (Input::IsKey(DIK_LALT)) return;  // ALT�L�[��������Ă���ꍇ�͏������Ȃ�

    // �X�N���[���T�C�Y
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);

    XMMATRIX vp =
    {
        w,  0,  0,  0,
        0,  -h, 0,  0,
        0,  0,  1,  0,
        w,  h,  0,  1
    };

    // �r���[�|�[�g
    XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
    // �v���W�F�N�V�����ϊ�
    XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    // �r���[�ϊ�
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    // �}�E�X���W�擾
    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    // �}�E�X�ʒu���x�N�g���ɕϊ�
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);
    // �}�E�X�ʒu�x�N�g����invVP�AInvProj�AinvView��������
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * invProj * invView);

    // �}�E�X�����W�擾
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * invProj * invView);

    // ������XZ
    int XX = -1, ZZ = -1;
    // �K���ȑ傫���l�ŏ�����
    float minDist = 9999999;

    // �X�e�[�W�T�C�Y
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_+1; y++)
            {
                // �}�E�X�ʒu�O�x�N�g������}�E�X�ʒu���x�N�g���Ƀ��C��ł�
                RayCastData data{};
                XMStoreFloat4(&data.start, vMousePosFront);
                XMStoreFloat4(&data.dir, vMousePosBack - vMousePosFront);

                Transform blockTrans;
                blockTrans.position_ = {(float)x,(float)y,(float)z};

                Model::SetTransform(hModel_[DEFAULT], blockTrans);
                Model::RayCast(hModel_[DEFAULT],data);

                // ���C�����������Ƃ�
                if (data.hit)
                {
                    // ����q�b�g
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

    // ���C������������
    if (XX >= 0)
    {
        // �ҏW���[�h�ύX
        switch (mode_)
        {
        case 0:
            table_[XX][ZZ].height_++;
            break;
        case 1:
            if (table_[XX][ZZ].height_ > 0)
                table_[XX][ZZ].height_--;
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
                blockTrans.position_ = { (float)x,(float)y,(float)z };

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
    // �t�@�C���p�X
    std::string failBase = "Assets/";
    std::string modelName[] =
    {
        "BoxDefault.fbx",
        "BoxBrick.fbx",
        "BoxGrass.fbx",
        "BoxSand.fbx",
        "BoxWater.fbx",
    };

    // ���f���f�[�^�̃��[�h
    for (int i = 0; i < TYPEMAX; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }

    // �u���b�N�^�C�v������
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }
}

// �_�C�A���O
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"�f�t�H���g");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"�����K");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"����");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"���n");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"��");
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

        case IDC_ALL_UP:
            allUpFlag_ = true;
            return TRUE;

        case IDC_ALL_DOWN:
            allDownFlag_ = true;
            return TRUE;

        case IDC_PERLIN:
            perlinFlag_ = true;
            return TRUE;
        case IDC_COLLAR:
            collarFlag_ = true;
        }
        return FALSE;

    }
    return FALSE;
}

// �o�C�i���t�@�C���ŃZ�[�u���[�h
void Stage::Save()
{
    char fileName[MAX_PATH] = "����.map";  // �t�@�C�������i�[����ϐ�

    // �㏑���ۑ��_�C�A���O�̐ݒ�
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;   // �㏑���ۑ��̊m�F�_�C�A���O��\��
    ofn.lpstrDefExt = "map";          // �f�t�H���g�g���q

    // �㏑���ۑ��_�C�A���O��\��
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    // �L�����Z�������ꍇ�͒��f
    if (selFile == FALSE) return;

    FILE* file;
    if (fopen_s(&file, fileName, "wb") != 0)
    {
        // �t�@�C�����J���Ȃ������ꍇ�G���[
        std::cerr << "�t�@�C�����J���܂���ł����B" << std::endl;
        return;
    }

    // �X�e�[�W�����o�C�i���`���Ńt�@�C���ɏ�������
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2] = { table_[x][z].type_, table_[x][z].height_ };
            if (fwrite(blockInfo, sizeof(int), 2, file) != 2)
            {
                // �������݂Ɏ��s�����ꍇ�G���[
                std::cerr << "�t�@�C���ւ̏������݂Ɏ��s���܂����B" << std::endl;
                fclose(file);
                return;
            }
        }
    }
    // �t�@�C�������
    fclose(file);
}

void Stage::Load()
{
    char fileName[MAX_PATH] = "";  // �t�@�C�������i�[����ϐ�

    // �u�t�@�C�����J���v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;   // ���݂���t�@�C�������I�ׂȂ��t���O
    ofn.lpstrDefExt = "map";          // �f�t�H���g�g���q

    // �u�t�@�C�����J���v�_�C�A���O��\��
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    // �L�����Z�������ꍇ�͒��f
    if (selFile == FALSE) return;

    FILE* file;
    if (fopen_s(&file, fileName, "rb") != 0)
    {
        // �t�@�C�����J���Ȃ������ꍇ�G���[
        std::cerr << "�t�@�C�����J���܂���ł����B" << std::endl;
        return;
    }

    // �X�e�[�W�����o�C�i���`���œǂݍ���
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2];
            if (fread(blockInfo, sizeof(int), 2, file) != 2)
            {
                // �ǂݍ��݂Ɏ��s�����ꍇ�G���[
                std::cerr << "�t�@�C������̓ǂݍ��݂Ɏ��s���܂����B" << std::endl;
                fclose(file);
                return;
            }
            table_[x][z].type_ = static_cast<BOX_TYPE>(blockInfo[0]);
            table_[x][z].height_ = blockInfo[1];
        }
    }

    // �t�@�C�������
    fclose(file);
}

// ���ʂɖ߂�
void Stage::ResetStage()
{
    // �\���̏�����
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }
}

// �X�e�[�W�̍����}�b�v��Perlin Noise���g�p���Đ���
void Stage::GenerateRandomHeightMap(unsigned int seed)
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            // Perlin Noise���g���č������v�Z
            double xCoord = static_cast<double>(x) / static_cast<double>(SIZE_X) * 3.0;
            double zCoord = static_cast<double>(z) / static_cast<double>(SIZE_Z) * 3.0;

            double height = perlin.noise(xCoord, zCoord, 0.0) * 10;

            // ������ݒ�
            table_[x][z].height_ = static_cast<int>(height);
        }
    }
}

// �n�`�S�̂��グ��
void Stage::AllUp()
{

    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            table_[x][z].height_++;
        }
    }
}

// �n�`�S�̂�������
void Stage::AllDown()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            if (table_[x][z].height_ > 0)
            {
                table_[x][z].height_--;
            }
        }
    }
}

// �p�[�����m�C�Y���s�p
void Stage::PerlinEXE()
{
    seed = rand() % 100000;
    GenerateRandomHeightMap(seed);
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            if (table_[x][z].height_ < 0)
                table_[x][z].height_ = 1;
        }
    }
}

// �����ɉ����ău���b�N�^�C�v��ύX
void Stage::ChangeBlockTypeByHeight()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int height = table_[x][z].height_;
            BOX_TYPE newBlockType;

            if (height <= 0)
                newBlockType = WATER;
            else if (height <= 2)
                newBlockType = SAND;
            else if (height <= 5)
                newBlockType = BRICK;
            else
                newBlockType = GRASS;

            SetBlockType(x, z, newBlockType);
        }
    }
}