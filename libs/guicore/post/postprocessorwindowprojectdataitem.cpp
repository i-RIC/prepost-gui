#include "../project/projectpostprocessors.h"
#include "postprocessorwindow.h"
#include "postprocessorwindowprojectdataitem.h"

PostProcessorWindowProjectDataItem::PostProcessorWindowProjectDataItem(ProjectDataItem* parent, QWidget* /*parentWindow*/) :
	ProjectDataItem(parent),
	m_window {nullptr}
{}

PostProcessorWindowProjectDataItem::~PostProcessorWindowProjectDataItem()
{
	if (m_window != nullptr) {
		PostProcessorWindow* w = m_window;
		m_window = nullptr;
		delete w->parentWidget();
	}
}

void PostProcessorWindowProjectDataItem::informWindowClose()
{
	if (m_window != nullptr) {
		m_window = nullptr;
		dynamic_cast<ProjectPostProcessors*>(parent())->requestDelete(this);
	}
}

PostProcessorWindow* PostProcessorWindowProjectDataItem::window() const {
	return m_window;
}
