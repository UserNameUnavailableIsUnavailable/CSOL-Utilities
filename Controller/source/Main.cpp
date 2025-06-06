﻿#include "pch.hpp"

#include "CSOBannerSuppressor.hpp"
#include "CommandDispatcher.hpp"
#include "Driver.hpp"
#include "Exception.hpp"
#include "Global.hpp"
#include "CLI/CLI.hpp"
#include "Console.hpp"
#include "HotKey.hpp"
#include "IdleEngine.hpp"
#include "LowLevelKeyboardHook.hpp"
#include "OCRIdleEngine.hpp"
#include "Utilities.hpp"

using namespace CSOL_Utilities;

std::unique_ptr<Driver> g_Driver;

void InitializeCommandLineArgs(CLI::App& app, int argc, wchar_t* argv[]);
void Boot();

#ifndef Test__
/* 使用 wmain 作为入口，确保命令行参数以 UTF-16 传入 */
int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	using namespace CSOL_Utilities;
	CLI::App main_app{ "CSOL-Utilities" };
	InitializeCommandLineArgs(main_app, argc, argv);
	CLI11_PARSE(main_app, argc, argv)
	std::string module_name = "Driver";
	DWORD dwErrorCode = ERROR_SUCCESS;
	try
	{
		Boot();
		g_Driver->Launch();
	}
	catch (std::exception& e)
	{
		Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
		Console::Error(Translate("Main::ERROR_Panic"));
		auto _ = std::getchar();
		dwErrorCode = GetLastError();
	}
	g_Driver.reset(); /* 析构 Driver，结束所有线程 */
	Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
	Console::Info(Translate("Main::INFO_SafelyExited"));
	return dwErrorCode;
}
#endif

void InitializeCommandLineArgs(CLI::App& app, int argc, wchar_t* argv[])
{
	app.add_option("--locale-resources-directory", Global::LocaleResourcesDirectory);
	app.add_option("--language", Global::LocaleName);
	app.add_option("--game-root-directory", Global::GameRootDirectory);
	app.add_option("--launch-game-command", Global::LaunchGameCmd);
	app.add_option("--executor-command-file-path", Global::ExecutorCommandFilePath);
	app.add_option("--start-game-room-timeout", Global::StartGameRoomTimeout);
	app.add_option("--login-timeout", Global::LoginTimeout);
	app.add_option("--load-map-timeout", Global::LoadMapTimeout);
	app.add_option("--LGHUB-Agent-name", Global::LGHUB_Agent_Name);
	app.add_option("--detect-mode", Global::DetectMode);
	app.add_option("--OCR-detection-model-path", Global::OCRDetectionModelPath);
	app.add_option("--OCR-recognition-model-path", Global::OCRRecognitionModelPath);
	app.add_option("--OCR-dictionary-path", Global::OCRDictionaryPath);
	app.add_option("--OCR-keywords-path", Global::OCRKeywordsPath);
	app.add_flag("--default-idle-after-reconnection", Global::DefaultIdleAfterReconnection);
	app.add_flag("--restart-game-on-loading-timeout", Global::RestartGameOnLoadingTimeout);
	app.add_flag("--allow-quick-fullscreen", Global::AllowQuickFullScreen);
	app.add_flag("--suppress-CSOBanner", Global::SuppressCSOBanner);
}

