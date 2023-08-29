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
    // カメラ焦点の初期位置をマップ中央に移動
    XMFLOAT3 camTar = { 7.5f, 0.0f, 7.5f };
    transform_.position_ = camTar;
}

void Control::Update()
{
    // カメラ回転
    if (Input::IsKey(DIK_LEFT))
    {
        transform_.rotate_.y -= 1.0f;
    }
    if (Input::IsKey(DIK_RIGHT))
    {
        transform_.rotate_.y += 1.0f;
    }
    if (Input::IsKey(DIK_UP)) // カメラ前回転
    {
        transform_.rotate_.x -= 1.0f;
    }
    if (Input::IsKey(DIK_DOWN)) // カメラ後回転
    {
        transform_.rotate_.x += 1.0f;
    }

    // カメラ上昇
    if (Input::IsKey(DIK_SPACE))
    {
        transform_.position_.y += 0.05f;
    }
    // カメラ下降
    if (Input::IsKey(DIK_LSHIFT))
    {
        transform_.position_.y -= 0.05f;
    }

    // 位置座標を移動ベクトルに変換
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

    // 回転行列を作成、デグリーをラジアンに変換
    XMMATRIX mRot = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    mRot *= XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));

    // 移動ベクトルを変換
    XMVECTOR vMoveForward = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);        // Z方向
    XMVECTOR vMoveRight = XMVectorSet(0.1f, 0.0f, 0.0f, 0.0f);          // X方向
    vMoveForward = XMVector3TransformCoord(vMoveForward, mRot);
    vMoveRight = XMVector3TransformCoord(vMoveRight, mRot);

    // 前進
    if (Input::IsKey(DIK_W))
    {
        vPos += vMoveForward;
    }
    // 後退
    if (Input::IsKey(DIK_S))
    {
        vPos -= vMoveForward;
    }
    // 右移動
    if (Input::IsKey(DIK_D))
    {
        vPos += vMoveRight;
    }
    // 左移動
    if (Input::IsKey(DIK_A))
    {
        vPos -= vMoveRight;
    }

    // ベクトル型をフロート3型に戻す
    XMStoreFloat3(&transform_.position_, vPos);

    // カメラ位置
    XMVECTOR vCam = { 0, 5, -10, 0 };
    vCam = XMVector3TransformCoord(vCam, mRot);
    XMFLOAT3 camPos;
    XMStoreFloat3(&camPos, vPos + vCam);
    Camera::SetPosition(camPos);

    // カメラ焦点
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
