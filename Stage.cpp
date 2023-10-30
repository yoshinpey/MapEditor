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
    // 乱数初期化
    srand((unsigned int)time(nullptr));

    // モデル初期化
    for (int i = 0; i < MODEL_NUM; i++) 
        hModel_[i] = -1;
    
    // 構造体初期化
    ResetStage();

    // パーリンノイズ初期化(シード値も設定可能)
    perlin = PerlinNoise();
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
    // モデルの読み込み
    LoadModels();

    // ステージの高さマップをランダムに生成
    GenerateRandomHeightMap();
}

void Stage::Update()
{
    // マウスボタン押してないときは早期リターンで計算しない
    if (!Input::IsMouseButtonDown(0)) return;
    // ALTを押しているときは計算しない
    if (Input::IsKey(DIK_LALT)) return;

    // スクリーンサイズ
    float w = (float)(Direct3D::scrWidth / 2.0f);
    float h = (float)(Direct3D::scrHeight / 2.0f);

    XMMATRIX vp =
    {
        w,  0,  0,  0,
        0,  -h, 0,  0,
        0,  0,  1,  0,
        w,  h,  0,  1
    };

    // ビューポート
    XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
    // プロジェクション変換
    XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
    // ビュー変換
    XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());

    // マウス座標取得
    XMFLOAT3 mousePosFront = Input::GetMousePosition();
    mousePosFront.z = 0.0f;
    // マウス位置をベクトルに変換
    XMVECTOR vMousePosFront = XMLoadFloat3(&mousePosFront);
    // マウス位置ベクトルにinvVP、InvProj、invViewをかける
    vMousePosFront = XMVector3TransformCoord(vMousePosFront, invVP * invProj * invView);

    // マウス後ろ座標取得
    XMFLOAT3 mousePosBack = Input::GetMousePosition();
    mousePosBack.z = 1.0f;
    XMVECTOR vMousePosBack = XMLoadFloat3(&mousePosBack);
    vMousePosBack = XMVector3TransformCoord(vMousePosBack, invVP * invProj * invView);

    // 判定後のXZ
    int XX = -1, ZZ = -1;
    // 適当な大きい値で初期化
    float minDist = 9999999;

    // ステージサイズ
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            for (int y = 0; y < table_[x][z].height_+1; y++)
            {
                // マウス位置前ベクトルからマウス位置後ろベクトルにレイを打つ
                RayCastData data{};
                XMStoreFloat4(&data.start, vMousePosFront);
                XMStoreFloat4(&data.dir, vMousePosBack - vMousePosFront);

                Transform blockTrans;
                blockTrans.position_ = {(float)x,(float)y,(float)z};

                Model::SetTransform(hModel_[DEFAULT], blockTrans);
                Model::RayCast(hModel_[DEFAULT],data);

                // レイが当たったとき
                if (data.hit)
                {
                    // 初回ヒット
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

    // レイが当たった後
    if (XX >= 0)
    {
        // 編集モード変更
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
    // ファイルパス
    std::string failBase = "Assets/";
    std::string modelName[] =
    {
        "BoxDefault.fbx",
        "BoxBrick.fbx",
        "BoxGrass.fbx",
        "BoxSand.fbx",
        "BoxWater.fbx",
    };

    // モデルデータのロード
    for (int i = 0; i < TYPEMAX; i++)
    {
        hModel_[i] = Model::Load(failBase + modelName[i]);
        assert(hModel_[i] >= 0);
    }

    // ブロックタイプ初期化
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }
}

// ダイアログ
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"デフォルト");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"レンガ");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"草原");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"砂地");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 0, (LPARAM)"水");
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

