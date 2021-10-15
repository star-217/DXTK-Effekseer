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
	@brief	Effekseer�̏����ݒ�
	@return�@true ���� ���s false
*/
bool DX12Effekseer::CEffekseer::Initialize()
{
	//�G�t�F�N�g�����_���[�̐���
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_renderer = ::EffekseerRendererDX12::Create(DXTK->Device, DXTK->CommandQueue, 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

	//�������v�[���̐���
	m_sfMemoryPoolEfk = EffekseerRenderer::CreateSingleFrameMemoryPool(m_renderer->GetGraphicsDevice());

	//�R�}���h���X�g�̐���
	m_commandListEfk = EffekseerRenderer::CreateCommandList(m_renderer->GetGraphicsDevice(), m_sfMemoryPoolEfk);

	//�G�t�F�N�g�̃}�l�[�W���[�̐���
	m_manager = Effekseer::Manager::Create(10000);

	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	//�`�惂�W���[���̐ݒ�
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

	// �e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ肷��B
	// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());
	m_manager->SetModelLoader(m_renderer->CreateModelLoader());
	m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());
	m_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	m_renderer->SetCommandList(m_commandListEfk);

	auto g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	//�v���W�F�N�V�����r���[�����ݒ�
	m_renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH
	(90.0f / 180.0f * 3.14, (float)1280 / 720, 1.0f, 500.0f));

	//�J�����̏����ݒ�
	m_renderer->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH
	(g_position, Effekseer::Vector3D(0, 0, 0), ::Effekseer::Vector3D(0, 1, 0)));

	if (m_manager == nullptr || m_renderer == nullptr || m_commandListEfk == nullptr)
		return false;

	return true;
}

/**
	@brief	Effekseer�̉��
*/
void DX12Effekseer::CEffekseer::Reset()
{
	m_manager.Reset();
	m_renderer.Reset();
}

/**
	@brief	Effekseer�̍X�V
*/
void DX12Effekseer::CEffekseer::Update()
{
	m_manager->Update();
	m_sfMemoryPoolEfk->NewFrame();
}

/**
	@brief	Effekseer�̕`��
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
	@brief	�J�����ݒ�
	@param	cameraPosition �J�����̃|�W�V����
*/
void DX12Effekseer::CEffekseer::SetCamera(Vector3 cameraPositon)
{
	Effekseer::Vector3D position = Effekseer::Vector3D(cameraPositon.x, cameraPositon.y, cameraPositon.z);

	m_renderer->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH
	(position, Effekseer::Vector3D(0, 0, 0), ::Effekseer::Vector3D(0, 1, 0)));
}

/**
	@brief	�G�t�F�N�g�̍Đ�
	@param	effectName �G�t�F�N�g��
	@return �n���h���ԍ�
*/
Effekseer::Handle DX12Effekseer::CEffekseer::Play(EFFECT effectName)
{
	EFFECTHANDLE handle = m_manager->Play(effectName, 0, 0, 0);

	return handle;
}

/**
	@brief	�G�t�F�N�g�̒�~
	@param	handleName �n���h���ԍ�
*/
void DX12Effekseer::CEffekseer::Stop(EFFECTHANDLE handleName)
{
	m_manager->StopEffect(handleName);
}

/**
	@brief	�G�t�F�N�g�̈ꎞ��~�@�Đ�
	@param	handleName �n���h���ԍ�
*/
void DX12Effekseer::CEffekseer::Pause(EFFECTHANDLE handleName)
{
	auto flag = m_manager->GetPaused(handleName);
	m_manager->SetPaused(handleName, !flag);
}

/**
	@brief	�G�t�F�N�g�̃|�W�V�����ύX
	@param	handleName		�n���h���ԍ�
	@param	effectPosition	�|�W�V����
*/
void DX12Effekseer::CEffekseer::SetPosition(EFFECTHANDLE handleName,Vector3 effectPosition)
{
	Effekseer::Vector3D position = Effekseer::Vector3D(effectPosition.x, effectPosition.y, effectPosition.z);
	m_manager->SetLocation(handleName, position);

}

/**
	@brief	�G�t�F�N�g���ړ�������
	@param	handleName	�n���h���ԍ�
	@param	position	�|�W�V����
*/
void DX12Effekseer::CEffekseer::MoveEffect(EFFECTHANDLE handleName, Vector3 position)
{
	Effekseer::Vector3D pos = Effekseer::Vector3D(position.x, position.y, position.z);
	m_manager->AddLocation(handleName, pos);
}

/**
	@brief	�G�t�F�N�g����]������
	@param	handleName	�n���h���ԍ�
	@param	rotation	���[�e�[�V����
*/
void DX12Effekseer::CEffekseer::SetRotation(EFFECTHANDLE handleName, Vector3 rotation)
{
	m_manager->SetRotation(handleName, rotation.x, rotation.y, rotation.z);
}

/**
	@brief	�G�t�F�N�g�̃X�P�[���ύX
	@param	handleName	�n���h���ԍ�
	@param	scale		�g�嗦
*/
void DX12Effekseer::CEffekseer::SetScale(EFFECTHANDLE handleName, Vector3 scale)
{
	m_manager->SetScale(handleName, scale.x, scale.y, scale.z);
}

/**
	@brief	�G�t�F�N�g�����̈ʒu�܂œ�����
	@param	handleName	�n���h���ԍ�
	@param	position	�|�W�V����
*/
void DX12Effekseer::CEffekseer::SetTarget(EFFECTHANDLE handleName, Vector3 position)
{
	Effekseer::Vector3D pos = Effekseer::Vector3D(position.x, position.y, position.z);
	m_manager->SetTargetLocation(handleName, pos);
}

/**
	@brief	�G�t�F�N�g�̍Đ����x���擾����
	@param	handleName	�n���h���ԍ�
	@return �Đ����x
*/
float DX12Effekseer::CEffekseer::GetSpeed(EFFECTHANDLE handleName)
{
	return m_manager->GetSpeed(handleName);
}

/**
	@brief	�G�t�F�N�g�̍Đ����x��ύX����
	@param	handleName	�n���h���ԍ�
	@param	float		�Đ����x
*/
void DX12Effekseer::CEffekseer::SetSpeed(EFFECTHANDLE handleName , float speed)
{
	m_manager->SetSpeed(handleName, speed);
}

/**
	@brief	�G�t�F�N�g�𐶐�����
	@param	fileName	�t�@�C����
	@return	�G�t�F�N�g
*/
Effekseer::EffectRef DX12Effekseer::CEffekseer::Create(LPCWSTR fileName)
{
	Effekseer::EffectRef effect = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)fileName, 1.0f);

	return effect;
}
