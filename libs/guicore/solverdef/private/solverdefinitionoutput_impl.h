#ifndef SOLVERDEFINITIONOUTPUT_IMPL_H
#define SOLVERDEFINITIONOUTPUT_IMPL_H

#include "../solverdefinitionoutput.h"

class SolverDefinitionOutput::Impl
{
public:
	std::string m_name;

	QString m_caption;
	QString m_toolTip;
	std::string m_englishCaption;
};

#endif // SOLVERDEFINITIONOUTPUT_IMPL_H
