#include <string>
#include <cstdio>
#include <iostream>

#include "Engine/Model.h"
#include "Engine/Fbx.h"
#include "Engine/Debug.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"

#include "resource.h"
#include "Stage.h"

Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), perlin(0)
{
    // ����������
    srand((unsigned int)time(nullptr));

    // ���f��������
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    // �\���̏�����
    ResetStage();

    // �p�[�����m�C�Y������(�V�[�h�l���ݒ�\)
    perlin = PerlinNoise();
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
    // ���f���̓ǂݍ���
    LoadModels();

    // �X�e�[�W�̍����}�b�v�������_���ɐ���
    GenerateRandomHeightMap();
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
        }
        return FALSE;
    }
    return FALSE;
}

////////�e�L�X�g�t�@�C���ŃZ�[�u���[�h
#if 0
void Stage::Save()
{
    char fileName[MAX_PATH] = "����.map";  //�t�@�C����������ϐ�

    //�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;                         	    //���O�����ĕۑ��_�C�A���O�̐ݒ�p�\����
    ZeroMemory(&ofn, sizeof(ofn));            	    //�\���̏�����
    ofn.lStructSize = sizeof(OPENFILENAME);   	    //�\���̂̃T�C�Y
    ofn.lpstrFilter =
        TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")        //�t�@�C���̎��
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");     //�t�@�C���̎��
    ofn.lpstrFile = fileName;               	    //�t�@�C����
    ofn.nMaxFile = MAX_PATH;               	        //�p�X�̍ő啶����
    ofn.Flags = OFN_OVERWRITEPROMPT;   		        //�t���O
    ofn.lpstrDefExt = "map";                  	    //�f�t�H���g�g���q

    /*
    <�t���O>
    �㏑���ۑ����邩�m�F����FOFN_OVERWRITEPROMPT(�ۑ��̎��̓R��)
    ���݂���t�@�C�������I�ׂȂ��FOFN_FILEMUSTEXIST(�J�����̓R��)
    */

    //�u�t�@�C����ۑ��v�_�C�A���O
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //�L�����Z�������璆�f
    if (selFile == FALSE) return;

    ////�t�@�C���쐬��J��
    HANDLE hFile;
    hFile = CreateFile
    (
        fileName,                   // �t�@�C����
        GENERIC_WRITE,              // �A�N�Z�X���[�h
        0,                          // ���L�i�Ȃ��j
        NULL,                       // �Z�L�����e�B�����i�p�����Ȃ��j
        CREATE_ALWAYS,              // �쐬���@
        FILE_ATTRIBUTE_NORMAL,      // �����ƃt���O�i�ݒ�Ȃ��j
        NULL                        // �g�������i�Ȃ��j
    );


    // �t�@�C���ɃX�e�[�W������������
    if (hFile != INVALID_HANDLE_VALUE) 
    {
        for (int x = 0; x < SIZE_X; x++) 
        {
            for (int z = 0; z < SIZE_Z; z++) 
            {
                // �u���b�N�̎�ނƍ������t�@�C���ɏ�������
                char blockInfo[100];
                _snprintf_s(blockInfo, sizeof(blockInfo), "%d %d %d %d\n", x, z, table_[x][z].type_, table_[x][z].height_);
                DWORD bytesWritten;
                WriteFile(hFile, blockInfo, strlen(blockInfo), &bytesWritten, NULL);
            }
        }
        // ����
        CloseHandle(hFile);
    }
#if 0
    /*
    * <�A�N�Z�X���[�h>
    �������݁FGENERIC_WRITE
    �ǂݍ��݁FGENERIC_READ
    <�쐬���@>
    �V�����t�@�C�������i�����̃t�@�C��������Ə㏑���j�FCREATE_ALWAYS
    �t�@�C�����J��    �i�����̃t�@�C�����Ȃ���΃G���[�j�FOPEN_EXISTING
    */

    ////�f�[�^��������
    DWORD dwBytes = 0;      //�������݈ʒu
    WriteFile
    (
        hFile,                      //�t�@�C���n���h��
        "data",                     //�ۑ�����f�[�^�i������j
        (DWORD)strlen("data"),      //�������ޕ�����
        &dwBytes,                   //�������񂾃T�C�Y������ϐ�
        NULL                        //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j
    );

    ////�f�[�^�ǂݍ���

    //�t�@�C���̃T�C�Y���擾
    DWORD fileSize = GetFileSize(hFile, NULL);

    //�t�@�C���̃T�C�Y�����������m��
    char* data;
    data = new char[fileSize];

    DWORD dwBytes = 0; //�ǂݍ��݈ʒu

    ReadFile(
        hFile,     //�t�@�C���n���h��
        data,      //�f�[�^������ϐ�
        fileSize,  //�ǂݍ��ރT�C�Y
        &dwBytes,  //�ǂݍ��񂾃T�C�Y
        NULL);     //�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j

    //�J��
    delete data;
#endif
}

