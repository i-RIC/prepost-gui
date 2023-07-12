#ifndef SOLVERDEFINITIONGRIDOUTPUT_IMPL_H
#define SOLVERDEFINITIONGRIDOUTPUT_IMPL_H

#include "../solverdefinitiongridoutput.h"

class SolverDefinitionGridOutput::Impl
{
public:
	Impl(SolverDefinitionGridOutput* parent);
	~Impl();

	void load(const QDomElement& elem);

	Position m_position;

	ColorMapFactoryI* m_colorMapFactory;
	QDomElement m_defaultColorMapSetting;

	SolverDefinitionGridOutput* m_parent;
};

#endif // SOLVERDEFINITIONGRIDOUTPUT_IMPL_H
