#include <Windows.h>
#include <cassert>
#include <csignal>
#include <filesystem>
#include <locale>
#include <memory>
#include <string>
#include <utility>
#include "CommandDispatcher.hpp"
#include "Driver.hpp"
#include "Event.hpp"
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

void InitializeCommandLineArgs(CLI::App& app, int argc, wchar_t* argv[])
{
	app.add_option("--locale-resources-directory", Global::g_LocaleResourcesDirectory);
	app.add_option("--language", Global::g_LocaleName);
	app.add_option("--game-root-directory", Global::g_GameRootDirectory);
	app.add_option("--launch-game-command", Global::g_LaunchGameCmd);
	app.add_option("--executor-command-file-path", Global::g_ExecutorCommandFilePath);
	app.add_option("--start-game-room-timeout", Global::g_StartGameRoomTimeout);
	app.add_option("--login-timeout", Global::g_LoginTimeout);
	app.add_option("--load-map-timeout", Global::g_LoadMapTimeout);
	app.add_option("--LGHUB-Agent-name", Global::g_LGHUB_Agent_Name);
	app.add_option("--detect-mode", Global::g_DetectMode);
	app.add_option("--OCR-detection-model-path", Global::g_OCRDetectionModelPath);
	app.add_option("--OCR-recognition-model-path", Global::g_OCRRecognitionModelPath);
	app.add_option("--OCR-dictionary-path", Global::g_OCRDictionaryPath);
	app.add_option("--OCR-keywords-path", Global::g_OCRKeywordsPath);
	app.add_option("--default-idle-after-reconnection", Global::g_DefaultIdleAfterReconnection);
	app.add_option("--restart-game-on-loading-timeout", Global::g_RestartGameOnLoadingTimeout);
	app.add_option("--allow-quick-fullscreen", Global::g_AllowQuickFullScreen);
}

void Boot()
{
#ifdef WIN32
	std::replace(Global::g_LocaleName.begin(), Global::g_LocaleName.end(), '_', '-');
	Global::g_LocaleName += L".UTF-8";
#endif
	std::locale locale(ConvertUtf16ToUtf8(Global::g_LocaleName));
	std::locale::global(locale);
	LoadLanguagePackage(Global::g_LanguagePackage);
#ifdef _DEBUG
	for (auto& [k, v] : Global::g_LanguagePackage)
	{
		Console::Debug(std::format("{} = {}", k, v));
	}
#endif

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
		DWORD cbData;
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
	};

	if (Global::g_LaunchGameCmd.empty())
	{
		std::wstring buffer;
		Console::Info(Translate("Main::INFO_AutoDetectLaunchGameCmd"));
		query_registry_string(HKEY_CURRENT_USER, L"Software\\TCGame", L"setup", buffer);
		std::filesystem::path tcgame_path = std::filesystem::canonical(buffer) / L"TCGame.exe";
		Global::g_LaunchGameCmd = std::format(L"\"{}\" cso", tcgame_path.wstring());
	}
	if (Global::g_GameRootDirectory.empty())
	{
		std::wstring buffer;
		Console::Info(Translate("Main::INFO_AutoDetectGameRootDir"));
		query_registry_string(HKEY_CURRENT_USER, L"Software\\TCGame\\csol", L"gamepath", buffer);
		Global::g_GameRootDirectory = std::filesystem::canonical(buffer).wstring();
	}
	
	std::filesystem::path game_executable_path =
		std::filesystem::path(Global::g_GameRootDirectory) / L"Bin" / L"cstrike-online.exe";

	Console::Info(Translate("Main::INFO_GameRootDir@1", ConvertUtf16ToUtf8(Global::g_GameRootDirectory)));
	Console::Info(Translate("Main::INFO_LaunchGameCmd@1", ConvertUtf16ToUtf8(Global::g_LaunchGameCmd)));

	GameProcessInformation game_process_information {
		L"cstrike-online.exe",
		L"Counter-Strike Online",
        game_executable_path.wstring(),
		Global::g_LaunchGameCmd,
	};
	
	OCRBackboneInformation ocr_backbone_information {
		.DBNetPath = convert_to_absolute_path(Global::g_OCRDetectionModelPath),
		.CRNNPath = convert_to_absolute_path(Global::g_OCRRecognitionModelPath),
		.DictPath = convert_to_absolute_path(Global::g_OCRDictionaryPath),
		.KeywordsPath = convert_to_absolute_path(Global::g_OCRKeywordsPath)
	};

	auto idle_engine = std::make_unique<OCRIdleEngine>(std::move(game_process_information), ocr_backbone_information);
	
	auto command_dispatcher =
		std::make_unique<CommandDispatcher>(convert_to_absolute_path(Global::g_ExecutorCommandFilePath));

	auto driver_hotkey_bindings = std::make_unique<DriverHotkeyBindings>(DriverHotkeyBindings {
		.hkNULL = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '0'),
		.hkNormalIdle = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '1'),
		.hkExtendedIdle = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '2'),
		.hkBatchCombineParts = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '3'),
		.hkBatchPurchaseItem = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '4'),
		.hkLocateCursor = HotKey(MOD_ALT | MOD_CONTROL | MOD_SHIFT, '5'),
	});

	g_Driver = std::make_unique<Driver>(std::move(idle_engine), std::move(command_dispatcher), std::move(driver_hotkey_bindings));
	SetConsoleCtrlHandler([] (DWORD dwCtrlType) -> BOOL {
		g_Driver->Terminate();
		return TRUE;
	}, true);
	
	if (!Global::g_AllowQuickFullScreen)
	{
		LowLevelKeyboardHook disable_alt_enter([] (int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
			if (nCode == HC_ACTION) {
				KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
				if ((GetAsyncKeyState(VK_MENU) & 0x8000) && kbStruct->vkCode == VK_RETURN) {
				#ifdef _DEBUG
					Console::Debug("屏蔽 { Alt Enter }。");
				#endif
					return 1;
				}
			}
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		});
		g_Driver->RegisterLowLevelKeyboardHook(std::move(disable_alt_enter));
	}
}

#ifndef Test__
/* 使用 wmain 作为入口，确保命令行参数以 UTF-16 传入 */
int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	using namespace CSOL_Utilities;
	CLI::App main_app{ "CSOL-Utilities" };
	InitializeCommandLineArgs(main_app, argc, argv);
	CLI11_PARSE(main_app, argc, argv)
	try
	{
		Boot();
		Console::Info(Translate("Main::ABOUT"));
		Console::Info(Translate("Main::NOTE"));
		Console::Info(Translate("Main::GITEE_URL"));
		Console::Info(Translate("Main::GITHUB_URL"));
		Console::Info(Translate("Main::HOW_TO_EXIT"));
		g_Driver->Launch();
	}
	catch (std::exception& e)
	{
		Console::Error(e.what());
		Console::Error(Translate("Main::ERROR_Panic"));
		std::getchar();
		return GetLastError();
	}
	g_Driver.reset(); /* 在结束进程前（调用 ExitProcess）析构 Driver */
}
#endif