void Stage::Load()
{
    char fileName[MAX_PATH] = "";  // �t�@�C����������ϐ�

    // �u�t�@�C�����J���v�_�C�A���O�̐ݒ�
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter =
        TEXT("�}�b�v�f�[�^(*.map)\0*.map\0")
        TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;   // ���݂���t�@�C�������I�ׂȂ��t���O
    ofn.lpstrDefExt = "map";          // �f�t�H���g�g���q

    // �u�t�@�C�����J���v�_�C�A���O
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    // �L�����Z�������璆�f
    if (selFile == FALSE) return;

    // �t�@�C�����J��
    HANDLE hFile;
    hFile = CreateFile
    (
        fileName,               // �t�@�C����
        GENERIC_READ,           // �ǂݍ��݃A�N�Z�X���[�h
        0,                      // ���L�i�Ȃ��j
        NULL,                   // �Z�L�����e�B�����i�p�����Ȃ��j
        OPEN_EXISTING,          // �t�@�C�����J�����@
        FILE_ATTRIBUTE_NORMAL,  // �����ƃt���O�i�ݒ�Ȃ��j
        NULL                    // �g�������i�Ȃ��j
    );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        // �t�@�C������X�e�[�W����ǂݍ���
        char buffer[100];
        while (ReadLineFromFile(hFile, buffer, sizeof(buffer)))
        {
            int x, z, type, height;
            if (sscanf_s(buffer, "%d %d %d %d", &x, &z, &type, &height) == 4)
            {
                // �ǂݍ��񂾏����X�e�[�W�ɐݒ�
                if (x >= 0 && x < SIZE_X && z >= 0 && z < SIZE_Z)
                {
                    table_[x][z].type_ = static_cast<BOX_TYPE>(type);
                    table_[x][z].height_ = height;
                }
            }
        }

        // �t�@�C�������
        CloseHandle(hFile);
    }
}
#endif

//////�o�C�i���t�@�C���ŃZ�[�u���[�h
#if 1
void Stage::Save()
{
    char fileName[MAX_PATH] = "����.map";  // �t�@�C�������i�[����ϐ�

    FILE* file;
    if (fopen_s(&file, fileName, "wb") != 0)
    {
        // �t�@�C�����J���Ȃ������ꍇ�̃G���[�n���h�����O���s��
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
                // �f�[�^�̏������݂Ɏ��s�����ꍇ�̃G���[�n���h�����O���s��
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
    ofn.Flags = OFN_FILEMUSTEXIST;   // ���݂���t�@�C�������I���ł��Ȃ��t���O
    ofn.lpstrDefExt = "map";          // �f�t�H���g�̊g���q

    // �u�t�@�C�����J���v�_�C�A���O��\��
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    // �L�����Z�������ꍇ�͒��f
    if (selFile == FALSE) return;

    FILE* file;
    if (fopen_s(&file, fileName, "rb") != 0)
    {
        // �t�@�C�����J���Ȃ������ꍇ�̃G���[�n���h�����O���s��
        std::cerr << "�t�@�C�����J���܂���ł����B" << std::endl;
        return;
    }

    // �X�e�[�W�����o�C�i���`���Ńt�@�C������ǂݍ���
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2];
            if (fread(blockInfo, sizeof(int), 2, file) != 2)
            {
                // �f�[�^�̓ǂݍ��݂Ɏ��s�����ꍇ�̃G���[�n���h�����O���s��
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
#endif

bool Stage::ReadLineFromFile(HANDLE hFile, char* buffer, DWORD bufferSize)
{
    if (hFile == INVALID_HANDLE_VALUE || buffer == nullptr || bufferSize == 0)
    {
        return false;
    }

    DWORD bytesRead = 0;
    char ch = 0;
    DWORD totalBytesRead = 0;

    while (totalBytesRead < bufferSize - 1)
    {
        if (ReadFile(hFile, &ch, 1, &bytesRead, nullptr) && bytesRead == 1)
        {
            if (ch == '\n' || ch == '\r')
            {
                // ���s������ǂ񂾂�I��
                break;
            }
            buffer[totalBytesRead++] = ch;
        }
        else
        {
            // �t�@�C���̏I�[�ɒB�����ꍇ��G���[�����������ꍇ�͏I��
            break;
        }
    }

    // �k���I�[
    buffer[totalBytesRead] = '\0';

    return totalBytesRead > 0;
}

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

//////////�J����//////////
#if 0
    // �X�e�[�W�̃u���b�N��ύX����
void ChangeBlock(int x, int z, BOX_TYPE type, int height)
{
    // �ύX�O�̏�Ԃ�ۑ�
    StageChange previousState;
    previousState.x = x;
    previousState.z = z;
    previousState.type = table_[x][z].type_;
    previousState.height = table_[x][z].height_;

    // �ύX��K�p
    table_[x][z].type_ = type;
    table_[x][z].height_ = height;

    // �ύX�����ɒǉ�
    changeHistory.push(previousState);
}

void Stage::Undo()
{
    if (!changeHistory.empty())
    {
        StageChange previousState = changeHistory.top();
        changeHistory.pop();

        // �ύX�����ɖ߂�
        table_[previousState.x][previousState.z].type_ = previousState.type;
        table_[previousState.x][previousState.z].height_ = previousState.height;
    }
}

void Stage::Redo()
{
}

#endif


// �X�e�[�W�̍����}�b�v��Perlin Noise���g�p���Đ���
void Stage::GenerateRandomHeightMap()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            // �p�[�����m�C�Y���獂���𐶐�
            double xCoord = static_cast<double>(x) / SIZE_X;
            double zCoord = static_cast<double>(z) / SIZE_Z;
            double height = perlin.noise(xCoord, zCoord);

            // �X�P�[�����O�Ȃǂ̒���
            height = (height + 1.0) * 0.5; // 0����1�͈̔͂ɃX�P�[��

            // ������ݒ�
            table_[x][z].height_ = static_cast<int>(height * SIZE_Y);
        }
    }
}