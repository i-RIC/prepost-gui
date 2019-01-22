#include "post2dwindowcellflagdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QMouseEvent>
#include <QXmlStreamWriter>

#include <vtkActorCollection.h>
#include <vtkCellData.h>
#include <vtkIdList.h>
#include <vtkIntArray.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowCellFlagDataItem::Post2dWindowCellFlagDataItem(const std::string& attname, int val, const QColor& col, const QString& caption, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_attributeName (attname),
	m_value {val},
	m_color {col}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	setupActors();
}

Post2dWindowCellFlagDataItem::~Post2dWindowCellFlagDataItem()
{
	renderer()->RemoveActor(m_actor);
}

const std::string& Post2dWindowCellFlagDataItem::attributeName() const
{
	return m_attributeName;
}

int Post2dWindowCellFlagDataItem::value() const
{
	return m_value;
}

const QColor& Post2dWindowCellFlagDataItem::color() const
{
	return m_color;
}

void Post2dWindowCellFlagDataItem::setColor(const QColor& col)
{
	m_color = col;
	m_actor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
}

void Post2dWindowCellFlagDataItem::setOpacity(int o)
{
	m_actor->GetProperty()->SetOpacity(o / 100.);
}

void Post2dWindowCellFlagDataItem::setupActors()
{
	m_extractCells = vtkSmartPointer<vtkExtractCells>::New();

	m_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_mapper->SetInputConnection(m_extractCells->GetOutputPort());
	m_mapper->ScalarVisibilityOff();

	m_actor = vtkSmartPointer<vtkActor>::New();
	m_actor->SetMapper(m_mapper);
	m_actor->GetProperty()->SetColor(m_color.red() / 255., m_color.green() / 255., m_color.blue() / 255.);
	m_actor->VisibilityOff();
	renderer()->AddActor(m_actor);
	actorCollection()->AddItem(m_actor);

	update();
}

void Post2dWindowCellFlagDataItem::update()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* set = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->filteredData();
	if (set == nullptr) {return;}
	vtkIntArray* cellvals = vtkIntArray::SafeDownCast(set->GetCellData()->GetArray(m_attributeName.c_str()));
	if (cellvals == nullptr) {
		// no data available.
		return;
	}
	for (vtkIdType i = 0; i < set->GetNumberOfCells(); ++i) {
		int val = cellvals->GetValue(i);
		if (val == m_value) {
			idlist->InsertNextId(i);
		}
	}
	m_extractCells->SetInputData(set);
	m_extractCells->SetCellList(idlist);
	m_extractCells->Modified();

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowCellFlagDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowCellFlagDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}


void Post2dWindowCellFlagDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QColor col = loadColorAttribute("color", node, Qt::red);
	setColor(col);
}

void Post2dWindowCellFlagDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("attributeName", m_attributeName.c_str());
	iRIC::setIntAttribute(writer, "value", m_value);
	writeColorAttribute("color", m_color, writer);
}

void Post2dWindowCellFlagDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, scale, 1);
}

void Post2dWindowCellFlagDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initCellInputAttributeBrowser();
}

void Post2dWindowCellFlagDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearCellInputAttributeBrowser();
}

void Post2dWindowCellFlagDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateCellInputAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowCellFlagDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixCellInputAttributeBrowser(QPoint(event->x(), event->y()), v);
}
