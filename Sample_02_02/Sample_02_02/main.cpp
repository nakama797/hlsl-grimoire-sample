#include "stdafx.h"
#include "system/system.h"
#define _USE_MATH_DEFINES
#include <math.h>


// 頂点構造体
struct SimpleVertex
{
    float pos[3];       // 頂点座標
    float color[3];     // 頂点カラー
};

// 関数宣言
void InitRootSignature(RootSignature& rs);
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps);

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // ゲームの初期化
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    // ここから初期化を行うコードを記述する
    //////////////////////////////////////

    // 1. ルートシグネチャを作成
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    // 2. シェーダーをロード
    Shader vs, ps;
    vs.LoadVS("Assets/shader/sample.fx", "VSMain");
    ps.LoadPS("Assets/shader/sample.fx", "PSMain");

    // 3. パイプラインステートを作成
    PipelineState pipelineState;
    InitPipelineState(pipelineState, rootSignature, vs, ps);

    //// 4. 三角形の頂点バッファを作成
    //// 頂点配列を定義
    const  int POLYGONAL_NUMBER = 17;	//　多角形の頂点数
    const float POLYGON_RADIUS = 0.75f;	// 外接円の半径
    SimpleVertex vertices[POLYGONAL_NUMBER];
    for (int i = 0; i < POLYGONAL_NUMBER; i++) {
        vertices[i].pos[0] = cos(2 * M_PI * i / POLYGONAL_NUMBER) * (float)POLYGON_RADIUS * 9.0f / 16.0f;
        vertices[i].pos[1] = sin(2 * M_PI * i / POLYGONAL_NUMBER) * POLYGON_RADIUS;
        vertices[i].pos[2] = 0;
        vertices[i].color[0] = cos(2 * M_PI * i / POLYGONAL_NUMBER);
        vertices[i].color[1] = sin(2 * M_PI * i / POLYGONAL_NUMBER);
        vertices[i].color[2] = (float)i / POLYGONAL_NUMBER;
        //vertices[i].color[3] = 1.0f;
    }


    //SimpleVertex vertices[] = {
    //    {
    //        {-0.5f, -0.5f, 0.0f},
    //        { 1.0f, 0.0f, 0.0f }
    //    },
    //    {
    //        { 0.0f, 0.5f, 0.0f },
    //        { 0.0f, 1.0f, 0.0f }
    //    },
    //    {
    //        { 0.5f, -0.5f, 0.0f },
    //        { 0.0f, 0.0f, 1.0f }
    //    }
    //};

    VertexBuffer triangleVB;
    triangleVB.Init(sizeof(vertices), sizeof(vertices[0]));
    triangleVB.Copy(vertices);

    // 5. 三角形のインデックスバッファを作成
    //インデックス配列
    uint16_t indices[(POLYGONAL_NUMBER - 2) * 3];
    for (int i = 0; i < _countof(indices); i += 3) {
        indices[i] = 0;
        indices[i + 1] = (i / 3) + 1;
        indices[i + 2] = (i / 3) + 2;
    }

    IndexBuffer triangleIB;
    triangleIB.Init(sizeof(indices), 2);
    triangleIB.Copy(indices);

    //////////////////////////////////////
    // 初期化を行うコードを書くのはここまで！！！
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ここからゲームループ
    while (DispatchWindowMessage())
    {
        // レンダリング開始
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ここから絵を描くコードを記述する
        //////////////////////////////////////

        // 1. ルートシグネチャを設定
        renderContext.SetRootSignature(rootSignature);
        // 2. パイプラインステートを設定
        renderContext.SetPipelineState(pipelineState);
        // 3. プリミティブのトポロジーを設定
        renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // 4. 頂点バッファを設定
        renderContext.SetVertexBuffer(triangleVB);
        // 5. インデックスバッファを設定
        renderContext.SetIndexBuffer(triangleIB);
        // 6. ドローコール
        renderContext.DrawIndexed(57);

        /// //////////////////////////////////////
        // 絵を描くコードを書くのはここまで！！！
        //////////////////////////////////////
        // レンダリング終了
        g_engine->EndFrame();
    }
    return 0;
}

// ルートシグネチャの初期化
void InitRootSignature( RootSignature& rs )
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}

// パイプラインステートの初期化
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps)
{
    // 頂点レイアウトを定義する
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // パイプラインステートを作成
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rs.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs.GetCompiledBlob());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps.GetCompiledBlob());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineState.Init(psoDesc);
}