////////テキストファイルでセーブロード
#if 0
void Stage::Save()
{
    char fileName[MAX_PATH] = "無題.map";  //ファイル名を入れる変数

    //「ファイルを保存」ダイアログの設定
    OPENFILENAME ofn;                         	    //名前をつけて保存ダイアログの設定用構造体
    ZeroMemory(&ofn, sizeof(ofn));            	    //構造体初期化
    ofn.lStructSize = sizeof(OPENFILENAME);   	    //構造体のサイズ
    ofn.lpstrFilter =
        TEXT("マップデータ(*.map)\0*.map\0")        //ファイルの種類
        TEXT("すべてのファイル(*.*)\0*.*\0\0");     //ファイルの種類
    ofn.lpstrFile = fileName;               	    //ファイル名
    ofn.nMaxFile = MAX_PATH;               	        //パスの最大文字数
    ofn.Flags = OFN_OVERWRITEPROMPT;   		        //フラグ
    ofn.lpstrDefExt = "map";                  	    //デフォルト拡張子

    /*
    <フラグ>
    上書き保存するか確認する：OFN_OVERWRITEPROMPT(保存の時はコレ)
    存在するファイルしか選べない：OFN_FILEMUSTEXIST(開く時はコレ)
    */

    //「ファイルを保存」ダイアログ
    BOOL selFile;
    selFile = GetSaveFileName(&ofn);

    //キャンセルしたら中断
    if (selFile == FALSE) return;

    ////ファイル作成､開く
    HANDLE hFile;
    hFile = CreateFile
    (
        fileName,                   // ファイル名
        GENERIC_WRITE,              // アクセスモード
        0,                          // 共有（なし）
        NULL,                       // セキュリティ属性（継承しない）
        CREATE_ALWAYS,              // 作成方法
        FILE_ATTRIBUTE_NORMAL,      // 属性とフラグ（設定なし）
        NULL                        // 拡張属性（なし）
    );


    // ファイルにステージ情報を書き込む
    if (hFile != INVALID_HANDLE_VALUE) 
    {
        for (int x = 0; x < SIZE_X; x++) 
        {
            for (int z = 0; z < SIZE_Z; z++) 
            {
                // ブロックの種類と高さをファイルに書き込む
                char blockInfo[100];
                _snprintf_s(blockInfo, sizeof(blockInfo), "%d %d %d %d\n", x, z, table_[x][z].type_, table_[x][z].height_);
                DWORD bytesWritten;
                WriteFile(hFile, blockInfo, strlen(blockInfo), &bytesWritten, NULL);
            }
        }
        // 閉じる
        CloseHandle(hFile);
    }
#if 0
    /*
    * <アクセスモード>
    書き込み：GENERIC_WRITE
    読み込み：GENERIC_READ
    <作成方法>
    新しくファイルを作る（同名のファイルがあると上書き）：CREATE_ALWAYS
    ファイルを開く    （同名のファイルがなければエラー）：OPEN_EXISTING
    */

    ////データ書き込み
    DWORD dwBytes = 0;      //書き込み位置
    WriteFile
    (
        hFile,                      //ファイルハンドル
        "data",                     //保存するデータ（文字列）
        (DWORD)strlen("data"),      //書き込む文字数
        &dwBytes,                   //書き込んだサイズを入れる変数
        NULL                        //オーバーラップド構造体（今回は使わない）
    );

    ////データ読み込み

    //ファイルのサイズを取得
    DWORD fileSize = GetFileSize(hFile, NULL);

    //ファイルのサイズ分メモリを確保
    char* data;
    data = new char[fileSize];

    DWORD dwBytes = 0; //読み込み位置

    ReadFile(
        hFile,     //ファイルハンドル
        data,      //データを入れる変数
        fileSize,  //読み込むサイズ
        &dwBytes,  //読み込んだサイズ
        NULL);     //オーバーラップド構造体（今回は使わない）

    //開放
    delete data;
#endif
}

void Stage::Load()
{
    char fileName[MAX_PATH] = "";  // ファイル名を入れる変数

    // 「ファイルを開く」ダイアログの設定
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter =
        TEXT("マップデータ(*.map)\0*.map\0")
        TEXT("すべてのファイル(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;   // 存在するファイルしか選べないフラグ
    ofn.lpstrDefExt = "map";          // デフォルト拡張子

    // 「ファイルを開く」ダイアログ
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    // キャンセルしたら中断
    if (selFile == FALSE) return;

    // ファイルを開く
    HANDLE hFile;
    hFile = CreateFile
    (
        fileName,               // ファイル名
        GENERIC_READ,           // 読み込みアクセスモード
        0,                      // 共有（なし）
        NULL,                   // セキュリティ属性（継承しない）
        OPEN_EXISTING,          // ファイルを開く方法
        FILE_ATTRIBUTE_NORMAL,  // 属性とフラグ（設定なし）
        NULL                    // 拡張属性（なし）
    );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        // ファイルからステージ情報を読み込む
        char buffer[100];
        while (ReadLineFromFile(hFile, buffer, sizeof(buffer)))
        {
            int x, z, type, height;
            if (sscanf_s(buffer, "%d %d %d %d", &x, &z, &type, &height) == 4)
            {
                // 読み込んだ情報をステージに設定
                if (x >= 0 && x < SIZE_X && z >= 0 && z < SIZE_Z)
                {
                    table_[x][z].type_ = static_cast<BOX_TYPE>(type);
                    table_[x][z].height_ = height;
                }
            }
        }

        // ファイルを閉じる
        CloseHandle(hFile);
    }
}
#endif

