#include "DX12Effekseer.h"

#ifndef _DEBUG
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"LLGI.lib")
#pragma comment(lib,"EffekseerRendererDX12.lib")
#else
#pragma comment(lib,"Effekseerd.lib")
#pragma comment(lib,"LLGId.lib")
#pragma comment(lib,"EffekseerRendererDX12d.lib")
#endif


DX12Effekseer::CEffekseer::CEffekseer() :m_manager(nullptr),
m_renderer(nullptr),m_sfMemoryPoolEfk(nullptr), m_commandListEfk(nullptr)
{

}

DX12Effekseer::CEffekseer::~CEffekseer()
{
	Reset();
}

/**
	@brief	Effekseerの初期設定
	@return　true 成功 失敗 false
*/
bool DX12Effekseer::CEffekseer::Initialize()
{
	//エフェクトレンダラーの生成
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_renderer = ::EffekseerRendererDX12::Create(DXTK->Device, DXTK->CommandQueue, 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

	//メモリプールの生成
	m_sfMemoryPoolEfk = EffekseerRenderer::CreateSingleFrameMemoryPool(m_renderer->GetGraphicsDevice());

	//コマンドリストの生成
	m_commandListEfk = EffekseerRenderer::CreateCommandList(m_renderer->GetGraphicsDevice(), m_sfMemoryPoolEfk);

	//エフェクトのマネージャーの生成
	m_manager = Effekseer::Manager::Create(10000);

	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	//描画モジュールの設定
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());
	m_manager->SetModelLoader(m_renderer->CreateModelLoader());
	m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());
	m_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	m_renderer->SetCommandList(m_commandListEfk);

	auto g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	//プロジェクションビュー初期設定
	m_renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH
	(90.0f / 180.0f * 3.14, (float)1280 / 720, 1.0f, 500.0f));

	//カメラの初期設定
	m_renderer->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH
	(g_position, Effekseer::Vector3D(0, 0, 0), ::Effekseer::Vector3D(0, 1, 0)));

	if (m_manager == nullptr || m_renderer == nullptr || m_commandListEfk == nullptr)
		return false;

	return true;
}

/**
	@brief	Effekseerの解放
*/
void DX12Effekseer::CEffekseer::Reset()
{
	m_manager.Reset();
	m_renderer.Reset();
}

/**
	@brief	Effekseerの更新
*/
void DX12Effekseer::CEffekseer::Update()
{
	m_manager->Update();
	m_sfMemoryPoolEfk->NewFrame();
}

/**
	@brief	Effekseerの描画
*/
void DX12Effekseer::CEffekseer::Renderer()
{
	EffekseerRendererDX12::BeginCommandList(m_commandListEfk, DXTK->CommandList);
	m_renderer->BeginRendering();
	m_manager->Draw();
	m_renderer->EndRendering();
	EffekseerRendererDX12::EndCommandList(m_commandListEfk);
}

/**
	@brief	カメラ設定
	@param	cameraPosition カメラのポジション
*/
void DX12Effekseer::CEffekseer::SetCamera(Vector3 cameraPositon)
{
	Effekseer::Vector3D position = Effekseer::Vector3D(cameraPositon.x, cameraPositon.y, cameraPositon.z);

	m_renderer->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH
	(position, Effekseer::Vector3D(0, 0, 0), ::Effekseer::Vector3D(0, 1, 0)));
}

/**
	@brief	エフェクトの再生
	@param	effectName エフェクト名
	@return ハンドル番号
*/
Effekseer::Handle DX12Effekseer::CEffekseer::Play(EFFECT effectName)
{
	EFFECTHANDLE handle = m_manager->Play(effectName, 0, 0, 0);

	return handle;
}

/**
	@brief	エフェクトの停止
	@param	handleName ハンドル番号
*/
void DX12Effekseer::CEffekseer::Stop(EFFECTHANDLE handleName)
{
	m_manager->StopEffect(handleName);
}

/**
	@brief	エフェクトの一時停止　再生
	@param	handleName ハンドル番号
*/
void DX12Effekseer::CEffekseer::Pause(EFFECTHANDLE handleName)
{
	auto flag = m_manager->GetPaused(handleName);
	m_manager->SetPaused(handleName, !flag);
}

/**
	@brief	エフェクトのポジション変更
	@param	handleName		ハンドル番号
	@param	effectPosition	ポジション
*/
void DX12Effekseer::CEffekseer::SetPosition(EFFECTHANDLE handleName,Vector3 effectPosition)
{
	Effekseer::Vector3D position = Effekseer::Vector3D(effectPosition.x, effectPosition.y, effectPosition.z);
	m_manager->SetLocation(handleName, position);

}

/**
	@brief	エフェクトを移動させる
	@param	handleName	ハンドル番号
	@param	position	ポジション
*/
void DX12Effekseer::CEffekseer::MoveEffect(EFFECTHANDLE handleName, Vector3 position)
{
	Effekseer::Vector3D pos = Effekseer::Vector3D(position.x, position.y, position.z);
	m_manager->AddLocation(handleName, pos);
}

/**
	@brief	エフェクトを回転させる
	@param	handleName	ハンドル番号
	@param	rotation	ローテーション
*/
void DX12Effekseer::CEffekseer::SetRotation(EFFECTHANDLE handleName, Vector3 rotation)
{
	m_manager->SetRotation(handleName, rotation.x, rotation.y, rotation.z);
}

/**
	@brief	エフェクトのスケール変更
	@param	handleName	ハンドル番号
	@param	scale		拡大率
*/
void DX12Effekseer::CEffekseer::SetScale(EFFECTHANDLE handleName, Vector3 scale)
{
	m_manager->SetScale(handleName, scale.x, scale.y, scale.z);
}

/**
	@brief	エフェクトを特定の位置まで動かす
	@param	handleName	ハンドル番号
	@param	position	ポジション
*/
void DX12Effekseer::CEffekseer::SetTarget(EFFECTHANDLE handleName, Vector3 position)
{
	Effekseer::Vector3D pos = Effekseer::Vector3D(position.x, position.y, position.z);
	m_manager->SetTargetLocation(handleName, pos);
}

/**
	@brief	エフェクトの再生速度を取得する
	@param	handleName	ハンドル番号
	@return 再生速度
*/
float DX12Effekseer::CEffekseer::GetSpeed(EFFECTHANDLE handleName)
{
	return m_manager->GetSpeed(handleName);
}

/**
	@brief	エフェクトの再生速度を変更する
	@param	handleName	ハンドル番号
	@param	float		再生速度
*/
void DX12Effekseer::CEffekseer::SetSpeed(EFFECTHANDLE handleName , float speed)
{
	m_manager->SetSpeed(handleName, speed);
}

/**
	@brief	エフェクトを生成する
	@param	fileName	ファイル名
	@return	エフェクト
*/
Effekseer::EffectRef DX12Effekseer::CEffekseer::Create(LPCWSTR fileName)
{
	Effekseer::EffectRef effect = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)fileName, 1.0f);

	return effect;
}
