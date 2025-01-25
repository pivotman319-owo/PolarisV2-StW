#include "about_window.h"
#include "imgui_text_extension.h"
#include "build.h"

#include <string>

namespace polaris::ui::window::windows
{
	void AboutWindow::Draw()
	{
		std::string verNum = POLARIS_VER;
		std::string verEnv = POLARIS_ID;
		std::string combined = verNum + "-" + verEnv;

		ImGui::SetNextWindowSize(ImVec2(370, 650));
		ImGui::Begin("About", &m_bIsOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		{
			ImGui::Header("Polaris");
			ImGui::SameLine();

			ImGui::Header(ImVec4(1.0f, 1.0f, 1.0f, 0.15f), combined.c_str());

			ImGui::SmallText("They said to think back, but I think we went a bit too far.");

			ImGui::Dummy(ImVec2(1, 10));

			ImGui::Header2("Lead Developer");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Cyuubi");
			ImGui::Text("irma");

			ImGui::Dummy(ImVec2(1, 5));

			ImGui::Header2("Developer");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "BattleDash");
			ImGui::Text("pivotman319");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "kanner");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Samicc");
			ImGui::Text("wiktorwiktor12");

			ImGui::Dummy(ImVec2(1, 5));

			ImGui::Header2("Relation");
			ImGui::Text("crush");
			ImGui::Text("darkblade");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Slushia");

			ImGui::Dummy(ImVec2(1, 5));

			ImGui::Header2("Special Thanks");
			ImGui::Text("chase");

			ImGui::Dummy(ImVec2(1, 10));

			ImGui::Dummy(ImVec2(260, 0));
			ImGui::SameLine();
			if (ImGui::Button("Dismiss"))
				m_bIsOpen = false;

			ImGui::End();
		}
	}
}