#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowisosurfacesettingdialog.h"
#include "post3dwindownodescalardataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QList>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkTextProperty.h>

Post3dWindowNodeScalarGroupDataItem::Post3dWindowNodeScalarGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Isosurface"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	setDefaultValues();
	setupActors();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())) {
		auto item = new Post3dWindowNodeScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
}

Post3dWindowNodeScalarGroupDataItem::~Post3dWindowNodeScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_isoSurfaceActor);
}

void Post3dWindowNodeScalarGroupDataItem::setDefaultValues()
{
	m_isoValue = 0.0;
	m_fullRange = true;
	m_color = Qt::white;
}

void Post3dWindowNodeScalarGroupDataItem::updateActorSettings()
{
	m_isoSurfaceActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (m_target == "") {return;}

	// update current active scalar
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupIsosurfaceSetting();
	updateColorSetting();

	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	setTarget(iRIC::toStr(elem.attribute("solution")));
	m_fullRange = iRIC::getBooleanAttribute(node, "fullRange", true);
	m_range.iMin = iRIC::getIntAttribute(node, "iMin");
	m_range.iMax = iRIC::getIntAttribute(node, "iMax");
	m_range.jMin = iRIC::getIntAttribute(node, "jMin");
	m_range.jMax = iRIC::getIntAttribute(node, "jMax");
	m_range.kMin = iRIC::getIntAttribute(node, "kMin");
	m_range.kMax = iRIC::getIntAttribute(node, "kMax");
	m_isoValue = iRIC::getDoubleAttribute(node, "value");
	m_color = iRIC::getColorAttribute(node, "color", Qt::white);
	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_target.c_str());
	iRIC::setBooleanAttribute(writer, "fullRange", m_fullRange);
	iRIC::setIntAttribute(writer, "iMin", m_range.iMin);
	iRIC::setIntAttribute(writer, "iMax", m_range.iMax);
	iRIC::setIntAttribute(writer, "jMin", m_range.jMin);
	iRIC::setIntAttribute(writer, "jMax", m_range.jMax);
	iRIC::setIntAttribute(writer, "kMin", m_range.kMin);
	iRIC::setIntAttribute(writer, "kMax", m_range.kMax);
	iRIC::setDoubleAttribute(writer, "value", m_isoValue);
	iRIC::setColorAttribute(writer, "color", m_color);
}

void Post3dWindowNodeScalarGroupDataItem::setupActors()
{
	// Mapper for graphical primitives
	m_isoSurfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_isoSurfaceMapper->ScalarVisibilityOff();

	// Create an actor for the Isosurface
	m_isoSurfaceActor = vtkSmartPointer<vtkActor>::New();
	m_isoSurfaceActor->SetMapper(m_isoSurfaceMapper);

	// Visualize
	renderer()->AddActor(m_isoSurfaceActor);

	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post3dWindowNodeScalarGroupDataItem::assignActorZValues(const ZDepthRange& /*range*/)
{

}

void Post3dWindowNodeScalarGroupDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::setupIsosurfaceSetting()
{
	// input data
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointSet* ps = cont->data();

	// extract interest volume
	vtkSmartPointer<vtkExtractGrid> voi = vtkSmartPointer<vtkExtractGrid>::New();
	voi->SetInputData(ps);
	voi->SetVOI(
		m_range.iMin, m_range.iMax,
		m_range.jMin, m_range.jMax,
		m_range.kMin, m_range.kMax);
	voi->Update();

	// Create the isosurface
	vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
	contourFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, m_target.c_str());
	contourFilter->SetInputConnection(voi->GetOutputPort());
	contourFilter->GenerateValues(1, m_isoValue, m_isoValue);

	// Map the data to graphical primitives
	m_isoSurfaceMapper->SetInputConnection(contourFilter->GetOutputPort());
	this->updateColorSetting();

	// set color
	if (vtkProperty* p = m_isoSurfaceActor->GetProperty()) {
		p->SetColor(m_color.red() / 255.0, m_color.green() / 255.0, m_color.blue() / 255.0);
	}

	m_actorCollection->AddItem(m_isoSurfaceActor);
}