void Boot()
{
	Global::LocaleName += L".UTF-8";
	std::locale locale(ConvertUtf16ToUtf8(Global::LocaleName));
	std::locale::global(locale);
	LoadLanguagePackage(Global::g_LanguagePackage);
	Console::Info(Translate("Main::INFO_Version@1", "1.5.2"));
	Console::Warn(Translate("Main::WARN_Note"));
	Console::Info(Translate("Main::INFO_Author"));
	Console::Info(Translate("Main::INFO_Feedback"));
	Console::Info(Translate("Main::INFO_Gitee_URL"));
	Console::Info(Translate("Main::INFO_GitHub_URL"));
	Console::Info(Translate("Main::INFO_HowToExit"));
	if (!IsRunningAsAdmin())
	{
		Console::Warn(Translate("Main::WARN_NotRunningAsAdmin"));
	}
	auto convert_to_absolute_path = [] (const std::filesystem::path& path) {
		if (path.is_relative())
		{
			return GetModulePath().parent_path() / path;
		}
		return path;
	};
	
	auto query_registry_string = [] (HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValue, std::wstring& buffer)
	{
		DWORD cbData = 0;
		auto ret = RegGetValueW(HKEY_CURRENT_USER, lpSubKey, lpValue, RRF_RT_REG_SZ, nullptr, nullptr, &cbData);
		if (ret != ERROR_SUCCESS)
		{
			throw Exception(Translate("Main::ERROR_RegGetValueW@1", ret));
		}
		buffer.resize(cbData);
		ret = RegGetValueW(HKEY_CURRENT_USER, lpSubKey, lpValue, RRF_RT_REG_SZ, nullptr, buffer.data(), &cbData);
		if (ret != ERROR_SUCCESS)
		{
			throw Exception(Translate("Main::ERROR_RegGetValueW@1", ret));
		}
		buffer.resize(cbData - 1); /* cbData 包含了末尾空字符 */
	};

	if (Global::LaunchGameCmd.empty())
	{
		std::wstring buffer;
		Console::Info(Translate("Main::INFO_AutoDetectLaunchGameCmd"));
		query_registry_string(HKEY_CURRENT_USER, L"Software\\TCGame", L"setup", buffer);
		std::filesystem::path tcgame_path = std::filesystem::canonical(buffer) / L"TCGame.exe";
		Global::LaunchGameCmd = std::format(L"\"{}\" cso", tcgame_path.wstring());
	}
	if (Global::GameRootDirectory.empty())
	{
		std::wstring buffer;
		Console::Info(Translate("Main::INFO_AutoDetectGameRootDir"));
		query_registry_string(HKEY_CURRENT_USER, L"Software\\TCGame\\csol", L"gamepath", buffer);
		Global::GameRootDirectory = std::filesystem::canonical(buffer).wstring();
	}
	
	std::filesystem::path game_executable_path =
		std::filesystem::path(Global::GameRootDirectory) / L"Bin" / L"cstrike-online.exe";

	Console::Info(Translate("Main::INFO_GameRootDir@1", ConvertUtf16ToUtf8(Global::GameRootDirectory)));
	Console::Info(Translate("Main::INFO_LaunchGameCmd@1", ConvertUtf16ToUtf8(Global::LaunchGameCmd)));

	GameProcessInformation game_process_information {
		L"cstrike-online.exe",
		L"Counter-Strike Online",
        game_executable_path.wstring(),
		Global::LaunchGameCmd,
	};
	
	OCRBackboneInformation ocr_backbone_information {
		.DBNetPath = convert_to_absolute_path(Global::OCRDetectionModelPath),
		.CRNNPath = convert_to_absolute_path(Global::OCRRecognitionModelPath),
		.DictPath = convert_to_absolute_path(Global::OCRDictionaryPath),
		.KeywordsPath = convert_to_absolute_path(Global::OCRKeywordsPath)
	};

	auto idle_engine = std::make_unique<OCRIdleEngine>(std::move(game_process_information), ocr_backbone_information);
	
	auto command_dispatcher =
		std::make_unique<CommandDispatcher>(convert_to_absolute_path(Global::ExecutorCommandFilePath));

	auto driver_hotkey_bindings = std::make_unique<DriverHotkeyBindings>(DriverHotkeyBindings {
		.hkNULL = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '0'),
		.hkNormalIdle = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '1'),
		.hkExtendedIdle = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '2'),
		.hkBatchCombineParts = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '3'),
		.hkBatchPurchaseItem = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '4'),
		.hkLocateCursor = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '5'),
	});

	g_Driver = std::make_unique<Driver>(std::move(driver_hotkey_bindings), std::move(idle_engine), std::move(command_dispatcher));
	SetConsoleCtrlHandler([] (DWORD dwCtrlType) -> BOOL {
		g_Driver->Terminate();
		return TRUE;
	}, true);
	
	if (!Global::AllowQuickFullScreen)
	{
		LowLevelKeyboardHook disable_alt_enter([] (int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
			if (nCode == HC_ACTION)
			{
				KBDLLHOOKSTRUCT* pKeyboardLowLevelHookStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
				/* 先按 Alt 后按 Enter 或先按 Enter 后按 Alt 都会触发游戏全屏化，需要特殊处理 */
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)	
				{
					/* 先 Alt 后 Enter */
					if ((GetAsyncKeyState(VK_MENU) & 0x8000) && pKeyboardLowLevelHookStruct->vkCode == VK_RETURN)
					{
					#ifdef _DEBUG
						Console::Debug("屏蔽 { Alt Enter }。");
					#endif
						return 1;
					}
					/* 先 Enter 后 Alt */
					if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && (pKeyboardLowLevelHookStruct->vkCode == VK_LMENU || pKeyboardLowLevelHookStruct->vkCode == VK_RMENU))
					{
					#ifdef _DEBUG
						Console::Debug("屏蔽 { Alt Enter }。");
					#endif
						return 1;
					}
				}
			}
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		});
		g_Driver->RegisterLowLevelKeyboardHook(std::move(disable_alt_enter));
		Console::Info(Translate("Main::INFO_DisableQuickFullscreen"));
	}
	if (Global::SuppressCSOBanner)
	{
		auto cso_banner_path = std::filesystem::path(Global::GameRootDirectory) / L"Bin" / L"CSOBanner.exe";
		auto cso_banner_suppressor = std::make_unique<CSOBannerSuppressor>(cso_banner_path);
		g_Driver->RegisterOptionalModule(std::move(cso_banner_suppressor));
		Console::Info(Translate("Main::INFO_SuppressCSOBanner"));
	}
}
