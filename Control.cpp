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
    XMFLOAT3 stageSize = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);

    ///////////////// カメラ焦点の初期位置をマップ中央に移動
    XMFLOAT3 camTar = { SIZE_X / 2.0f, SIZE_Y, SIZE_Z / 2.0f };
    transform_.position_ = camTar;
}

void Control::Update()
{
    if (Input::IsMouseButton(0)&& Input::IsKey(DIK_LALT))
    {
        // Mouseで操作
        UseMouse();
    }
    else
    {
        // キーボードで操作
        UseKey();
    }

    // カメラ上昇
    if (Input::IsKey(DIK_SPACE))
    {
        transform_.position_.y += 0.08f;
    }
    // カメラ下降
    if (Input::IsKey(DIK_LSHIFT))
    {
        transform_.position_.y -= 0.08f;
    }

    // 位置座標を移動ベクトルに変換
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);

    // 回転行列を作成、デグリーをラジアンに変換
    XMMATRIX mRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(transform_.rotate_.x),XMConvertToRadians(transform_.rotate_.y),0.0f);

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

void Control::UseMouse()
{
    // マウスの移動量を取得
    XMFLOAT3 mouseMove = Input::GetMouseMove();

    // マウスの移動量に基づいてカメラを回転させる
    transform_.rotate_.y += (mouseMove.x) * 0.3f; // 水平方向の回転
    transform_.rotate_.x += (mouseMove.y) * 0.3f; // 垂直方向の回転

    // 垂直方向の回転を制限する
    if (transform_.rotate_.x > 60.0f)
        transform_.rotate_.x = 60.0f;
    else if (transform_.rotate_.x < -60.0f)
        transform_.rotate_.x = -60.0f;

    // カメラの回転行列を計算
    XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMMATRIX mView = mRotX * mRotY;

    //拡大後の位置フォワードべくった作る
    //トランスフォームノーマル
    //ベクトルの方向に知つけ

    // カメラの位置と焦点を設定
    XMFLOAT3 camPosFloat3;
    XMFLOAT3 camTargetFloat3;
    XMVECTOR camPosVector = XMVectorSet(0, 0, 0, 1); // カメラの位置
    camPosVector = XMVector3TransformCoord(camPosVector, mView);
    XMVECTOR camTargetVector = XMVectorSet(0, 0, 1, 1); // カメラの焦点
    camTargetVector = XMVector3TransformCoord(camTargetVector, mView);
    XMStoreFloat3(&camPosFloat3, camPosVector);
    XMStoreFloat3(&camTargetFloat3, camTargetVector);
    Camera::SetPosition(camPosFloat3);
    Camera::SetTarget(camTargetFloat3);
}

void Control::UseKey()
{
    float rotateSpeed = 1.0f;

    // カメラ回転
    if (Input::IsKey(DIK_LEFT))
    {
        transform_.rotate_.y -= rotateSpeed;
    }
    if (Input::IsKey(DIK_RIGHT))
    {
        transform_.rotate_.y += rotateSpeed;
    }
    if (Input::IsKey(DIK_UP))
    {
        transform_.rotate_.x -= rotateSpeed;

        // 下回転の角度制限
        if (transform_.rotate_.x <= -30.0f)
        {
            transform_.rotate_.x = -30.0f;
        }
    }
    if (Input::IsKey(DIK_DOWN))
    {
        transform_.rotate_.x += rotateSpeed;

        // 上回転の角度制限
        if (transform_.rotate_.x >= 50.0f)
        {
            transform_.rotate_.x = 50.0f;
        }
    }
}