void Post3dWindowNodeScalarGroupDataItem::updateVisibility()
{
	bool ancientVisible = isAncientChecked();
	updateVisibility(ancientVisible);

	renderGraphicsView();
}

void Post3dWindowNodeScalarGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);
}

QDialog* Post3dWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowIsosurfaceSettingDialog* dialog = new Post3dWindowIsosurfaceSettingDialog(p);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent());

	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {return nullptr;}

	dialog->setEnabled(true);
	dialog->setZoneData(zItem->dataContainer());
	dialog->setTarget(m_target);

	// it's made enabled ALWAYS.
	//	dialog->setEnabled(isEnabled());
	dialog->setFullRange(m_fullRange);
	dialog->setRange(m_range);

	dialog->setIsoValue(m_isoValue);
	dialog->setColor(this->m_color);

	dialog->setColor(m_color);

	return dialog;
}

class Post3dWindowIsosurfaceSetProperty : public QUndoCommand
{
public:
	Post3dWindowIsosurfaceSetProperty(
		bool enabled, const std::string& sol,
		bool fullrange, StructuredGridRegion::Range3d range,
		double isovalue, const QColor& color, Post3dWindowNodeScalarGroupDataItem* item)
		: QUndoCommand(QObject::tr("Update Contour Setting")) {
		m_newEnabled = enabled;
		m_newCurrentSolution = sol;
		m_newFullRange = fullrange;
		m_newRange = range;
		m_newIsoValue = isovalue;
		m_newColor = color;

		m_oldEnabled = item->isEnabled();
		m_oldCurrentSolution = item->m_target;
		m_oldFullRange = item->m_fullRange;
		m_oldRange = item->m_range;
		m_oldIsoValue = item->m_isoValue;
		m_oldColor = item->m_color;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->setTarget(m_oldCurrentSolution);
		m_item->m_fullRange = m_oldFullRange;
		m_item->m_range = m_oldRange;
		m_item->m_isoValue = m_oldIsoValue;
		m_item->m_color = m_oldColor;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->setTarget(m_newCurrentSolution);
		m_item->m_fullRange = m_newFullRange;
		m_item->m_range = m_newRange;
		m_item->m_isoValue = m_newIsoValue;
		m_item->m_color = m_newColor;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	bool m_oldEnabled;
	std::string m_oldCurrentSolution;
	bool m_oldFullRange;
	StructuredGridRegion::Range3d m_oldRange;
	double m_oldIsoValue;
	QColor m_oldColor;

	bool m_newEnabled;
	std::string m_newCurrentSolution;
	bool m_newFullRange;
	StructuredGridRegion::Range3d m_newRange;
	double m_newIsoValue;
	QColor m_newColor;

	Post3dWindowNodeScalarGroupDataItem* m_item;
};

void Post3dWindowNodeScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post3dWindowIsosurfaceSettingDialog* dialog = dynamic_cast<Post3dWindowIsosurfaceSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(
		new Post3dWindowIsosurfaceSetProperty(
			dialog->enabled(), dialog->target(),
			dialog->fullRange(), dialog->range(),
			dialog->isoValue(), dialog->color(), this));
}

void Post3dWindowNodeScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Isosurface physical value change"));
	pushRenderCommand(cmd, this, true);
}

std::string Post3dWindowNodeScalarGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowNodeScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSettings();
}

void Post3dWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_isoSurfaceActor->SetScale(1, 1, scale);
}

void Post3dWindowNodeScalarGroupDataItem::updateColorSetting()
{
	this->m_isoSurfaceActor->GetProperty()->SetColor(m_color.red()/255., m_color.green()/255., m_color.blue()/255.);
}
