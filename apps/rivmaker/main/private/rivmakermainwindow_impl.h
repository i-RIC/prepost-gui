#ifndef RIVMAKERMAINWINDOW_IMPL_H
#define RIVMAKERMAINWINDOW_IMPL_H

#include "../rivmakermainwindow.h"
#include "../../window/preprocessor/preprocessorwindow.h"
#include "../../window/verticalcrosssection/verticalcrosssectionwindow.h"

class Project;

class RivmakerMainWindow::Impl
{
public:
	Impl(RivmakerMainWindow* w);
	~Impl();

	PreProcessorWindow m_preProcessorWindow;
	VerticalCrossSectionWindow m_verticalCrossSectionWindow;

	Project* m_project;
};

#endif // RIVMAKERMAINWINDOW_IMPL_H
