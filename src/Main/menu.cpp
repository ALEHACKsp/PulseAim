/**
*
*                      PulseAim
*			 github.com/ytmcgamer/PulseAim
*                 made by YTMcGamer#1337
*
*/
#include "../Incl.h"
#include "../imgui/imgui.cpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../Header/util.h"
#include "../Header/offsets.h"
#include "../Header/Structs.h"
#include "../Header/core.h"
#include "../Header/Config.h"

using namespace std;

int aimkey;
int airstuckkey;
bool wkekj = true;

ID3D11Device* device = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

HRESULT(*PresentOriginal)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
HRESULT(*ResizeOriginal)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;
WNDPROC oWndProc;

static const void* game_rbx_jmp;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool ShowMenu = false;

void ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = 16.0f;
	float width = height * 1.60f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())

		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.20f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.0f, 0.0f, 0.80f, 1.0f), ImVec4(0.0f, 0.0f, 0.80f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.0f, 0.0f, 1.0f), ImVec4(0.0f, 0.85f, 0.0f, 1.0f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius + 0.80f, IM_COL32(255, 255, 255, 255));
}
void mousemove(float tarx, float tary, float X, float Y, int smooth)
{
	float ScreenCenterX = (X / 2);
	float ScreenCenterY = (Y / 2);
	float TargetX = 0;
	float TargetY = 0;

	if (tarx != 0)
	{
		if (tarx > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - tarx);
			TargetX /= smooth;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (tarx < ScreenCenterX)
		{
			TargetX = tarx - ScreenCenterX;
			TargetX /= smooth;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (tary != 0)
	{
		if (tary > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - tary);
			TargetY /= smooth;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (tary < ScreenCenterY)
		{
			TargetY = tary - ScreenCenterY;
			TargetY /= smooth;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), 0, 0);
}

VOID AddMarker(ImGuiWindow& window, float width, float height, float* start, PVOID pawn, LPCSTR text, ImU32 color) {
	auto root = Util::GetPawnRootLocation(pawn);
	if (root) {
		auto pos = *root;
		float dx = start[0] - pos.X;
		float dy = start[1] - pos.Y;
		float dz = start[2] - pos.Z;

		if (Util::WorldToScreen(width, height, &pos.X)) {
			float dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 100.0f;

			CHAR modified[0xFF] = { 0 };
			snprintf(modified, sizeof(modified), ("%s\ \n %dm "), text, static_cast<INT>(dist));

			auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, 0, modified);
			window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f, pos.Y - size.y / 2.0f), color, modified);
		}
	}
}

FLOAT GetDistance(ImGuiWindow& window, float width, float height, float* start, PVOID pawn) {
	auto root = Util::GetPawnRootLocation(pawn);
	float dist;
	if (root) {
		auto pos = *root;
		float dx = start[0] - pos.X;
		float dy = start[1] - pos.Y;
		float dz = start[2] - pos.Z;

		if (Util::WorldToScreen(width, height, &pos.X)) {
			dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 1000.0f;
			return dist;
		}
	}
}

__declspec(dllexport) LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_KEYUP && (wParam == VK_INSERT || (ShowMenu && wParam == VK_ESCAPE))) {
		ShowMenu = !ShowMenu;
		ImGui::GetIO().MouseDrawCursor = ShowMenu;
	}
	else if (msg == WM_QUIT && ShowMenu) {
		ExitProcess(0);
	}

	if (ShowMenu) {
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

extern uint64_t base_address = 0;
DWORD processID;
const ImVec4 color = { 255.0,255.0,255.0,1 };
const ImVec4 red = { 0.65,0,0,1 };
const ImVec4 white = { 255.0,255.0,255.0,1 };
const ImVec4 green = { 0.03,0.81,0.14,1 };
const ImVec4 blue = { 0.21660784313,0.56470588235,0.90980392156,1.0 };

ImGuiWindow& BeginScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}
std::string TextFormat(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	char buffer[2048];
	vsprintf(buffer, format, argptr);

	va_end(argptr);

	return buffer;
}

void Active()
{
	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->Colors[ImGuiCol_Button] = ImColor(25, 30, 34);
	Style->Colors[ImGuiCol_ButtonActive] = ImColor(25, 30, 34);
	Style->Colors[ImGuiCol_ButtonHovered] = ImColor(25, 30, 34);
}
void Hovered()
{
	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->Colors[ImGuiCol_Button] = ImColor(19, 22, 27);
	Style->Colors[ImGuiCol_ButtonActive] = ImColor(19, 22, 27);
	Style->Colors[ImGuiCol_ButtonHovered] = ImColor(19, 22, 27);
}

