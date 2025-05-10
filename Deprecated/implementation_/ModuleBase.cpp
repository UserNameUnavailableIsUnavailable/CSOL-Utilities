#include "ModuleBase.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>

using namespace CSOL_Utilities;

std::atomic_uint ModuleBase::s_id_pool = 0;

std::shared_ptr<ModuleFactory> ModuleFactory::create()
{
	return std::make_shared<ModuleFactory>(Private());
}

std::shared_ptr<ModuleBase> ModuleFactory::find_module(unsigned int id)
{
	std::lock_guard lock(s_module_table_lock);
	const auto& it = s_module_table.find(id);
	return it != s_module_table.end() ? it->second.lock() : nullptr;
}

void ModuleFactory::remove_module(unsigned int id)
{
	std::lock_guard lock(s_module_table_lock);
	const auto& it = s_module_table.find(id);
	if (it == s_module_table.end())
	{
		return;
	}
	if (const auto& module = it->second.lock())
	{
		module->m_manufacturer.reset();
	}
	s_module_table.erase(it);
}

ModuleBase::~ModuleBase()
{
	if (const auto& manufacturer = m_manufacturer.lock())
	{
		manufacturer->remove_module(m_id);
	}
}
