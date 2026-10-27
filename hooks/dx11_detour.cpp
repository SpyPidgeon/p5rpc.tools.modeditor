#include "dx11_detour.h"
#include "imguistyling.h"
#include "quickpanel.h"

extern HMODULE dll_handle;

HWND gameWindow = NULL;

typedef long(__stdcall* present)(IDXGISwapChain*, UINT, UINT);
present p_present;
present p_present_target;
bool get_present_pointer()
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = gameWindow;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGISwapChain* swap_chain;
	ID3D11Device* device;

	const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		feature_levels,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		nullptr,
		nullptr);
	if (result == S_OK)
	{
		void** p_vtable = *reinterpret_cast<void***>(swap_chain);
		swap_chain->Release();
		device->Release();
		//context->Release();
		p_present_target = (present)p_vtable[8];
		return true;
	}
	else
	{
		printf("[P5R Mod Editor] Failed to hook DX11 with reason: 0x%x\n", result);
	}
	return false;
}

bool init = false;
bool fetchContext = true;
HWND window = NULL;
ID3D11Device* p_device = NULL;
ID3D11DeviceContext* p_context = NULL;
ID3D11RenderTargetView* mainRenderTargetView = NULL;

WNDPROC oWndProc;
ImFont* windowFont;
bool quit = false;
bool lockControls = false;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	auto context = ImGui::GetCurrentContext();
	ImGuiIO* io = nullptr;

	if (context != nullptr)
	{
		io = &ImGui::GetIO();
		lockControls = io->WantTextInput;
	}
	else
		lockControls = false;

	switch (uMsg)
	{
	case WM_QUIT:
		printf("[P5R Mod Editor] Mod exited\n");

		if (context != nullptr)
			io->Fonts->RemoveFont(windowFont);

		ImGui::DestroyContext();
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
		break;
	case WM_KILLFOCUS:
	case WM_SIZE:
	case WM_SIZING:
		if (context != nullptr)
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		if (mainRenderTargetView != nullptr) mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
		fetchContext = false;
		init = false;
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

	case WM_KEYDOWN:
		if ((lParam & (1 << 30)))
		{
			break;
		}

		if (wParam == VK_F1)
			ToggleRender();

		if (quickPanel->noclip.enabled)
		{
			quickPanel->noclip.NoclipKeyStateOn(wParam);
		}

		break;

	case WM_KEYUP:

		if (quickPanel->noclip.enabled)
		{
			quickPanel->noclip.NoclipKeyStateOff(wParam);
		}

		break;
	}

	if (fetchContext && context != nullptr)
	{
		if (SUCCEEDED(ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)))
			return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

int GetWindowHeight(HWND window)
{
	RECT rect;

	if (GetWindowRect(window, &rect))
	{
		return rect.bottom - rect.top;
	}

	return 0;
}

static long __stdcall detour_present(IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags) {

	if (!fetchContext)
		return p_present(p_swap_chain, sync_interval, flags);

	if (!init) {
		if (SUCCEEDED(p_swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&p_device)))
		{
			p_device->GetImmediateContext(&p_context);

			if (p_context == nullptr) p_present(p_swap_chain, sync_interval, flags);

			DXGI_SWAP_CHAIN_DESC sd;
			p_swap_chain->GetDesc(&sd);

			if (sd.OutputWindow != gameWindow)
				return  p_present(p_swap_chain, sync_interval, flags);

			window = gameWindow;

			ID3D11Texture2D* pBackBuffer;
			p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

			if (pBackBuffer == nullptr) return p_present(p_swap_chain, sync_interval, flags);

			p_device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();

			if (oWndProc == nullptr) oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			ImGui::CreateContext();

			SetImGuiStyle();

			if (ImGui::GetCurrentContext() == nullptr) return p_present(p_swap_chain, sync_interval, flags);

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			io.MouseDrawCursor = true;
			int h = GetWindowHeight(window);
			windowFont = io.Fonts->AddFontFromFileTTF(GetDLLPath("font\\arial.ttf").c_str(), h * 0.013f);
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(p_device, p_context);
			init = true;
		}
		else
			return p_present(p_swap_chain, sync_interval, flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	RenderStructWidgets();

	ImGui::EndFrame();
	ImGui::Render();

	p_context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return p_present(p_swap_chain, sync_interval, flags);
}

bool DX11Hook()
{
	if (!get_present_pointer())
		return false;

	p_present = p_present_target;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)p_present, detour_present);
	DetourTransactionCommit();

	return true;
}

void ToggleRender()
{
	fetchContext = !fetchContext;
}