//////バイナリファイルでセーブロード
#if 1
void Stage::Save()
{
    char fileName[MAX_PATH] = "無題.map";  // ファイル名を格納する変数

    FILE* file;
    if (fopen_s(&file, fileName, "wb") != 0)
    {
        // ファイルを開けなかった場合のエラーハンドリングを行う
        std::cerr << "ファイルを開けませんでした。" << std::endl;
        return;
    }

    // ステージ情報をバイナリ形式でファイルに書き込む
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2] = { table_[x][z].type_, table_[x][z].height_ };
            if (fwrite(blockInfo, sizeof(int), 2, file) != 2)
            {
                // データの書き込みに失敗した場合のエラーハンドリングを行う
                std::cerr << "ファイルへの書き込みに失敗しました。" << std::endl;
                fclose(file);
                return;
            }
        }
    }

    // ファイルを閉じる
    fclose(file);
}

void Stage::Load()
{
    char fileName[MAX_PATH] = "";  // ファイル名を格納する変数

    // 「ファイルを開く」ダイアログの設定
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0")
        TEXT("すべてのファイル(*.*)\0*.*\0\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;   // 存在するファイルしか選択できないフラグ
    ofn.lpstrDefExt = "map";          // デフォルトの拡張子

    // 「ファイルを開く」ダイアログを表示
    BOOL selFile;
    selFile = GetOpenFileName(&ofn);

    // キャンセルした場合は中断
    if (selFile == FALSE) return;

    FILE* file;
    if (fopen_s(&file, fileName, "rb") != 0)
    {
        // ファイルを開けなかった場合のエラーハンドリングを行う
        std::cerr << "ファイルを開けませんでした。" << std::endl;
        return;
    }

    // ステージ情報をバイナリ形式でファイルから読み込む
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            int blockInfo[2];
            if (fread(blockInfo, sizeof(int), 2, file) != 2)
            {
                // データの読み込みに失敗した場合のエラーハンドリングを行う
                std::cerr << "ファイルからの読み込みに失敗しました。" << std::endl;
                fclose(file);
                return;
            }
            table_[x][z].type_ = static_cast<BOX_TYPE>(blockInfo[0]);
            table_[x][z].height_ = blockInfo[1];
        }
    }

    // ファイルを閉じる
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
                // 改行文字を読んだら終了
                break;
            }
            buffer[totalBytesRead++] = ch;
        }
        else
        {
            // ファイルの終端に達した場合やエラーが発生した場合は終了
            break;
        }
    }

    // ヌル終端
    buffer[totalBytesRead] = '\0';

    return totalBytesRead > 0;
}

void Stage::ResetStage()
{
    // 構造体初期化
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            SetBlockType(x, z, DEFAULT);
            SetBlockHeight(x, z, SIZE_Y);
        }
    }
}

//////////開発中//////////
#if 0
    // ステージのブロックを変更する
void ChangeBlock(int x, int z, BOX_TYPE type, int height)
{
    // 変更前の状態を保存
    StageChange previousState;
    previousState.x = x;
    previousState.z = z;
    previousState.type = table_[x][z].type_;
    previousState.height = table_[x][z].height_;

    // 変更を適用
    table_[x][z].type_ = type;
    table_[x][z].height_ = height;

    // 変更履歴に追加
    changeHistory.push(previousState);
}

void Stage::Undo()
{
    if (!changeHistory.empty())
    {
        StageChange previousState = changeHistory.top();
        changeHistory.pop();

        // 変更を元に戻す
        table_[previousState.x][previousState.z].type_ = previousState.type;
        table_[previousState.x][previousState.z].height_ = previousState.height;
    }
}

void Stage::Redo()
{
}

#endif


// ステージの高さマップをPerlin Noiseを使用して生成
void Stage::GenerateRandomHeightMap()
{
    for (int x = 0; x < SIZE_X; x++)
    {
        for (int z = 0; z < SIZE_Z; z++)
        {
            // パーリンノイズから高さを生成
            double xCoord = static_cast<double>(x) / SIZE_X;
            double zCoord = static_cast<double>(z) / SIZE_Z;
            double height = perlin.noise(xCoord, zCoord);

            // スケーリングなどの調整
            height = (height + 1.0) * 0.5; // 0から1の範囲にスケール

            // 高さを設定
            table_[x][z].height_ = static_cast<int>(height * SIZE_Y);
        }
    }
}