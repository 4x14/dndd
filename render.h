#include "../game/render/loop.h"
#include "../overlay/misc/icons.h"
#include <D3D11.h>
#include "../Vendor/ImGui/imgui_impl_dx11.h"
#include "../overlay/misc/icon.h"
#include "../Vendor/ImGui/imgui_internal.h"
#include "../overlay/misc/imgui_notify.h"
#include <D3DX11core.h>
#include "../overlay/misc/fonts.h"
#include "../overlay/misc/image.h"
#include <D3DX11.h>
#include <D3DX11tex.h>
#include "menu.h"
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
HWND window_handle;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;

ID3D11ShaderResourceView* Logo = nullptr;

ImVec2 pos;
ImFont* icons;
ImFont* colorpicker;
ImFont* tabfont;
ImFont* MenuFont1;

namespace n_render
{
    class c_render {
    public:
        auto imgui() -> bool
        {
            SPOOF_FUNC

            DXGI_SWAP_CHAIN_DESC swap_chain_description;
            ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
            swap_chain_description.BufferCount = 2;
            swap_chain_description.BufferDesc.Width = 0;
            swap_chain_description.BufferDesc.Height = 0;
            swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
            swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
            swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_description.OutputWindow = window_handle;
            swap_chain_description.SampleDesc.Count = 1;
            swap_chain_description.SampleDesc.Quality = 0;
            swap_chain_description.Windowed = 1;
            swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            D3D_FEATURE_LEVEL d3d_feature_lvl;

            const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

            D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

            ID3D11Texture2D* pBackBuffer;
            D3DX11_IMAGE_LOAD_INFO info;
            ID3DX11ThreadPump* pump{ nullptr };
            //D3DX11CreateShaderResourceViewFromMemory(d3d_device, rawData, sizeof(rawData), &info,
            //    pump, &Logo, 0);
            d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

            d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

            pBackBuffer->Release();

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
            ImFontConfig icons_config;

            ImFontConfig CustomFont;
            CustomFont.FontDataOwnedByAtlas = false;

            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            icons_config.OversampleH = 2.5;
            icons_config.OversampleV = 2.5;

            ImGui_ImplWin32_Init(window_handle);
            ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);


            ImFontConfig font_config;
            font_config.OversampleH = 1;
            font_config.OversampleV = 1;
            font_config.PixelSnapH = 1;


            static const ImWchar ranges[] =
            {
                0x0020, 0x00FF,
                0x0400, 0x044F,
                0,
            };

            MenuFont = io.Fonts->AddFontFromFileTTF(E("C:\\fortnite.otf"), 18.f);
            IconsFont = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 22.0f, &icons_config, icons_ranges);

            GameFont = io.Fonts->AddFontFromFileTTF(E("C:\\fortnite.otf"), 15.f);

            MenuFont1 = io.Fonts->AddFontFromFileTTF(E("C:\\fortnite.otf"), 18.f);

            tabfont = io.Fonts->AddFontFromMemoryTTF(&mainfonthxd, sizeof mainfonthxd, 16, NULL, io.Fonts->GetGlyphRangesCyrillic());
            icons = io.Fonts->AddFontFromMemoryTTF(iconshxd, sizeof(iconshxd), 14.0f, &font_config, ranges);
            colorpicker = io.Fonts->AddFontFromMemoryTTF(iconshxd, sizeof(iconshxd), 28.0f, &font_config, ranges);

            d3d_device->Release();

            return true;
        }

        auto hijack() -> bool
        {
            SPOOF_FUNC;

            // window_handle = FindWindowA_Spoofed(E("CiceroUIWndFrame"), E("CiceroUIWndFrame"));                    // Visual Studio
            window_handle = FindWindowA_Spoofed(E("MedalOverlayClass"), E("MedalOverlay"));                          // Medal
            // window_handle = FindWindowA_Spoofed(E("CEF-OSC-WIDGET"), E("NVIDIA GeForce Overlay"));                // Nvidia
            // window_handle = FindWindowA_Spoofed(E("GDI+ Hook Window Class"), E("GDI+ Window (FPSMonitor.exe)"));  // Fps Monitor

            if (!window_handle) {
                MessageBoxA(0, skCrypt("{!} Overlay Failed."), skCrypt(""), MB_ICONINFORMATION);
                return false;
            }

            // Set window attributes
            SetWindowPos_Spoofed(window_handle, HWND_TOPMOST, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
            SetLayeredWindowAttributes_Spoofed(window_handle, RGB(0, 0, 0), 255, LWA_ALPHA);
            SetWindowLongA_Spoofed(window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

            // Extend frame into client area
            MARGINS margin = { -1 };
            DwmExtendFrameIntoClientArea(window_handle, &margin);

            // Show and update window
            ShowWindow_Spoofed(window_handle, SW_SHOW);
            UpdateWindow_Spoofed(window_handle);

            // Hide window
            ShowWindow_Spoofed(window_handle, SW_HIDE);

            return true;
        }

        auto menu() -> void
        {
            SPOOF_FUNC;

            if (GetAsyncKeyState_Spoofed(VK_RCONTROL) & 1)
                globals::ShowWindow = !globals::ShowWindow;

            char fpsText[256];
            sprintf_s(fpsText, std::string("NiggerFart <-> MS: %.1f <-> FPS: %.1f").c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::GetBackgroundDrawList()->AddText(ImVec2(5, 5), ImColor(255, 255, 255), fpsText);

            if (globals::ShowWindow)
            {
                rendermenu();
            }
        }

        auto draw() -> void
        {
            SPOOF_FUNC;

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            g_main->actor_loop();
            menu();

            // Render ImGui frame
            ImGui::Render();
            const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
            d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
            d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // Present the frame

            if (misc::vsync)
            {
                d3d_swap_chain->Present(1, 0); // VSYNC enabled
            }
            else
            {
                d3d_swap_chain->Present(0, 0); // VSYNC enabled
            }
        }

        auto render() -> bool
        {
            SPOOF_FUNC

            static RECT rect_og;
            MSG msg = { NULL };
            ZeroMemory(&msg, sizeof(MSG));

            while (msg.message != WM_QUIT)
            {

                UpdateWindow_Spoofed(window_handle);
                ShowWindow_Spoofed(window_handle, SW_SHOW);

                if (PeekMessageA_Spoofed(&msg, window_handle, 0, 0, PM_REMOVE))
                {
                    TranslateMessage_Spoofed(&msg);
                    DispatchMessage(&msg);
                }

                ImGuiIO& io = ImGui::GetIO();
                io.DeltaTime = 1.0f / 60.0f;


                POINT p_cursor;
                GetCursorPos(&p_cursor);
                io.MousePos.x = p_cursor.x;
                io.MousePos.y = p_cursor.y;

                if (GetAsyncKeyState_Spoofed(VK_LBUTTON)) {
                    io.MouseDown[0] = true;
                    io.MouseClicked[0] = true;
                    io.MouseClickedPos[0].x = io.MousePos.x;
                    io.MouseClickedPos[0].x = io.MousePos.y;
                }
                else
                    io.MouseDown[0] = false;

                draw();


            }
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            DestroyWindow(window_handle);

            return true;

        }

    };
} static n_render::c_render* render = new n_render::c_render();