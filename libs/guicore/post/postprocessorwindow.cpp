#include "../project/projectdata.h"
#include "../project/projectmainfile.h"
#include "postprocessorwindow.h"
#include "postprocessorwindowprojectdataitem.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>

#include <QMdiSubWindow>

const int PostProcessorWindow::MINWIDTH = 120;
const int PostProcessorWindow::MINHEIGHT = 100;

PostProcessorWindow::PostProcessorWindow(QWidget* parent, int index, PostProcessorWindowProjectDataItem* pdi) :
	QMainWindowWithSnapshot {parent},
	m_projectDataItem {pdi},
	m_index {index}
{
	setMinimumSize(MINWIDTH, MINHEIGHT);
}

PostProcessorWindow::~PostProcessorWindow()
{
	m_projectDataItem->informWindowClose();
}

const QIcon& PostProcessorWindow::icon() const
{
	return m_icon;
}

int PostProcessorWindow::index() const
{
	return m_index;
}

void PostProcessorWindow::setIndex(int index)
{
	m_index = index;
}

void PostProcessorWindow::applyOffset(double /*x_diff*/, double /*y_diff*/)
{}

PostSolutionInfo* PostProcessorWindow::postSolutionInfo()
{
	return m_projectDataItem->projectData()->mainfile()->postSolutionInfo();
}

PreProcessorGeoDataGroupDataItemI* PostProcessorWindow::refDataGroupDataItem()
{
	auto preModel = postSolutionInfo()->iricMainWindow()->preProcessorWindow()->dataModel();
	if (preModel->geoDataTopDataItem() == nullptr) {
		// Post Only mode
		return nullptr;
	}
	return preModel->geoDataTopDataItem()->groupDataItem(std::string("_referenceinformation"));
}