VOID EndScene(ImGuiWindow& window) {
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.17f, 0.18f, 0.2f, 1.0f));

	if (ShowMenu) {
		ImGuiStyle* Style = &ImGui::GetStyle();
		Style->WindowRounding = 0;
		Style->WindowBorderSize = 0;
		Style->ChildRounding = 0;
		Style->FrameBorderSize = 0;
		Style->Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 0);
		Style->Colors[ImGuiCol_ChildBg] = ImColor(19, 22, 27);
		Style->Colors[ImGuiCol_Button] = ImColor(25, 30, 34);
		Style->Colors[ImGuiCol_ButtonHovered] = ImColor(25, 30, 34);
		Style->Colors[ImGuiCol_ButtonActive] = ImColor(25, 30, 34);
		static auto Name = "Project-X";
		static auto Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
		static int Active_Tab = 1;
		static int Active_SubTabRage = 1;
		static int Active_SubTabVisuals = 1;
		static int Active_SubTabMisc = 1;
		static int Active_SubTabSkins = 1;
		static int Active_SubTabConfigs = 1;
		if (ImGui::Begin(Name, NULL, Flags))
		{
			ImGui::BeginChild("##Main", ImVec2{ 640, 430 }, false);
			{
				ImGui::BeginChild("##UP", ImVec2{ 640, 45 }, false);
				{
					ImGui::SetCursorPos(ImVec2{ 240, 0 });
					if (Active_Tab == 1) Active(); else Hovered();
					if (ImGui::Button("Aimbot", ImVec2{ 75, 45 }))
						Active_Tab = 1;
					ImGui::SetCursorPos(ImVec2{ 320, 0 });
					if (Active_Tab == 2) Active(); else Hovered();
					if (ImGui::Button("Visuals", ImVec2{ 75, 45 }))
						Active_Tab = 2;
					ImGui::SetCursorPos(ImVec2{ 400, 0 });
					if (Active_Tab == 3) Active(); else Hovered();
					if (ImGui::Button("Misc", ImVec2{ 75, 45 }))
						Active_Tab = 3;
				}
				ImGui::EndChild();
				ImGui::SetCursorPos(ImVec2{ 0, 45 });
				Style->Colors[ImGuiCol_ChildBg] = ImColor(25, 30, 34);
				Style->Colors[ImGuiCol_Button] = ImColor(25, 30, 34);
				Style->Colors[ImGuiCol_ButtonHovered] = ImColor(25, 30, 34);
				Style->Colors[ImGuiCol_ButtonActive] = ImColor(25, 30, 34);
				ImGui::BeginChild("##Childs", ImVec2{ 640, 365 }, false);
				{
					ImGui::SetCursorPos(ImVec2{ 15, 5 });
					Style->ChildRounding = 0;
					ImGui::BeginChild("##Left", ImVec2{ 155, 320 }, false);
					{
						if (Active_Tab == 1)
						{
							ImGui::SetCursorPosY(10);
							if (ImGui::Button("Main                              ", ImVec2{ 140, 20 })) Active_SubTabRage = 1;
							if (ImGui::Button("Config                             ", ImVec2{ 140, 20 })) Active_SubTabRage = 2;
						}
						else if (Active_Tab == 2)
						{
							ImGui::SetCursorPosY(10);
							if (ImGui::Button("Player                               ", ImVec2{ 140, 20 })) Active_SubTabVisuals = 1;
							if (ImGui::Button("World                              ", ImVec2{ 140, 20 })) Active_SubTabVisuals = 2;
						}
					}
					ImGui::EndChild();

					ImGui::SetCursorPos(ImVec2{ 175, 5 });
					Style->Colors[ImGuiCol_ChildBg] = ImColor(29, 34, 38);
					Style->ChildRounding = 5;
					ImGui::BeginChild("##SubMain", ImVec2{ 455, 350 }, false);
					{
						ImGui::SetCursorPos(ImVec2{ 10, 10 });
						if (Active_Tab == 1 && Active_SubTabRage == 1)
						{
						}
						else if (Active_Tab == 1 && Active_SubTabRage == 2)
						{
						}
						else if (Active_Tab == 2 && Active_SubTabVisuals == 1)
						{
						}
						else if (Active_Tab == 2 && Active_SubTabVisuals == 2)
						{
						}
					}
					ImGui::EndChild();
				}
				ImGui::EndChild();
				ImGui::SetCursorPos(ImVec2{ 0, 410 });
				Style->Colors[ImGuiCol_ChildBg] = ImColor(45, 50, 54);
				Style->ChildRounding = 0;
				ImGui::BeginChild("##Text", ImVec2{ 640, 20 }, false);
				{
					ImGui::SetCursorPos(ImVec2{ 505, 3 });
					ImGui::Text("Project-X");
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}
}
VOID AddLine(ImGuiWindow& window, float width, float height, float a[3], float b[3], ImU32 color, float& minX, float& maxX, float& minY, float& maxY) {
	float ac[3] = { a[0], a[1], a[2] };
	float bc[3] = { b[0], b[1], b[2] };
	if (Util::WorldToScreen(width, height, ac) && Util::WorldToScreen(width, height, bc)) {
		window.DrawList->AddLine(ImVec2(ac[0], ac[1]), ImVec2(bc[0], bc[1]), color, 2.0f);

		minX = min(ac[0], minX);
		minX = min(bc[0], minX);

		maxX = max(ac[0], maxX);
		maxX = max(bc[0], maxX);

		minY = min(ac[1], minY);
		minY = min(bc[1], minY);

		maxY = max(ac[1], maxY);
		maxY = max(bc[1], maxY);
	}
}

__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	static float width = 0;
	static float height = 0;
	static HWND hWnd = 0;
	ImGuiIO& io = ImGui::GetIO();

	if (!device) {
		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);

		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();

		ID3D11Texture2D* backBuffer = 0;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		backBuffer->GetDesc(&backBufferDesc);

		hWnd = FindWindow((L"UnrealWindow"), (L"Fortnite  "));
		if (!width) {
			oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
		}

		width = (float)backBufferDesc.Width;
		height = (float)backBufferDesc.Height;
		backBuffer->Release();

		ImGui::GetIO().Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\arialbd.ttf"), 21.0f);

		ImGui_ImplDX11_Init(hWnd, device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();
	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	auto& window = BeginScene();
	auto success = FALSE;
	do {
		float closestDistance = FLT_MAX;
		PVOID closestPawn = NULL;

		auto world = *Offsets::uWorld;
		if (!world) break;

		auto gameInstance = ReadPointer(world, Offsets::Engine::World::OwningGameInstance);
		if (!gameInstance) break;

		auto localPlayers = ReadPointer(gameInstance, Offsets::Engine::GameInstance::LocalPlayers);
		if (!localPlayers) break;

		auto localPlayer = ReadPointer(localPlayers, 0);
		if (!localPlayer) break;

		auto localPlayerController = ReadPointer(localPlayer, Offsets::Engine::Player::PlayerController);
		if (!localPlayerController) break;

		auto localPlayerPawn = reinterpret_cast<UObject*>(ReadPointer(localPlayerController, Offsets::Engine::PlayerController::AcknowledgedPawn));
		if (!localPlayerPawn) break;

		auto localPlayerWeapon = ReadPointer(localPlayerPawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
		if (!localPlayerWeapon) break;

		auto localPlayerRoot = ReadPointer(localPlayerPawn, Offsets::Engine::Actor::RootComponent);
		if (!localPlayerRoot) break;

		auto localPlayerState = ReadPointer(localPlayerPawn, Offsets::Engine::Pawn::PlayerState);
		if (!localPlayerState) break;

		auto localPlayerLocation = reinterpret_cast<float*>(reinterpret_cast<PBYTE>(localPlayerRoot) + Offsets::Engine::SceneComponent::RelativeLocation);
		auto localPlayerTeamIndex = ReadDWORD(localPlayerState, Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

		auto weaponName = Util::GetObjectFirstName((UObject*)localPlayerWeapon);
		auto isProjectileWeapon = wcsstr(weaponName.c_str(), L"");

		Core::LocalPlayerPawn = localPlayerPawn;
		Core::LocalPlayerController = localPlayerController;

		std::vector<PVOID> playerPawns;
		for (auto li = 0UL; li < ReadDWORD(world, Offsets::Engine::World::Levels + sizeof(PVOID)); ++li) {
			auto levels = ReadPointer(world, Offsets::Engine::World::Levels);
			if (!levels) break;

			auto level = ReadPointer(levels, li * sizeof(PVOID));
			if (!level) continue;

			for (auto ai = 0UL; ai < ReadDWORD(level, Offsets::Engine::Level::AActors + sizeof(PVOID)); ++ai) {
				auto actors = ReadPointer(level, Offsets::Engine::Level::AActors);
				if (!actors) break;

				auto pawn = reinterpret_cast<UObject*>(ReadPointer(actors, ai * sizeof(PVOID)));
				if (!pawn || pawn == localPlayerPawn) continue;
				auto name = Util::GetObjectFirstName(pawn);
				if (wcsstr(name.c_str(), L"PlayerPawn_Athena_C") || wcsstr(name.c_str(), L"PlayerPawn_Athena_Phoebe_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_Base_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_DateAlpha_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_Date_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_Lonely_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_TomatoAlpha_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_Tomato_C") || wcsstr(name.c_str(), L"BP_GasketPlayerPawn_Wasabi_C") || wcsstr(name.c_str(), L"BP_MangPlayerPawn") || wcsstr(name.c_str(), L"HoagieVehicle_C")) {
					playerPawns.push_back(pawn);
				}

				else	if (Settings.Containers && wcsstr(name.c_str(), L"AthenaSupplyDrop_Llama")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Fat Llama", ImGui::GetColorU32({ 0.03f, 0.78f, 0.91f, 1.0f }));
				}

				else if (Settings.Containers && wcsstr(name.c_str(), L"HoagieVehicle_C")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Chopper", ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }));
				}
				else if (Settings.Containers && wcsstr(name.c_str(), L"MeatballVehicle_L")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Im under the watter help me", ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }));
				}
				else if (Settings.Containers && wcsstr(name.c_str(), L"BasicCar")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "gay car", ImGui::GetColorU32({ 0.031f, 0.984f, 0.075f, 1.0f }));
				}

				else if (Settings.Containers && wcsstr(name.c_str(), L"FireTruck")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "FIRETRUCK", ImGui::GetColorU32({ 0.031f, 0.984f, 0.075f, 1.0f }));
				}

				else if (Settings.Containers && wcsstr(name.c_str(), L"Icecream")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Free Ice", ImGui::GetColorU32({ 0.031f, 0.984f, 0.075f, 1.0f }));
				}
				else if (Settings.Containers && wcsstr(name.c_str(), L"Taxi")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Taxi", ImGui::GetColorU32({ 255,255,0,255 }));
				}
				else if (Settings.Containers && wcsstr(name.c_str(), L"BP_HoverDronePawn_C")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Drone", ImGui::GetColorU32({ 255,255,0,255 }));
				}
			}
		}
		float CurrentAimPointer[3] = { 0 };
		float AimPointer;
		if (Settings.AimPoint == 0) {
			AimPointer = BONE_HEAD_ID;
		}
		else if (Settings.AimPoint == 1) {
			AimPointer = BONE_NECK_ID;
		}
		else if (Settings.AimPoint == 2) {
			AimPointer = BONE_CHEST_ID;
		}
		else if (Settings.AimPoint == 3) {
			AimPointer = BONE_PELVIS_ID;
		}
		else if (Settings.AimPoint == 4) {
			AimPointer = BONE_RIGHTELBOW_ID;
		}
		else if (Settings.AimPoint == 5) {
			AimPointer = BONE_LEFTELBOW_ID;
		}
		else if (Settings.AimPoint == 6) {
			AimPointer = BONE_RIGHTTHIGH_ID;
		}
		else if (Settings.AimPoint == 7) {
			AimPointer = BONE_LEFTTHIGH_ID;
		}
		else if (Settings.AimPoint == 8) {
		}

		for (auto pawn : playerPawns)
		{
			auto state = ReadPointer(pawn, Offsets::Engine::Pawn::PlayerState);
			if (!state) continue;

			auto mesh = ReadPointer(pawn, Offsets::Engine::Character::Mesh);
			if (!mesh) continue;

			auto bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh);
			if (!bones) bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh + 0x10);
			if (!bones) continue;
			float compMatrix[4][4] = { 0 };
			Util::ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + 0x1C0), compMatrix);
			float root[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 0, root);
			float head[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 66, head);

			float neck[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 65, neck);

			float chest[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 36, chest);

			float pelvis[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 2, pelvis);

			float leftShoulder[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 9, leftShoulder);

			float rightShoulder[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 62, rightShoulder);

			float leftElbow[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 10, leftElbow);

			float rightElbow[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 38, rightElbow);

			float leftHand[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 11, leftHand);

			float rightHand[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 39, rightHand);

			float leftLeg[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 67, leftLeg);

			float rightLeg[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 74, rightLeg);

			float leftThigh[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 73, leftThigh);

			float rightThigh[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 80, rightThigh);

			float leftFoot[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 68, leftFoot);

			float rightFoot[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 75, rightFoot);

			float leftFeet[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 71, leftFeet);

			float rightFeet[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 78, rightFeet);

			float leftFeetFinger[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 72, leftFeetFinger);

			float rightFeetFinger[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 79, rightFeetFinger);

			Util::GetBoneLocation(compMatrix, bones, AimPointer, CurrentAimPointer);
			auto color1 = ImGui::GetColorU32({ green });
			auto color12 = ImGui::GetColorU32({ 1,0,0,1 });
			auto color = ImGui::GetColorU32({ Settings.PlayerTeammate[0], Settings.PlayerTeammate[1], Settings.PlayerTeammate[2], 1.0f });
			auto color13 = ImGui::GetColorU32({ 0,0,0, 1.0f });
			FVector viewPoint = { 0 };
			if (ReadDWORD(state, 0xE88) == localPlayerTeamIndex) {
				color = ImGui::GetColorU32({ 1,0,0,1 });
			}
			else if (!Settings.CheckVisible) {
				auto w2s = *reinterpret_cast<FVector*>(CurrentAimPointer);
				if (Util::WorldToScreen(width, height, &w2s.X)) {
					auto dx = w2s.X - (width / 2);
					auto dy = w2s.Y - (height / 2);
					auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
					if (dist < Settings.AimbotFOV && dist < closestDistance) {
						closestDistance = dist;
						closestPawn = pawn;
					}
				}
			}
			else if ((ReadBYTE(pawn, Offsets::FortniteGame::FortPawn::bIsDBNO) & 1) && (isProjectileWeapon || Util::LineOfSightTo(localPlayerController, pawn, &viewPoint))) {
				color = ImGui::GetColorU32({ 1,0,0,1 });
				if (Settings.AutoAimbot) {
					if (Settings.AimPoint = 8) {
						Util::GetBoneLocation(compMatrix, bones, BONE_HEAD_ID, CurrentAimPointer);
						auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
						auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
						auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
						auto dist = dx * dx + dy * dy + dz * dz;
						if (dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;
						}
						else {
							Util::GetBoneLocation(compMatrix, bones, BONE_NECK_ID, CurrentAimPointer);
							auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
							auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
							auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
							auto dist = dx * dx + dy * dy + dz * dz;
							if (dist < closestDistance) {
								closestDistance = dist;
								closestPawn = pawn;
							}
							else {
								Util::GetBoneLocation(compMatrix, bones, BONE_CHEST_ID, CurrentAimPointer);
								auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
								auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
								auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
								auto dist = dx * dx + dy * dy + dz * dz;
								if (dist < closestDistance) {
									closestDistance = dist;
									closestPawn = pawn;
								}
								else {
									Util::GetBoneLocation(compMatrix, bones, BONE_PELVIS_ID, CurrentAimPointer);
									auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
									auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
									auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
									auto dist = dx * dx + dy * dy + dz * dz;
									if (dist < closestDistance) {
										closestDistance = dist;
										closestPawn = pawn;
									}
								}
							}
						}
					}
					else {
						auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
						auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
						auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
						auto dist = dx * dx + dy * dy + dz * dz;
						if (dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;
						}
					}
				}
				else
				{
					auto w2s = *reinterpret_cast<FVector*>(CurrentAimPointer);
					if (Util::WorldToScreen(width, height, &w2s.X)) {
						auto dx = w2s.X - (width / 2);
						auto dy = w2s.Y - (height / 2);
						auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
						if (dist < Settings.AimbotFOV && dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;
						}
					}
				}
			}

