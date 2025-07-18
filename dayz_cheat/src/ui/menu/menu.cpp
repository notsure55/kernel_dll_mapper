#include "menu.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "../../toggles/toggles.hpp"
#include "../../features/esp/esp.hpp"

namespace Menu {

    void menu() {
        ImGui::Begin("Black");
        if (ImGui::BeginTabBar("Modules")) {
            if (ImGui::BeginTabItem("Aimbot")) {
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void display(ID3D11DeviceContext* context, ID3D11RenderTargetView* mainRenderTargetView) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
        Esp::run();
        if (Toggles::menu) {
            menu();
        }
		ImGui::Render();
		context->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}
