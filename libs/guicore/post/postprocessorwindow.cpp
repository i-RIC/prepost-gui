#include "../project/projectdata.h"
#include "../project/projectmainfile.h"
#include "postprocessorwindow.h"
#include "postprocessorwindowprojectdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>

#include <QMdiSubWindow>

const int PostProcessorWindow::MINWIDTH = 120;
const int PostProcessorWindow::MINHEIGHT = 100;

PostProcessorWindow::PostProcessorWindow(QWidget* parent, int index, PostProcessorWindowProjectDataItem* pdi)
	: QMainWindow(parent)
{
	m_projectDataItem = pdi;
	m_index = index;
	setMinimumSize(MINWIDTH, MINHEIGHT);
}

PostProcessorWindow::~PostProcessorWindow()
{
	m_projectDataItem->informWindowClose();
}

PostSolutionInfo* PostProcessorWindow::postSolutionInfo()
{
	return m_projectDataItem->projectData()->mainfile()->postSolutionInfo();
}

PreProcessorGeoDataGroupDataItemInterface* PostProcessorWindow::refDataGroupDataItem()
{
	auto preModel = postSolutionInfo()->iricMainWindow()->preProcessorWindow()->dataModel();
	return preModel->geoDataTopDataItem()->groupDataItem(std::string("_referenceinformation"));
}