#include "../Header/offsets.h"
#include "../Header/Helper.h"
#include "../Discord/Discord.h"
#include "../Discord/Discord.cpp"
#include "../Discord/Helper.cpp"
			if (GetKeyState(VK_INSERT) & 1)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(500, 600), ImVec2(0, 0), ImGui::GetColorU32({ 0, 0, 0, 1 }));
				ImGui::GetWindowDrawList()->AddRect(ImVec2(500, 600), ImVec2(0, 0), ImGui::GetColorU32({ 1, 1, 1, 1 }));
				ImGui::GetWindowDrawList()->AddText(ImVec2(5, 0), ImGui::GetColorU32({ green }), "COVID-69 REBORN by Kenny's Cheetos#6969");
				if (GetKeyState(VK_F1) & 1)
				{
					Settings.Aimbot = true;
				}
				else
				{
					Settings.Aimbot = false;
				}
				if (Settings.Aimbot)
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ green }), "F1 Aimbot = enabled");
				}
				else
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ 1, 0, 0, 1 }), "F1 Aimbot = disabled");
				}

				if (GetKeyState(VK_F2) & 1)
				{
					Settings.SilentAimbot = true;
				}
				else
				{
					Settings.SilentAimbot = false;
				}
				if (Settings.SilentAimbot)
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ green }), "\n\nF2 Silent Aimbot = enabled");
				}
				else
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ 1, 0, 0, 1 }), "\n\nF2 Aimbot = disabled");
				}

				if (GetKeyState(VK_F3) & 1)
				{
					Settings.Players = true;
				}
				else
				{
					Settings.Players = false;
				}
				if (Settings.Players)
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ green }), "\n\n\nF3 ESP = enabled");
				}
				else
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ 1, 0, 0, 1 }), "\n\n\nF3 ESP = disabled");
				}

				if (GetKeyState(VK_F4) & 1)
				{
					Settings.kazoisgay = true;
				}
				else
				{
					Settings.kazoisgay = false;
				}
				if (Settings.kazoisgay)
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ green }), "\n\n\n\n\nF4 LINE ESP = enabled");
				}
				else
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ 1, 0, 0, 1 }), "\n\n\n\n\nF4 LINE ESP = disabled");
				}

				if (GetKeyState(VK_F5) & 1)
				{
					Settings.burstrapid = true;
				}
				else
				{
					Settings.burstrapid = false;
				}
				if (Settings.burstrapid)
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ green }), "\n\n\n\n\n\nF5 RAPIDE FIRE = enabled");
				}
				else
				{
					ImGui::GetWindowDrawList()->AddText(ImVec2(5, 50), ImGui::GetColorU32({ 1, 0, 0, 1 }), "\n\n\n\n\n\nF5 RAPIDE FIRE = disabled");
				}
			}

			if (Settings.Speedhack)
			{
				auto actorCurrentWeapon = ReadPointer(pawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
				if (!actorCurrentWeapon) continue;

				auto actorItemDef = ReadPointer(actorCurrentWeapon, Offsets::FortniteGame::FortWeapon::WeaponData);
				if (!actorItemDef) continue;

				auto actorItemDisplayName = reinterpret_cast<FText*>(ReadPointer(actorItemDef, Offsets::FortniteGame::FortItemDefinition::DisplayName));
				if (!actorItemDisplayName || !actorItemDisplayName->c_str()) continue;

				auto bottomPos = *reinterpret_cast<FVector*>(head);

				if (Util::WorldToScreen(width, height, &bottomPos.X))
				{
					CHAR text[0xFF] = { 0 };
					wcstombs(text, actorItemDisplayName->c_str(), sizeof(text));
				}
			}

			float minX = FLT_MAX;
			float maxX = -FLT_MAX;
			float minY = FLT_MAX;
			float maxY = -FLT_MAX;

			auto end = *reinterpret_cast<FVector*>(CurrentAimPointer);
			if (Util::WorldToScreen(width, height, &end.X)) {
				if (Settings.Players) {
					AddLine(window, width, height, head, neck, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, neck, pelvis, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, chest, leftShoulder, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, chest, rightShoulder, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, leftShoulder, leftElbow, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, rightShoulder, rightElbow, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, leftElbow, leftHand, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, rightElbow, rightHand, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, pelvis, leftLeg, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, pelvis, rightLeg, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, leftLeg, leftThigh, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, rightLeg, rightThigh, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, leftThigh, leftFoot, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, rightThigh, rightFoot, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, leftFoot, leftFeet, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, rightFoot, rightFeet, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, leftFeet, leftFeetFinger, color12, minX, maxX, minY, maxY);
					AddLine(window, width, height, rightFeet, rightFeetFinger, color12, minX, maxX, minY, maxY);
				}

				if (Settings.Keybindss)
				{
					if (Core::LocalPlayerPawn && Core::LocalPlayerController)
					{
						if (Util::SpoofCall(GetAsyncKeyState, Settings.keybind.Airstuck))
						{
							*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(Core::LocalPlayerPawn) + Offsets::Engine::Actor::CustomTimeDilation) = 0;
						}
						else
						{
							*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(Core::LocalPlayerPawn) + Offsets::Engine::Actor::CustomTimeDilation) = 1;
						}
					}
				}

				auto w2s = *reinterpret_cast<FVector*>(head);
				auto dx = w2s.X;
				auto dy = w2s.Y;
				auto dz = w2s.Z;
				auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
				auto topLeft = ImVec2(minX - 3.0f, minY - 3.0f);
				auto bottomRight = ImVec2(maxX + 3.0f, maxY);
				auto topRight = ImVec2(maxX + 3.0f, minY - 3.0f);
				auto bottomLeft = ImVec2(minX - 3.0f, maxY + 3.f);
				if (minX < width && maxX > 0 && minY < height && maxY > 0) {
					auto cornerRAD = (1 / dist) * 5000;
					if (cornerRAD < 2) { cornerRAD = 2; }
					if (cornerRAD > 25) { cornerRAD = 50; }

					window.DrawList->AddRect(topLeft, bottomRight, ImGui::GetColorU32({ 1.0f, 1.0f, 0.0f, 0.7f }), cornerRAD, 15, 3);

					if (ReadDWORD(state, 0xE88) != localPlayerTeamIndex) {
						if (Settings.LineESP) {
							auto w2s = *reinterpret_cast<FVector*>(head);

							auto end = *reinterpret_cast<FVector*>(CurrentAimPointer);
							if (Util::WorldToScreen(width, height, &end.X)) {
								window.DrawList->AddCircleFilled(ImVec2(w2s.X, w2s.Y), 4, ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }), 64);
								float targetRoot[3] = { 0 };

								auto targetPos = *reinterpret_cast<FVector*>(targetRoot);
								if (Util::WorldToScreen(width, height, &targetPos.X))
								{
								}
							}
						}

						if (Settings.kazoisgay)
						{
							if (Settings.Aimbot)
							{
								int TargetHitbox;
								if (Settings.AimPoint == 0)
								{
									TargetHitbox = 66;
								}
								else if (Settings.AimPoint == 1)
								{
									TargetHitbox = 65;
								}
								else if (Settings.AimPoint == 2)
								{
									TargetHitbox = 5;
								}
								else if (Settings.AimPoint == 3)
								{
									TargetHitbox = 0;
								}
								else if (Settings.AimPoint == 4)
								{
									TargetHitbox = 2;
								}

								if (Core::TargetPawn != nullptr)
								{
									auto targetMesh = ReadPointer(Core::TargetPawn, Offsets::Engine::Character::Mesh);
									if (!targetMesh) continue;

									auto targetBones = ReadPointer(targetMesh, Offsets::Engine::StaticMeshComponent::StaticMesh);
									if (!targetBones) targetBones = ReadPointer(targetMesh, Offsets::Engine::StaticMeshComponent::StaticMesh + 0x10);
									if (!targetBones) continue;

									float targetCompMatrix[4][4] = { 0 };
									Util::ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(targetMesh) + Offsets::Engine::StaticMeshComponent::ComponentToWorld), targetCompMatrix);

									float targetRoot[3] = { 0 };
									Util::GetBoneLocation(targetCompMatrix, targetBones, TargetHitbox, targetRoot);

									auto targetPos = *reinterpret_cast<FVector*>(targetRoot);
									if (Util::WorldToScreen(width, height, &targetPos.X))
									{
										window.DrawList->AddLine(ImVec2(width / 2, height / 2), ImVec2(targetPos.X, targetPos.Y), ImGui::GetColorU32({ 1,0,0,1 }));
									}
								}
							}
						}

						if (Settings.PlayerNames) {
							auto actorCurrentWeapon = ReadPointer(pawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
							if (!actorCurrentWeapon) continue;

							auto actorItemDef = ReadPointer(actorCurrentWeapon, Offsets::FortniteGame::FortWeapon::WeaponData);
							if (!actorItemDef) continue;

							auto actorItemDisplayName = reinterpret_cast<FText*>(ReadPointer(actorItemDef, Offsets::FortniteGame::FortItemDefinition::DisplayName));
							if (!actorItemDisplayName || !actorItemDisplayName->c_str()) continue;

							auto bottomPos = *reinterpret_cast<FVector*>(head);

							if (Util::WorldToScreen(width, height, &bottomPos.X))
							{
								CHAR text[0xFF] = { 0 };
								wcstombs(text, actorItemDisplayName->c_str(), sizeof(text));
								FString playerName;
								Core::ProcessEvent(state, Offsets::Engine::PlayerState::GetPlayerName, &playerName, 0);
								if (playerName.c_str()) {
									CHAR copy[0xFF] = { 0 };
									auto w2s = *reinterpret_cast<FVector*>(head);
									if (Util::WorldToScreen(width, height, &w2s.X)) {
										auto w2s = *reinterpret_cast<FVector*>(head);
										auto dx = w2s.X;
										auto dy = w2s.Y;
										auto dz = w2s.Z;
										auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
										CHAR copy[0xFF] = { 0 };
										wcstombs(copy, playerName.c_str(), sizeof(copy));
										Util::FreeInternal(playerName.c_str());

										auto centerTop = ImVec2((topLeft.x + bottomRight.x) / 2.0f, topLeft.y);
										auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, 0, copy);

										window.DrawList->AddText(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y), color12, text);
									}
								}
							}
						}
					}
				}
			}
		}

		if (Settings.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, 0x02) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = FALSE;

			if (Settings.Aimbot && Settings.AntiAim && Util::SpoofCall(GetAsyncKeyState, 0x02)) {
				int rnd = rand();
				FRotator args = { 0 };
				args.Yaw = rnd;
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
			}
		}
		else {
			Core::TargetPawn = nullptr;
			Core::NoSpread = FALSE;
		}

		bool isSilent = Settings.SilentAimbot;
		bool isRage = Settings.AutoAimbot;
		if (Settings.SpinBot && Util::SpoofCall(GetAsyncKeyState, Settings.keybind.Spinbot) && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			int rnd = rand();
			FRotator args = { 0 };
			args.Yaw = rnd;
			if (closestPawn) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = TRUE;
			}
			else {
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
			}
			Settings.AutoAimbot = true;
			Settings.Aimbot = 10000000000.0f;
			Settings.SilentAimbot = true;
		}
		else {
			if (!isSilent) {
				Settings.SilentAimbot = false;
			}
			if (!isRage) {
				Settings.AutoAimbot = false;
			}

			if (Settings.SilentAimbot) {
				isSilent = true;
			}
			if (Settings.AutoAimbot) {
				isRage = true;
			}
		}

		if (Settings.FlickAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, 0x02) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
		}

		if (Settings.AutoAim && closestPawn && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
		}

		if (Settings.DrawAimbotFOV) {
			auto cornerRAD = 8000;
			if (cornerRAD < 2) { cornerRAD = 2; }
			if (cornerRAD > 25) { cornerRAD = 25; }
			window.DrawList->AddCircle(ImVec2(width / 2, height / 2), Settings.AimbotFOV, ImGui::GetColorU32({ 1.0f, 0.0f, 1.0f, Settings.FOVCircleFilledOpacity }), 36);
			if (Settings.DrawFilledAimbotFOV) {
			}
		}

		if (Settings.CrosshairSize) {
			window.DrawList->AddLine(ImVec2(width / 2 - 15, height / 2), ImVec2(width / 2 + 15, height / 2), ImGui::GetColorU32(red), 2);
			window.DrawList->AddLine(ImVec2(width / 2, height / 2 - 15), ImVec2(width / 2, height / 2 + 15), ImGui::GetColorU32(green), 2);
		}

		success = TRUE;
	} while (FALSE);

	if (!success) {
		Core::LocalPlayerController = Core::LocalPlayerPawn = Core::TargetPawn = nullptr;
	}
	EndScene(window);
	return PresentOriginal(swapChain, syncInterval, flags);
}

__declspec(dllexport) HRESULT ResizeHook(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	ImGui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return ResizeOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

bool Render::Initialize() {
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	auto featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow = FindWindow((L"UnrealWindow"), (L"Fortnite  "));

	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBox(0, L"Critical error have happened\nPlease contact an admin with the error code:\n0x0001b", L"Error", MB_ICONERROR);
		return FALSE;
	}

	auto table = *reinterpret_cast<PVOID**>(swapChain);
	auto present = table[8];
	auto resize = table[13];

	context->Release();
	device->Release();
	swapChain->Release();

	const auto pcall_present_discord = Helper::PatternScan(Discord::GetDiscordModuleBase(), "FF 15 ? ? ? ? 8B D8 E8 ? ? ? ? E8 ? ? ? ? EB 10");
	auto presentSceneAdress = Helper::PatternScan(Discord::GetDiscordModuleBase(),
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 41 8B F8");

	DISCORD.HookFunction(presentSceneAdress, (uintptr_t)PresentHook, (uintptr_t)&PresentOriginal);

	DISCORD.HookFunction(presentSceneAdress, (uintptr_t)ResizeHook, (uintptr_t)&PresentOriginal);
	return TRUE;
}