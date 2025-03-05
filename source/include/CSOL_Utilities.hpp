#pragma once

#include <Windows.h>
#include <memory>

namespace CSOL_Utilities
{
constexpr const char* DET_MODEL_FILE = "models/ch_PP-OCRv4_det_infer.onnx";
constexpr const char* CLS_MODEL_FILE = "models/ch_ppocr_mobile_v2.0_cls_infer_model.onnx";
constexpr const char* REC_MODEL_FILE = "models/ch_PP-OCRv4_rec_infer.onnx";
constexpr const char* KEY_FILE = "models/ppocr_keys_v1.txt";
constexpr const DWORD WM_GAME_PROCESS_EXIT = WM_APP; /* 用于报告游戏进程退出事件 */
enum class CSOL_UTILITIES_MESSAGE_LEVEL
{
    CUML_MESSAGE,
    CUML_WARNING,
    CUML_DEBUG,
    CUML_ERROR
};

enum class EXECUTOR_COMMAND
{
    CMD_NOP,
    CMD_START_GAME_ROOM,
    CMD_CHOOSE_CHARACTER,
    CMD_DEFAULT_IDLE,
    CMD_EXTENDED_IDLE,
    CMD_CONFIRM_RESULTS,
    CMD_CREATE_GAME_ROOM,
    CMD_BATCH_COMBINE_PARTS,
    CMD_BATCH_PURCHASE_ITEM,
    CMD_LOCATE_CURSOR,
    CMD_CLEAR_POPUPS
};
/* 游戏状态 */
enum class IN_GAME_STATE
{
    IGS_LOGIN,            /* 正在登陆 */
    IGS_IN_HALL,          /* 在大厅中 */
    IGS_IN_ROOM,          /* 在房间内 */
    IGS_LOADING,          /* 游戏场景正在加载 */
    IGS_IN_MAP,           /* 在游戏地图中 */
    IGS_UNKNOWN           /* 未知状态 */
};
enum class GAME_PROCESS_STATE
{
    GPS_BEING_CREATED, /* 游戏进程正在被创建 */
    GPS_RUNNING,       /* 游戏进程正在运行 */
    GPS_EXITED,        /* 游戏进程退出 */
    GPS_UNKNOWN,       /* 尚未确认游戏进程状态 */
};
using BITMAPBODY = uint8_t;
std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const char *string);
std::shared_ptr<char[]> ConvertUtf16ToUtf8(const wchar_t *string);
std::shared_ptr<wchar_t[]> QueryRegistryStringItem(HKEY hPredefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName);
BOOL IsRunningAsAdmin() noexcept;

/* 自动用于自动释放 Win32 handles */
template <typename ContentType = VOID, typename DeleterType = decltype(&CloseHandle)>
class UniqueHandle
{
public:
	friend void swap(UniqueHandle& uh1, UniqueHandle& uh2) noexcept
	{
		auto tmp = uh1.m_h;
		uh1.m_h = uh2.m_h;
		uh2.m_h = tmp;
	}
	UniqueHandle(ContentType* h = nullptr, DeleterType deleter = &CloseHandle) noexcept : m_h(h), m_Deleter(deleter) {}
	~UniqueHandle() noexcept
	{
		if (m_h != nullptr && m_h != INVALID_HANDLE_VALUE)
		{
			m_Deleter(m_h);
		}
	}
	UniqueHandle(const UniqueHandle&) = delete;
	UniqueHandle& operator=(const UniqueHandle&) = delete;
	UniqueHandle(UniqueHandle&& other) noexcept : m_h(nullptr)
	{
		swap(*this, other);
	}
	UniqueHandle& operator=(UniqueHandle&& other) noexcept
	{
		swap(*this, other);
		return *this;
	}
	explicit operator bool() const noexcept
	{
		return m_h != nullptr && m_h != INVALID_HANDLE_VALUE;
	}
	auto get() const noexcept
	{
		return m_h;
	}
	auto release() noexcept
	{
		auto temp = m_h;
		m_h = nullptr;
		return temp;
	}
	void reset(HANDLE h = nullptr) noexcept
	{
		if (m_h != nullptr && m_h != INVALID_HANDLE_VALUE)
		{
			m_Deleter(m_h);
		}
		m_h = h;
	}
private:
	ContentType* m_h = nullptr;
	DeleterType m_Deleter = &CloseHandle;
};
} // namespace CSOL_Utilities
