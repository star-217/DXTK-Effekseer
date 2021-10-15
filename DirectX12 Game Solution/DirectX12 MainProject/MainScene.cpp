//
// MainScene.cpp
//

#include "Base/pch.h"
#include "Base/dxtk.h"
#include "SceneFactory.h"

// Initialize member variables.
MainScene::MainScene()
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{

}

// Allocate all memory the Direct3D and Direct2D resources.
void MainScene::LoadAssets()
{
	DX12Effect.Initialize();
	as = DX12Effect.Create(L"laser.efk");
}

// Releasing resources required for termination.
void MainScene::Terminate()
{
	DXTK->ResetAudioEngine();
	DXTK->WaitForGpu();


	// TODO: Add your Termination logic here.

}

// Direct3D resource cleanup.
void MainScene::OnDeviceLost()
{
	DX12Effect.Reset();
}

// Restart any looped sounds here
void MainScene::OnRestartSound()
{

}

// Updates the scene.
NextScene MainScene::Update(const float deltaTime)
{
	// If you use 'deltaTime', remove it.
	UNREFERENCED_PARAMETER(deltaTime);

	// TODO: Add your game logic here.
	if (DXTK->KeyEvent->pressed.Space)
	{
		handle = DX12Effect.Play(as);
		int a = 0;
	}
	DX12Effect.Update();




	return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
	// TODO: Add your rendering code here.
	DXTK->ResetCommand();
	DXTK->ClearRenderTarget(Colors::Black);
	DX12Effect.Renderer();

	DXTK->ExecuteCommandList();
}
