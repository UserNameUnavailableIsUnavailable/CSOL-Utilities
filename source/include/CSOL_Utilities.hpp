#pragma once

#include <Windows.h>
#include <memory>

namespace CSOL_Utilities
{
    class UniqueHandle {
    public:
        friend void swap(UniqueHandle& uh1, UniqueHandle& uh2) noexcept
        {
            auto tmp = uh1.m_h;
            uh1.m_h = uh2.m_h;
            uh2.m_h = tmp;
        }
        UniqueHandle() noexcept : m_h(nullptr) {}
        UniqueHandle(HANDLE h) noexcept : m_h(h) {}
        ~UniqueHandle() noexcept
        {
            if (m_h != nullptr && m_h != INVALID_HANDLE_VALUE)
            {
                CloseHandle(m_h);
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
        HANDLE get() const noexcept
        {
            return m_h;
        }
        HANDLE release() noexcept
        {
            HANDLE temp = m_h;
            m_h = nullptr;
            return temp;
        }
        void reset(HANDLE h = nullptr) noexcept
        {
            if (m_h != nullptr && m_h != INVALID_HANDLE_VALUE)
            {
                CloseHandle(m_h);
            }
            m_h = h;
        }

    private:
        HANDLE m_h;
};
enum class EXECUTOR_COMMAND
{
    CMD_NOP,
    CMD_START_GAME_ROOM,
    CMD_CHOOSE_CLASS,
    CMD_PLAY_GAME_NORMAL,
    CMD_PLAY_GAME_EXTEND,
    CMD_TRY_CONFIRM_RESULT,
    CMD_CREATE_GAME_ROOM,
    CMD_COMBINE_PARTS,
    CMD_PURCHASE_ITEM,
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
enum class CONSOLE_LOG_LEVEL
{
    CLL_MESSAGE,
    CLL_WARNING,
    CLL_DEBUG,
    CLL_ERROR
};
constexpr const char* DET_MODEL_FILE = "models/ch_PP-OCRv4_det_infer.onnx";
constexpr const char* CLS_MODEL_FILE = "models/ch_ppocr_mobile_v2.0_cls_infer_model.onnx";
constexpr const char* REC_MODEL_FILE = "models/ch_PP-OCRv4_rec_infer.onnx";
constexpr const char* KEY_FILE = "models/ppocr_keys_v1.txt";
BOOL CaptureWindowAsBmpW(const wchar_t* filename, HWND hWnd);
std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const char *string);
std::shared_ptr<char[]> ConvertUtf16ToUtf8(const wchar_t *string);
std::shared_ptr<wchar_t[]> QueryRegistryStringItem(HKEY hPredefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName);
BOOL IsRunningAsAdmin() noexcept;
} // namespace CSOL_Utilities