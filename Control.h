#pragma once
#include "Engine/GameObject.h"

// �X�e�[�W���Ǘ�����N���X
class Control : public GameObject
{

public:
    // �R���X�g���N�^
    Control(GameObject* parent);

    // �f�X�g���N�^
    ~Control();

    // ������
    void Initialize() override;

    // �X�V
    void Update() override;

    // �`��
    void Draw() override;

    // �J��
    void Release() override;

    // �}�E�X�œ������ꍇ
    void UseMouse();

    // �L�[�{�[�h�œ������ꍇ
    void UseKey();
};