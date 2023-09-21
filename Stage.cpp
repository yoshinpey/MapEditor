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
    // ����������
    srand((unsigned int)time(nullptr));

    // ���f��������
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    // �\���̏�����
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
    // �e�X�g�p�̗���������
    srand((unsigned int)time(nullptr));

    // ���f���̓ǂݍ���
    LoadModels();

}

void Stage::Update()
{
    // �}�E�X�{�^�������ĂȂ��Ƃ��͑������^�[���Ōv�Z���Ȃ�
    if (!Input::IsMouseButtonDown(0)) return;
    // ALT�������Ă���Ƃ��͌v�Z���Ȃ�
    if (Input::IsKey(DIK_LALT)) return;

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
    XMMATRIX InvProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    
    // �r���[�ϊ�
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    // �}�E�X���W�擾
    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;

    // �}�E�X�ʒu�O���x�N�g���ɕϊ�
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);

    // �}�E�X�ʒu�x�N�g����invVP�AInvProj�AinvView��������
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * InvProj * invView);

    // �}�E�X���ʒu���x�N�g���ɕϊ�
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);

    // �}�E�X���x�N�g����invVP�AInvProj�AinvView��������
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * InvProj * invView);

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
                blockTrans.position_.x = x;
                blockTrans.position_.y = y;
                blockTrans.position_.z = z;

                Model::SetTransform(hModel_[0], blockTrans);
                Model::RayCast(hModel_[0],data);

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
            {
                table_[XX][ZZ].height_--;
            }
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
        }
        return FALSE;
    }
    return FALSE;
}

void Stage::Save()
{
    char fileName[MAX_PATH] = "����.map";  //�t�@�C����������ϐ�

    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	//���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	//�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	//�\���̂̃T�C�Y
    ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")    //�t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");               //�t�@�C���̎��
    ofn.lpstrFile = fileName;               	      //�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	    //�p�X�̍ő啶����
    ofn.Flags = OFN_OVERWRITEPROMPT;   		    //�t���O�i�����t�@�C�������݂�����㏑���m�F�j
    ofn.lpstrDefExt = "map";                  	//�f�t�H���g�g���q

    /*
    �u�t���O�v�̕�����
    �㏑���ۑ����邩�m�F����FOFN_OVERWRITEPROMPT(�ۑ��̎��̓R��)
    ���݂���t�@�C�������I�ׂȂ��FOFN_FILEMUSTEXIST(�J�����̓R��)
    */

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;


    HANDLE hFile;
    hFile = CreateFile
    (
        fileName,                   //�t�@�C����
        GENERIC_WRITE,              //�A�N�Z�X���[�h
        0,                          //���L�i�Ȃ��j
        NULL,                       //�Z�L�����e�B�����i�p�����Ȃ��j
        CREATE_ALWAYS,              //�쐬���@
        FILE_ATTRIBUTE_NORMAL,      //�����ƃt���O�i�ݒ�Ȃ��j
        NULL                        //�g�������i�Ȃ��j
    );
    /*
    �������݁FGENERIC_WRITE  
    �ǂݍ��݁FGENERIC_READ
    */
    std::string data = "";

    DWORD bytes = 0;
    WriteFile
    (
        hFile,              //�t�@�C���n���h��
        "ABCDEF",          //�ۑ�������������
        12,                 //�ۑ����镶����
        &bytes,             //�ۑ������T�C�Y
        NULL
    );
    CloseHandle(hFile);
}
