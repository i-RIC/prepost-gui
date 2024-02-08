#include "postcrosssectioninternalwindow.h"
#include "postcrosssectionwindow.h"
#include "postcrosssectionwindowprojectdataitem.h"

#include <guibase/widget/itemselectingdialog.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QMessageBox>
#include <QXmlStreamWriter>

namespace {

struct ZoneInformation {
	ZoneInformation(PostSolutionInfo::Dimension dim, const std::string& n) :
		dimension {dim},
		name {n}
	{}

	PostSolutionInfo::Dimension dimension;
	std::string name;
};

} // namespace

PostCrosssectionInternalWindow::PostCrosssectionInternalWindow(QWidget* parent) :
	AbstractCrosssectionWindow(parent)
{}

bool PostCrosssectionInternalWindow::setupInitialSetting()
{
	auto info = window()->postSolutionInfo();

	std::vector<ZoneInformation> zones;
	std::vector<QString> items;

	// 2d
	for (const auto& c : info->v4ZoneContainers2D()) {
		// TODO: currently limited to structured grid
		if (dynamic_cast<v4Structured2dGrid*> (c->gridData()->grid()) == nullptr) {continue;}

		zones.push_back(ZoneInformation(PostSolutionInfo::dim2D, c->zoneName()));
		items.push_back(c->caption());
	}
	// 3d
	for (const auto& c : info->v4ZoneContainers3D()) {
		// TODO: currently limited to structured grid
		if (dynamic_cast<v4Structured2dGrid*> (c->gridData()->grid()) == nullptr) {continue;}

		zones.push_back(ZoneInformation(PostSolutionInfo::dim3D, c->zoneName()));
		items.push_back(c->caption());
	}
	if (items.size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("No data to draw available."));
		return false;
	} else if (zones.size() == 1) {
		auto z = zones.at(0);
		m_dimension = z.dimension;
		m_zoneName = z.name;
	} else {
		ItemSelectingDialog dialog(this);
		dialog.setWindowTitle(tr("Select target grid"));
		dialog.setMessage(tr("Please select the grid to visualize"));
		dialog.setItems(items);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {return false;}

		auto selected = zones.at(dialog.selectedIndex());
		m_dimension = selected.dimension;
		m_zoneName = selected.name;
	}

	setupDisplaySettings();
	setTarget(AbstractCrosssectionWindow::Direction::I, 0);

	return true;
}

void PostCrosssectionInternalWindow::loadFromProjectMainFile(const QDomNode& node)
{
	auto elem = node.toElement();
	m_dimension = static_cast<PostSolutionInfo::Dimension> (iRIC::getIntAttribute(node, "dimension"));
	m_zoneName = iRIC::toStr(elem.attribute("zoneName"));

	setupDisplaySettings();

	AbstractCrosssectionWindow::loadFromProjectMainFile(node);
}

void PostCrosssectionInternalWindow::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setIntAttribute(writer, "dimension", static_cast<int> (m_dimension));
	writer.writeAttribute("zoneName", m_zoneName.c_str());

	AbstractCrosssectionWindow::saveToProjectMainFile(writer);
}

v4Structured2dGrid* PostCrosssectionInternalWindow::grid()
{
	auto info = window()->postSolutionInfo();
	v4PostZoneDataContainer* container = nullptr;

	if (m_dimension == PostSolutionInfo::Dimension::dim2D) {
		container = info->v4ZoneContainer2D(m_zoneName);
	} else if (m_dimension == PostSolutionInfo::Dimension::dim3D) {
		container = info->v4ZoneContainer3D(m_zoneName);
	}

	if (container == nullptr) {return nullptr;}

	return dynamic_cast<v4Structured2dGrid*> (container->gridData()->grid());
}


v4Structured2dGrid* PostCrosssectionInternalWindow::additionalGrid()
{
	auto info = window()->postSolutionInfo();
	v4PostZoneDataContainer* container = nullptr;

	if (m_dimension == PostSolutionInfo::Dimension::dim2D) {
		container = info->v4ZoneContainer2D(m_zoneName);
	} else if (m_dimension == PostSolutionInfo::Dimension::dim3D) {
		container = info->v4ZoneContainer3D(m_zoneName);
	}

	if (container == nullptr) {return nullptr;}

	return dynamic_cast<v4Structured2dGrid*> (container->inputGridData()->grid());
}

QString PostCrosssectionInternalWindow::additionalGridPrefix()
{
	return tr("(Input) ");
}

PostCrosssectionWindow* PostCrosssectionInternalWindow::window() const
{
	return dynamic_cast<PostCrosssectionWindow*> (parentWidget());
}
