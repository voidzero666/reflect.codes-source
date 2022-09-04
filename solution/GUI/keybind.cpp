#include "keybind.h"
#include "child.h"
#include "../Math.h"
#include "../imgui/imgui_internal.h"


const std::string state_names[] = { crypt_str("Always off"), crypt_str("Hold"), crypt_str("Toggle"), crypt_str("Release"), crypt_str("Always on") };

void pKeybind::imgui(std::string label = "")
{
	//name = label;
	ImGui::Text(name.c_str());
	//std::string keyname = crypt_str("");

	if (this->type != 0 && this->type != 4)
	{
		ImGui::SameLine();
		std::string keyname;

		if (!this->opened)
		{
			if (key)
			{
				//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("[ %s ]"), interfaces.inputsystem->vkey2string(key));
				keyname = std::string(crypt_str("[ ")) + std::string(interfaces.inputsystem->vkey2string(key)) + std::string(crypt_str(" ]"));
			}
			else
			{
				//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("[ key ]"));
				keyname = crypt_str("[ key ]");
			}
		}
		else
		{
			keyname = crypt_str("[ Press key ]");
		}
		

		if (this->opened)
		{
			ImGuiIO& io = ImGui::GetIO();
			for (int i = 0; i < 512; i++)
			{
				if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
				{
					key = i != VK_ESCAPE ? i : 0;
					if (i == 160 || i == 161)
					{
						key = VK_SHIFT;
					}
					else if (i == 162 || i == 163)
					{
						key = VK_CONTROL;
					}
					else if (i == 164 || i == 165)
					{
						key = VK_MENU;
					}

					this->opened = false;
				}
			}
				
			for (int i = 0; i < 5; i++)
			{
				if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != VK_INSERT)
				{
					key = i + (i > 1 ? 2 : 1);
					this->opened = false;
				}
			}		
		}
		
		
		if (ImGui::Button(keyname.c_str()))
		{
			this->opened = true;
		}
		
		if(ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(crypt_str("Click to change keybind"));
		}
	}

	//ImGui::SameLine(ImGui::CalcTextSize(std::string(crypt_str("\t") + name + keyname).c_str()).x + 5.f);
	ImGui::SameLine(0.f, 5.f);
	ImGui::PushID(name.c_str());
	
	ImGui::SetNextItemWidth(ImGui::CalcTextSize(state_names[this->type].c_str()).x + 9.f);
	if (ImGui::BeginCombo("", state_names[this->type].c_str()))
	{
		float epicsize = ImGui::CalcTextSize(crypt_str("Always off")).x + 9.f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
		for (int i = 0; i < 5; i++)
		{
			bool selected = (i == this->type);
			if (ImGui::Selectable(state_names[i].c_str(), &selected, 0, { epicsize, 16.f }))
				this->type = i;

		}

		ImGui::EndCombo();
	}
	//ImGui::Combo("", &this->type, crypt_str("Always off\0Hold\0Toggle\0Release\0Always on\0"));
	ImGui::PopID();
	
}