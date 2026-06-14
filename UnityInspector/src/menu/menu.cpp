#include "pch.h"
#include "menu.h"
#include "misc_tab/misc_tab.h"
#include "debug_tab/debug_tab.h"
#include "lua_tab/lua_tab.h"
#include "config/config.h"

namespace Menu
{
	static std::vector<std::unique_ptr<ITab>> s_Tabs;
	static bool s_Initialized = false;

	static std::atomic s_UpdateCheckFinished = false;
	static bool s_HasUpdate = false;
	static std::string s_LatestVersion = "";
	static std::string s_ReleaseUrl = "";
	static std::string s_UpdateError = "";

	static std::string FetchLatestReleaseJson(std::string& outError)
	{
		std::string responseData;
		HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;

		hSession = WinHttpOpen(L"UnityInspectorStandalone/1.0",
		                       WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		                       nullptr,
		                       nullptr, 0);
		if (!hSession)
		{
			outError = "WinHttpOpen failed: " + std::to_string(GetLastError());
			return "";
		}

		WinHttpSetTimeouts(hSession, 5000, 5000, 5000, 5000);

		hConnect = WinHttpConnect(hSession, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (!hConnect)
		{
			outError = "WinHttpConnect failed: " + std::to_string(GetLastError());
			WinHttpCloseHandle(hSession);
			return "";
		}

		hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/repos/PicoShot/UnityInspectorStandalone/releases/latest",
		                              nullptr, nullptr,
		                              nullptr,
		                              WINHTTP_FLAG_SECURE);
		if (!hRequest)
		{
			outError = "WinHttpOpenRequest failed: " + std::to_string(GetLastError());
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		BOOL bResults = WinHttpSendRequest(hRequest,
		                                   nullptr, 0,
		                                   nullptr, 0,
		                                   0, 0);

		if (!bResults)
		{
			outError = "WinHttpSendRequest failed: " + std::to_string(GetLastError());
		}
		else
		{
			bResults = WinHttpReceiveResponse(hRequest, nullptr);
			if (!bResults)
			{
				outError = "WinHttpReceiveResponse failed: " + std::to_string(GetLastError());
			}
		}

		if (bResults)
		{
			DWORD dwSize = 0;
			do
			{
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				{
					outError = "WinHttpQueryDataAvailable failed: " + std::to_string(GetLastError());
					break;
				}
				if (dwSize == 0) break;

				std::vector<char> tempBuffer(dwSize);
				DWORD dwDownloaded = 0;
				if (!WinHttpReadData(hRequest, tempBuffer.data(), dwSize, &dwDownloaded))
				{
					outError = "WinHttpReadData failed: " + std::to_string(GetLastError());
					break;
				}
				responseData.append(tempBuffer.data(), dwDownloaded);
			}
			while (dwSize > 0);
		}

		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		return responseData;
	}

	static std::vector<int> ParseVersion(const std::string& verStr)
	{
		std::vector<int> parts;
		std::string current;
		for (char c : verStr)
		{
			if (std::isdigit(static_cast<unsigned char>(c)))
			{
				current += c;
			}
			else if (c == '.' || c == '-')
			{
				if (!current.empty())
				{
					parts.push_back(std::stoi(current));
					current.clear();
				}
				if (c == '-') break;
			}
		}
		if (!current.empty())
		{
			parts.push_back(std::stoi(current));
		}
		return parts;
	}

	static bool IsNewerVersion(const std::string& latest, const std::string& current)
	{
		auto latest_parts = ParseVersion(latest);
		auto current_parts = ParseVersion(current);
		for (size_t i = 0; i < std::max(latest_parts.size(), current_parts.size()); ++i)
		{
			int l = (i < latest_parts.size()) ? latest_parts[i] : 0;
			int c = (i < current_parts.size()) ? current_parts[i] : 0;
			if (l > c) return true;
			if (l < c) return false;
		}
		return false;
	}

	void Init()
	{
		if (s_Initialized) return;
		s_Tabs.push_back(std::make_unique<DebugTab>());
		s_Tabs.push_back(std::make_unique<LuaConsoleTab>());
		s_Tabs.push_back(std::make_unique<MiscTab>());
		s_Initialized = true;

		std::thread([]
		{
			std::string err;
			std::string jsonStr = FetchLatestReleaseJson(err);
			if (!err.empty())
			{
				s_UpdateError = err;
				s_UpdateCheckFinished = true;
				return;
			}

			try
			{
				if (Json json = Json::parse(jsonStr); json.contains("tag_name") && json["tag_name"].is_string())
				{
					std::string latestTag = json["tag_name"];
					s_LatestVersion = latestTag;

					if (json.contains("html_url") && json["html_url"].is_string())
					{
						s_ReleaseUrl = json["html_url"];
					}
					else
					{
						s_ReleaseUrl = "https://github.com/PicoShot/UnityInspectorStandalone/releases";
					}

					if (IsNewerVersion(latestTag, VERSION))
					{
						s_HasUpdate = true;
					}
				}
			}
			catch (const std::exception& e)
			{
				s_UpdateError = std::string("JSON parse error: ") + e.what();
			}
			s_UpdateCheckFinished = true;
		}).detach();
	}

	void Render()
	{
		if (!s_Initialized) Init();

		if (Config::state.showMenu)
		{
			ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowPos(ImVec2(600, 400), ImGuiCond_FirstUseEver);

			std::string menuTitle = "UnityInspector v" VERSION;
#ifdef _DEBUG
			menuTitle += " (Debug)";
#endif

			ImGui::Begin(menuTitle.c_str(), &Config::state.showMenu,
			             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

			if (s_UpdateCheckFinished && s_HasUpdate)
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.26f, 0.59f, 0.98f, 0.20f));
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));

				if (ImGui::BeginChild("UpdateBanner", ImVec2(0, 34),
				                      ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding))
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("A new version (%s) is available!", s_LatestVersion.c_str());
					ImGui::SameLine();

					float button_width = 80.0f;
					ImGui::SetCursorPosX(ImGui::GetWindowWidth() - button_width - 8.0f);
					if (ImGui::Button("Update", ImVec2(button_width, 0)))
					{
						ShellExecuteA(nullptr, "open", s_ReleaseUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
					}
				}
				ImGui::EndChild();

				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor();
				ImGui::Spacing();
			}

			if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
			{
				for (const auto& tab : s_Tabs)
				{
					if (ImGui::BeginTabItem(tab->GetName().c_str()))
					{
						tab->Render();
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}
}
