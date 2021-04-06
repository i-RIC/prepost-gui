#include "post3dwindowcellcontourdataitem.h"
#include "post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QDomNode>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkExtractCells.h>
#include <vtkExtractGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

Post3dWindowCellContourDataItem::Post3dWindowCellContourDataItem(const QString& label, Post3dWindowDataItem* p) :
	Post3dWindowDataItem(label, QIcon(":/libs/guibase/images/iconPaper.png"), p),
	m_actor {vtkActor::New()},
	m_mapper {vtkPolyDataMapper::New()},
	m_polyData {vtkPolyData::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	m_actor->SetMapper(m_mapper);
	m_mapper->SetInputData(m_polyData);

	renderer()->AddActor(m_actor);
	actorCollection()->AddItem(m_actor);
}

Post3dWindowCellContourDataItem::~Post3dWindowCellContourDataItem()
{
	renderer()->RemoveActor(m_actor);

	m_actor->Delete();
	m_mapper->Delete();
	m_polyData->Delete();
}

const Post3dCellRangeSettingContainer& Post3dWindowCellContourDataItem::setting() const
{
	return m_setting;
}

void Post3dWindowCellContourDataItem::setSetting(const Post3dCellRangeSettingContainer& setting)
{
	m_setting = setting;
	updateActorSettings();
}

void Post3dWindowCellContourDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowCellContourDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_standardItem->setText(elem.attribute("caption", tr("Range")));

	m_setting.load(node);

	updateActorSettings();
}

void Post3dWindowCellContourDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("caption", m_standardItem->text());
	m_setting.save(writer);
}

void Post3dWindowCellContourDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, 1, scale);
}

void Post3dWindowCellContourDataItem::updateActorSettings()
{
	updatePolyData();
	updateColorSetting();
}

void Post3dWindowCellContourDataItem::updatePolyData()
{
	m_polyData->Initialize();
	auto zItem = dynamic_cast<Post3dWindowZoneDataItem*> (parent()->parent()->parent());
	if (zItem == nullptr) {return;}

	auto data = zItem->dataContainer()->data();
	if (data == nullptr) {return;}

	auto gItem = dynamic_cast<Post3dWindowCellContourGroupDataItem*> (parent());
	auto lut = gItem->lookupTable();

	auto grid = dynamic_cast<vtkStructuredGrid*> (data);
	auto extracted = m_setting.extractRegion(grid);
	auto extracted2 = gItem->setting().filterCellsWithUpperLower(extracted, *lut);
	auto gFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	gFilter->SetInputData(extracted2);
	gFilter->Update();

	m_polyData->DeepCopy(gFilter->GetOutput());
	extracted->Delete();
	extracted2->Delete();
}

void Post3dWindowCellContourDataItem::updateColorSetting()
{
	auto typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent()->parent());
	auto gi = dynamic_cast<Post3dWindowCellContourGroupDataItem*> (parent());

	auto target = iRIC::toStr(gi->setting().target);
	auto lookupTable = typedi->cellLookupTable(target);

	m_mapper->SetLookupTable(lookupTable->vtkObj());
	m_mapper->UseLookupTableScalarRangeOn();
	m_mapper->SetScalarModeToUseCellFieldData();
	m_mapper->SelectColorArray(target.c_str());
}
