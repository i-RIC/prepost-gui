#include "../project/projectpostprocessors.h"
#include "postprocessorwindow.h"
#include "postprocessorwindowprojectdataitem.h"

PostProcessorWindowProjectDataItem::~PostProcessorWindowProjectDataItem()
{
	if (m_window != nullptr) {
		PostProcessorWindow* w = m_window;
		m_window = nullptr;
		delete w;
	}
}

void PostProcessorWindowProjectDataItem::informWindowClose()
{
	if (m_window != nullptr) {
		m_window = nullptr;
		dynamic_cast<ProjectPostProcessors*>(parent())->requestDelete(this);
	}
}
