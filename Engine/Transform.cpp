#include "Transform.h"

Transform::Transform() :
    matTranslate_(XMMatrixIdentity()),
    matRotate_(XMMatrixIdentity()),
    matScale_(XMMatrixIdentity()),
    position_(XMFLOAT3(0, 0, 0)),
    rotate_(XMFLOAT3(0, 0, 0)),
    scale_(XMFLOAT3(1, 1, 1))
{
}

Transform::~Transform()
{
}

//変形
void Transform::Calclation()
{
    //移動行列
    matTranslate_ = XMMatrixTranslation(position_.x, position_.y, position_.z);

    //回転行列
    XMMATRIX rotateX, rotateY, rotateZ;
    rotateX = XMMatrixRotationX(XMConvertToRadians(rotate_.x));
    rotateY = XMMatrixRotationY(XMConvertToRadians(rotate_.y));
    rotateZ = XMMatrixRotationZ(XMConvertToRadians(rotate_.z));
    matRotate_ = rotateZ * rotateX * rotateY;

    //拡大縮小
    matScale_ = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
}

//ワールド行列を取得
XMMATRIX Transform::GetWorldMatrix()
{
    if (pParent_ != nullptr)
    {
        return  matScale_ * matRotate_ * matTranslate_ *pParent_->GetWorldMatrix();
    }
    else
    {
        return matScale_ * matRotate_ * matTranslate_;
    }
}

//法線変形用行列を取得
XMMATRIX Transform::GetNormalMatrix()
{
    return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}