#include "Engine/Input.h"
#include "Engine/Camera.h"

#include "Control.h"

Control::Control(GameObject* parent)
    :GameObject(parent, "Control")
{
}

Control::~Control()
{
}

void Control::Initialize()
{
    // �J�����œ_�̏����ʒu���}�b�v�����Ɉړ�
    XMFLOAT3 camTar = { 7.5f, 0.0f, 7.5f };
    transform_.position_ = camTar;
}

void Control::Update()
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
    if (Input::IsKey(DIK_UP)) // �J�����O��]
    {
        transform_.rotate_.x -= 1.0f;
    }
    if (Input::IsKey(DIK_DOWN)) // �J�������]
    {
        transform_.rotate_.x += 1.0f;
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
    XMMATRIX mRot = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    mRot *= XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));

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
}

void Control::Draw()
{
}

void Control::Release()
{
}
