#include "CCSOLUtility.hpp"

static CCSOLUtilitiy* m_singleton = nullptr;

void CCSOLUtilitiy::InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath)
{
}

CCSOLUtilitiy::CCSOLUtilitiy(std::string sGameRootPath, std::string sGameLaucherPath) :
m_sGameRootPath(sGameRootPath), m_sGameLauncherPath(sGameLaucherPath)
{

}