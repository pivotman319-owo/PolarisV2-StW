#pragma once

#include "peh_table.h"

namespace polaris
{
	class Program
	{
	public:
		tables::PehTable* m_pMainTable; //!< The main PEH (Process Event Hook) Table.

		Program();
		~Program();
	};
}

extern polaris::Program* gpProgram;
