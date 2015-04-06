#include "postprocessorwindowprojectdataitem.h"
#include "postprocessorwindow.h"
#include "../project/projectpostprocessors.h"

PostProcessorWindowProjectDataItem::~PostProcessorWindowProjectDataItem(){
	if (m_window != 0){
		PostProcessorWindow* w = m_window;
		m_window = 0;
		delete w;
	}
}

void PostProcessorWindowProjectDataItem::informWindowClose()
{
	if (m_window != 0){
		m_window = 0;
		dynamic_cast<ProjectPostProcessors*>(parent())->requestDelete(this);
	}
}
