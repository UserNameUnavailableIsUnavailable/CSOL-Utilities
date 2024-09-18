#include "CCSOLUtility.hpp"

static CCSOLUtilitiy* m_singleton = nullptr;

CCSOLUtilitiy::CCSOLUtilitiy(std::string sGameRootPath, std::string sGameLaucherPath) :
m_sGameRootPath(sGameRootPath), m_sGameLauncherPath(sGameLaucherPath)
{

}