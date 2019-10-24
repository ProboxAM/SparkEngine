#include "Application.h"
#include "glew/glew.h"
#include "PanelAbout.h"



PanelAbout::PanelAbout(bool a) : Panel(a)
{
}


PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGui::Begin("About", &active);
	// Engine description
	ImGui::Text("Spark Engine");
	ImGui::Spacing();
	ImGui::TextWrapped("SparkEngine is a 3d videogame engine developed during the Game Development grade at CITM Terrasa (Spain).");
	if (ImGui::Button("SparkEngine Github")) { App->OpenWebURL("https://github.com/ProboxAM/SparkEngine"); }
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Authors
	ImGui::Text("Authors");
	ImGui::Spacing();
	if (ImGui::Button("Marc Guillen")) { App->OpenWebURL("https://github.com/Marcgs96"); }
	if (ImGui::Button("Axel Alavedra")) { App->OpenWebURL("https://github.com/AxelAlavedra"); }
	ImGui::Spacing();
	if (ImGui::Button("ProBoxAM")) { App->OpenWebURL("https://github.com/ProboxAM"); }
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Libraries
	ImGui::Text("Libraries");
	ImGui::Spacing();

	SDL_version version;
	SDL_GetVersion(&version);
	if (ImGui::Button("SDL")) { App->OpenWebURL("https://www.libsdl.org/"); }
	ImGui::SameLine(); ImGui::TextWrapped("%i.%i.%i", version.major, version.minor, version.patch);

	if (ImGui::Button("OpenGL")) { App->OpenWebURL("https://www.opengl.org/"); }
	ImGui::SameLine(); ImGui::TextWrapped("%s", glGetString(GL_VERSION));

	if (ImGui::Button("Glew")) { App->OpenWebURL("http://glew.sourceforge.net/"); }
	ImGui::SameLine(); ImGui::TextWrapped("%s", glewGetString(GLEW_VERSION));

	if (ImGui::Button("ImGui")) { App->OpenWebURL("https://github.com/ocornut/imgui"); }
	ImGui::SameLine(); ImGui::TextWrapped("%s", ImGui::GetVersion());

	if (ImGui::Button("MathGeoLib")) { App->OpenWebURL("https://github.com/juj/MathGeoLib"); }
	ImGui::SameLine(); ImGui::TextWrapped("1.5");

	if (ImGui::Button("PCG Random Number Generator")) { App->OpenWebURL("http://www.pcg-random.org/"); }
	ImGui::SameLine(); ImGui::TextWrapped("0.98");

	if (ImGui::Button("JSON for modern C++")) { App->OpenWebURL("https://github.com/nlohmann/json"); }
	ImGui::SameLine(); ImGui::TextWrapped("3.7.0");

	if (ImGui::Button("Open-Asset-Importer-Lib")) { App->OpenWebURL("http://www.assimp.org/"); }
	ImGui::SameLine(); ImGui::TextWrapped("5.0.0");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// License
	ImGui::TextWrapped("MIT License");
	ImGui::TextWrapped("Copyright(c) 2019 SparkEngine (Marc Guillen and Axel Alavedra)");
	ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the %cSoftware%c), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:", 34, 34);
	ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
	ImGui::TextWrapped("THE SOFTWARE IS PROVIDED %cAS IS%c, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.", 34, 34);

	ImGui::End();
}
