#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Model.h"

#include "Control.h"
#include "Stage.h"

Control::Control(GameObject* parent)
    :GameObject(parent, "Control")
{
}

Control::~Control()
{
}

void Control::Initialize()
{
    ///////////////// �J�����œ_�̏����ʒu���}�b�v�����Ɉړ�
    XMFLOAT3 camTar = { 7.5f, 0.0f, 7.5f };
    transform_.position_ = camTar;
}

void Control::Update()
{
    if (Input::IsMouseButton(0)&& Input::IsKey(DIK_LALT))
    {
        // Mouse�ő���
        UseMouse();
    }
    else
    {
        // �L�[�{�[�h�ő���
        UseKey();
    }

    // �J�����㏸
    if (Input::IsKey(DIK_SPACE))
    {
        transform_.position_.y += 0.05f;
    }
    // �J�������~
    if (Input::IsKey(DIK_LSHIFT))
    {
        transform_.position_.y -= 0.05f;
    }

    // �ʒu���W���ړ��x�N�g���ɕϊ�
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

    // ��]�s����쐬�A�f�O���[�����W�A���ɕϊ�
    XMMATRIX mRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(transform_.rotate_.x),XMConvertToRadians(transform_.rotate_.y),0.0f);

    // �ړ��x�N�g����ϊ�
    XMVECTOR vMoveForward = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);        // Z����
    XMVECTOR vMoveRight = XMVectorSet(0.1f, 0.0f, 0.0f, 0.0f);          // X����
    vMoveForward = XMVector3TransformCoord(vMoveForward, mRot);
    vMoveRight = XMVector3TransformCoord(vMoveRight, mRot);

    // �O�i
    if (Input::IsKey(DIK_W))
    {
        vPos += vMoveForward;
    }
    // ���
    if (Input::IsKey(DIK_S))
    {
        vPos -= vMoveForward;
    }
    // �E�ړ�
    if (Input::IsKey(DIK_D))
    {
        vPos += vMoveRight;
    }
    // ���ړ�
    if (Input::IsKey(DIK_A))
    {
        vPos -= vMoveRight;
    }

    // �x�N�g���^���t���[�g3�^�ɖ߂�
    XMStoreFloat3(&transform_.position_, vPos);

    // �J�����ʒu
    XMVECTOR vCam = { 0, 5, -10, 0 };
    vCam = XMVector3TransformCoord(vCam, mRot);
    XMFLOAT3 camPos;
    XMStoreFloat3(&camPos, vPos + vCam);
    Camera::SetPosition(camPos);

    // �J�����œ_
    XMFLOAT3 camTar = transform_.position_;
    camTar.x += XMVectorGetX(vMoveForward);
    camTar.z += XMVectorGetZ(vMoveForward);
    Camera::SetTarget(camTar);

    //���C�L���X�g
    /*
    Stage* pStage = (Stage*)FindObject("Stage");    //�X�e�[�W�I�u�W�F�N�g��T��
    int hGroundModel = pStage->GetModelHandle();    //���f���ԍ����擾

    RayCastData data;
    data.start = transform_.position_;   //���C�̔��ˈʒu
    data.dir = XMFLOAT3(0, -1, 0);       //���C�̕���
    Model::RayCast(hGroundModel, &data); //���C�𔭎�
    
    //���C������������
    if (data.hit)
    {
        //���̕��ʒu��������
        transform_.position_.y -= data.dist;
    }
    */
}

void Control::Draw()
{
}

void Control::Release()
{
}

void Control::UseMouse()
{
    // �}�E�X�̈ړ��ʂ��擾
    XMFLOAT3 mouseMove = Input::GetMouseMove();

    // �}�E�X�̈ړ��ʂɊ�Â��ăJ��������]������
    transform_.rotate_.y += (mouseMove.x) * 0.3f; // ���������̉�]
    transform_.rotate_.x += (mouseMove.y) * 0.3f; // ���������̉�]

    // ���������̉�]�𐧌�����
    if (transform_.rotate_.x > 60.0f)
        transform_.rotate_.x = 60.0f;
    else if (transform_.rotate_.x < -60.0f)
        transform_.rotate_.x = -60.0f;

    // �J�����̉�]�s����v�Z
    XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMMATRIX mView = mRotX * mRotY;

    // �J�����̈ʒu�Əœ_��ݒ�
    XMFLOAT3 camPosFloat3;
    XMFLOAT3 camTargetFloat3;
    XMVECTOR camPosVector = XMVectorSet(0, 0, 0, 1); // �J�����̈ʒu
    camPosVector = XMVector3TransformCoord(camPosVector, mView);
    XMVECTOR camTargetVector = XMVectorSet(0, 0, 1, 1); // �J�����̏œ_
    camTargetVector = XMVector3TransformCoord(camTargetVector, mView);
    XMStoreFloat3(&camPosFloat3, camPosVector);
    XMStoreFloat3(&camTargetFloat3, camTargetVector);
    Camera::SetPosition(camPosFloat3);
    Camera::SetTarget(camTargetFloat3);
}

void Control::UseKey()
{
    // �J������]
    if (Input::IsKey(DIK_LEFT))
    {
        transform_.rotate_.y -= 1.0f;
    }
    if (Input::IsKey(DIK_RIGHT))
    {
        transform_.rotate_.y += 1.0f;
    }
    if (Input::IsKey(DIK_UP))
    {
        transform_.rotate_.x -= 1.0f;

        // ����]�̊p�x����
        if (transform_.rotate_.x <= -30.0f)
        {
            transform_.rotate_.x = -30.0f;
        }
    }
    if (Input::IsKey(DIK_DOWN))
    {
        transform_.rotate_.x += 1.0f;

        // ���]�̊p�x����
        if (transform_.rotate_.x >= 50.0f)
        {
            transform_.rotate_.x = 50.0f;
        }
    }
}
