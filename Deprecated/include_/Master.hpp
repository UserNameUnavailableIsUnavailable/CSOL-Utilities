#pragma once

#include <array>
#include <memory>
#include "Command.hpp"
#include "Module.hpp"
#include "Signal.hpp"

namespace CSOL_Utilities
{
	struct KeyBinding
	{
		UINT uModifiers = MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT;
		UINT uKey;
	};
	class Master : public Module
	{
	public:
		Master(std::string name);
		void bind(unsigned int uIndex, unsigned int uModifiers, unsigned int uKey);
		virtual void work() override;

	private:
		std::array<KeyBinding, 6> m_KeyBindings;
		std::shared_ptr<Command> m_cmd;
	};
} // namespace CSOL_Utilities
