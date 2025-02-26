#include "post3dwindowfacecontourdataitem.h"
#include "post3dwindowfacecontourgroupdataitem.h"
#include "post3dwindowfacecontourgrouptopdataitem.h"

#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>

Post3dWindowFaceContourDataItem::Post3dWindowFaceContourDataItem(const QString& label, Post3dWindowDataItem* p) :
	Post3dWindowDataItem(label, QIcon(":/libs/guibase/images/iconPaper.svg"), p),
	m_actor {vtkActor::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor(m_actor);
	actorCollection()->AddItem(m_actor);
}

Post3dWindowFaceContourDataItem::~Post3dWindowFaceContourDataItem()
{
	renderer()->RemoveActor(m_actor);

	m_actor->Delete();
}

Post3dWindowCellRangeSettingContainer Post3dWindowFaceContourDataItem::setting() const
{
	auto s = m_setting;
	s.enabled.setValue(standardItem()->checkState() == Qt::Checked);

	return s;
}

void Post3dWindowFaceContourDataItem::setSetting(const Post3dWindowCellRangeSettingContainer& setting)
{
	m_setting = setting;
	m_isCommandExecuting = true;
	standardItem()->setCheckState(Qt::Checked);
	m_isCommandExecuting = false;
	updateActorSetting();
}

void Post3dWindowFaceContourDataItem::update()
{
	updateActorSetting();
}

Post3dWindowFaceContourGroupDataItem* Post3dWindowFaceContourDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowFaceContourGroupDataItem*> (parent());
}

void Post3dWindowFaceContourDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_standardItem->setText(elem.attribute("caption", tr("Range")));

	m_setting.load(node);

	updateActorSetting();
}

void Post3dWindowFaceContourDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("caption", m_standardItem->text());
	m_setting.save(writer);
}

void Post3dWindowFaceContourDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, 1, scale);
}

void Post3dWindowFaceContourDataItem::doUpdateActorSetting()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto cont = groupDataItem()->data();
	if (cont == nullptr) {return;}

	auto grid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid());
	vtkSmartPointer<vtkPolyData> polyData;

	const auto& s = m_setting;
	auto pos = groupDataItem()->topDataItem()->position();
	if (pos == v4SolutionGrid::Position::IFace) {
		polyData = grid->extractIFaceData(s.iMin.value(), s.iMax.value(), s.jMin.value(), s.jMax.value(), s.kMin.value(), s.kMax.value());
	} else if (pos == v4SolutionGrid::Position::JFace) {
		polyData = grid->extractJFaceData(s.iMin.value(), s.iMax.value(), s.jMin.value(), s.jMax.value(), s.kMin.value(), s.kMax.value());
	} else if (pos == v4SolutionGrid::Position::KFace) {
		polyData = grid->extractKFaceData(s.iMin.value(), s.iMax.value(), s.jMin.value(), s.jMax.value(), s.kMin.value(), s.kMax.value());
	}

	polyData->GetCellData()->SetActiveScalars(groupDataItem()->target().c_str());
	auto mapper = groupDataItem()->m_colorMapSetting->buildCellDataMapper(polyData, false);
	m_actor->SetMapper(mapper);
	mapper->Delete();
	m_actorCollection->AddItem(m_actor);
}

void Post3dWindowFaceContourDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post3dWindowFaceContourDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post3dWindowFaceContourDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post3dWindowFaceContourDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post3dWindowFaceContourDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

void Post3dWindowFaceContourDataItem::handleStandardItemChange()
{
	updateActorSetting();
	Post3dWindowDataItem::handleStandardItemChange();
}

bool Post3dWindowFaceContourDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

void Post3dWindowFaceContourDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	groupDataItem()->updateColorMapVisibility();
}
