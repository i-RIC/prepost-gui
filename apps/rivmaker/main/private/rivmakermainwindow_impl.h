#ifndef RIVMAKERMAINWINDOW_IMPL_H
#define RIVMAKERMAINWINDOW_IMPL_H

#include "../rivmakermainwindow.h"
#include "../../io/rivexporter.h"
#include "../../widgets/mousepositionwidget.h"
#include "../../widgets/valuewidget.h"
#include "../../window/preprocessor/preprocessorwindow.h"
#include "../../window/verticalcrosssection/verticalcrosssectionwindow.h"

#include <QSignalMapper>
#include <QString>
#include <QToolBar>

class Project;

class RivmakerMainWindow::Impl
{
public:
	Impl(RivmakerMainWindow* w);
	~Impl();

	PreProcessorWindow m_preProcessorWindow;
	VerticalCrossSectionWindow m_verticalCrossSectionWindow;

	QToolBar m_windowsToolBar;
	QSignalMapper m_windowActivationMapper;

	RivExporter m_rivExporter;

	ValueWidget m_valueWidget;
	MousePositionWidget m_mousePositionWidget;

	Project* m_project;

	QString m_lastFolder;
};

#endif // RIVMAKERMAINWINDOW_IMPL_H
