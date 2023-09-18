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
    //����������
    srand((unsigned int)time(nullptr));

    //���f��������
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    //�\���̏�����
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
    //�t�@�C���p�X
    std::string failBase = "Assets/";
    std::string modelName[] =
    {
        "BoxDefault.fbx",
        "BoxBrick.fbx",
        "BoxGrass.fbx",
        "BoxSand.fbx",
        "BoxWater.fbx",
    };

    //���f���f�[�^�̃��[�h
    for (int i = 0; i <TYPEMAX; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }

    //���f���ǂݍ���
    for (int x = 0; x < SIZE_X; x++) 
    {
        for (int z = 0; z < SIZE_Z; z++) 
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }

}

void Stage::Update()
{
    if (!Input::IsMouseButtonDown(0)) {
        return;
    }
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);
    //offsetx,y = 0;
    //minZ=0,maxZ =1
    XMMATRIX vp =
    {
        w,  0,  0,  0,
        0,  -h, 0,  0,
        0,  0,  1,  0,
        w,  h,  0,  1
    };
    //�r���[�|�[�g
    XMMATRIX invVP = XMMatrixInverse(nullptr, vp);

    //�v���W�F�N�V�����ϊ�
    XMMATRIX InvProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    
    //�r���[�ϊ�
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;

    //�}�E�X�ʒu�O���x�N�g���ɕϊ�
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);
    //�����invVP�AInvProj�AinvView��������
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * InvProj * invView);
    //�}�E�X���ʒu���x�N�g���ɕϊ�
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);
    //�����invVP�AInvProj�AinvView��������
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * InvProj * invView);

    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_+1; y++)
            {
                //�}�E�X�ʒu�O�x�N�g������}�E�X���ʒu�x�N�g���Ƀ��C��ł�
                RayCastData data{};
                XMStoreFloat4(&data.start, vMousePosFront);
                XMStoreFloat4(&data.dir, vMousePosBack - vMousePosFront);

                Transform blockTrans;
                blockTrans.position_.x = x;
                blockTrans.position_.y = y;
                blockTrans.position_.z = z;

                Model::SetTransform(hModel_[0], blockTrans);
                Model::RayCast(hModel_[0],data);

                //���C������������L�΂�
                if (data.hit)
                {
                    table_[x][z].height_++;
                    break;
                }
            }
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

//�_�C�A���O
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        //���W�I�{�^��������
        SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);

        //�R���{�{�b�N�X������
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, DEFAULT, (LPARAM)"�f�t�H���g");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, BRICK, (LPARAM)"�����K");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, GRASS, (LPARAM)"����");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, SAND, (LPARAM)"���n");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, WATER, (LPARAM)"��");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_SETCURSEL, TYPEMAX, 0);
        return TRUE;
    }
    return FALSE;
}