#ifndef POSTPROCESSORWINDOWPROJECTDATAITEM_H
#define POSTPROCESSORWINDOWPROJECTDATAITEM_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

class PostProcessorWindow;
class QWidget;

class GUICOREDLL_EXPORT PostProcessorWindowProjectDataItem : public ProjectDataItem
{
public:
	PostProcessorWindowProjectDataItem(ProjectDataItem* parent, QWidget* /*parentWindow*/)
		: ProjectDataItem(parent)
	{
		m_window = nullptr;
	}
	~PostProcessorWindowProjectDataItem();
	void informWindowClose();
	PostProcessorWindow* window(){
		return m_window;
	}

protected:
	PostProcessorWindow* m_window;

public:
	friend class PostProcessorWindow;
};

#endif // POSTPROCESSORWINDOWPROJECTDATAITEM_H
