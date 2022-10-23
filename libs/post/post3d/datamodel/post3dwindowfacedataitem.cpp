#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowfacedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/datamodel/graphicswindowdataitemstandarditemchangecommand.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QXmlStreamWriter>
#include <QDomElement>
#include <QDomNode>
#include <QStandardItem>
#include <QUndoCommand>

#include <vtkGeometryFilter.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>

class Post3dWindowFaceDataItemChangeCommand : public QUndoCommand
{
public:
	Post3dWindowFaceDataItemChangeCommand(Post3dWindowFaceDataItem* item) :
		QUndoCommand {Post3dWindowFaceDataItem::tr("Object Browser Item Change")}
	{
		m_command = new GraphicsWindowDataItemStandardItemChangeCommand(item);
		m_item = item;
	}
	void redo() {
		m_item->setIsCommandExecuting(true);

		m_command->redoStandardItem();

		m_item->setIsCommandExecuting(false);
	}
	void undo() {
		m_item->setIsCommandExecuting(true);

		m_command->undoStandardItem();

		m_item->setIsCommandExecuting(false);
	}
private:
	GraphicsWindowDataItemStandardItemChangeCommand* m_command;

	Post3dWindowFaceDataItem* m_item;
};

Post3dWindowFaceDataItem::Post3dWindowFaceDataItem(const QString& label, GraphicsWindowDataItem* p) :
	Post3dWindowDataItem {label, QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_actor {nullptr},
	m_dataOK {false}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	m_filter = vtkSmartPointer<vtkExtractGrid>::New();
}

void Post3dWindowFaceDataItem::update()
{
	m_dataOK = false;
	if (m_actor != nullptr) {m_actor->VisibilityOff();}
	Post3dWindowZoneDataItem* zdi = getZoneDataItem();
	PostZoneDataContainer* cont = zdi->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* pd = cont->data()->data();
	if (pd == nullptr) {return;}
	m_dataOK = true;

	m_filter->SetInputData(pd);
}

Post3dWindowFaceDataItem::~Post3dWindowFaceDataItem()
{
	if (m_actor != nullptr) {
		renderer()->RemoveActor(m_actor);
	}
	m_isCommandExecuting = true;
	m_standardItem->setCheckState(Qt::Unchecked);
	m_isCommandExecuting = false;
}

void Post3dWindowFaceDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();

	Setting setting;
	setting.enabled = iRIC::getBooleanAttribute(node, "enabled", true);
	m_standardItem->setText(elem.attribute("name"));
	QString dir = elem.attribute("direction");
	if (dir == "I") {
		setting.direction = dirI;
	} else if (dir == "J") {
		setting.direction = dirJ;
	} else if (dir == "K") {
		setting.direction = dirK;
	}
	setting.iMin = iRIC::getIntAttribute(node, "IMin");
	setting.iMax = iRIC::getIntAttribute(node, "IMax");
	setting.jMin = iRIC::getIntAttribute(node, "JMin");
	setting.jMax = iRIC::getIntAttribute(node, "JMax");
	setting.kMin = iRIC::getIntAttribute(node, "KMin");
	setting.kMax = iRIC::getIntAttribute(node, "KMax");
	setSetting(setting, false);
}

void Post3dWindowFaceDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_standardItem->text());
	iRIC::setBooleanAttribute(writer, "enabled", m_standardItem->checkState() == Qt::Checked);
	QString dirStr;
	switch (m_direction) {
	case dirI:
		dirStr = "I";
		break;
	case dirJ:
		dirStr = "J";
		break;
	case dirK:
		dirStr = "K";
		break;
	default:
		dirStr = "I";
	}
	writer.writeAttribute("direction", dirStr);
	iRIC::setIntAttribute(writer, "IMin", m_iMin);
	iRIC::setIntAttribute(writer, "IMax", m_iMax);
	iRIC::setIntAttribute(writer, "JMin", m_jMin);
	iRIC::setIntAttribute(writer, "JMax", m_jMax);
	iRIC::setIntAttribute(writer, "KMin", m_kMin);
	iRIC::setIntAttribute(writer, "KMax", m_kMax);
}

Post3dWindowFaceDataItem::Setting Post3dWindowFaceDataItem::setting()
{
	Post3dWindowFaceDataItem::Setting s;
	s.enabled = (m_standardItem->checkState() == Qt::Checked);
	s.direction = m_direction;
	s.iMin = m_iMin;
	s.iMax = m_iMax;
	s.jMin = m_jMin;
	s.jMax = m_jMax;
	s.kMin = m_kMin;
	s.kMax = m_kMax;
	return s;
}

void Post3dWindowFaceDataItem::setSetting(Setting news, bool draw)
{
	m_isCommandExecuting = true;
	if (news.enabled) {
		m_standardItem->setCheckState(Qt::Checked);
	} else {
		m_standardItem->setCheckState(Qt::Unchecked);
	}
	m_standardItemCopy = m_standardItem->clone();
	m_direction = news.direction;
	m_iMin = news.iMin;
	m_iMax = news.iMax;
	m_jMin = news.jMin;
	m_jMax = news.jMax;
	m_kMin = news.kMin;
	m_kMax = news.kMax;
	updateFilter();
	if (draw) {
		renderGraphicsView();
	}
	m_isCommandExecuting = false;
}

bool Post3dWindowFaceDataItem::enabled() const
{
	return m_standardItem->checkState() == Qt::Checked;
}

void Post3dWindowFaceDataItem::setEnabled(bool e)
{
	m_standardItem->setCheckState(e ? Qt::Checked : Qt::Unchecked);

	delete m_standardItemCopy;
	m_standardItemCopy = m_standardItem->clone();
}

Post3dWindowFaceDataItem::Direction Post3dWindowFaceDataItem::direction() const
{
	return m_direction;
}

void Post3dWindowFaceDataItem::setDirection(Direction dir)
{
	m_direction = dir;
}

unsigned int Post3dWindowFaceDataItem::iMin() const
{
	return m_iMin;
}

unsigned int Post3dWindowFaceDataItem::iMax() const
{
	return m_iMax;
}

unsigned int Post3dWindowFaceDataItem::jMin() const
{
	return m_jMin;
}

unsigned int Post3dWindowFaceDataItem::jMax() const
{
	return m_jMax;
}

unsigned int Post3dWindowFaceDataItem::kMin() const
{
	return m_kMin;
}

unsigned int Post3dWindowFaceDataItem::kMax() const
{
	return m_kMax;
}

void Post3dWindowFaceDataItem::setIMin(unsigned int val)
{
	m_iMin = val;
}

void Post3dWindowFaceDataItem::setIMax(unsigned int val)
{
	m_iMax = val;
}

void Post3dWindowFaceDataItem::setJMin(unsigned int val)
{
	m_jMin = val;
}

void Post3dWindowFaceDataItem::setJMax(unsigned int val)
{
	m_jMax = val;
}

void Post3dWindowFaceDataItem::setKMin(unsigned int val)
{
	m_kMin = val;
}

void Post3dWindowFaceDataItem::setKMax(unsigned int val)
{
	m_kMax = val;
}

vtkExtractGrid* Post3dWindowFaceDataItem::filter() const
{
	return m_filter;
}

void Post3dWindowFaceDataItem::setActor(vtkActor* actor)
{
	if (m_actor != nullptr) {
		renderer()->RemoveActor(m_actor);
	}
	m_actor = actor;
	renderer()->AddActor(m_actor);
	actorCollection()->RemoveAllItems();
	actorCollection()->AddItem(m_actor);
	if (m_dataOK && m_standardItem->checkState() == Qt::Checked) {
		m_actor->VisibilityOn();
	} else {
		m_actor->VisibilityOff();
	}
}

vtkPolyData* Post3dWindowFaceDataItem::getOutputPolyData()
{
	auto geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geometryFilter->SetInputConnection(getOutputPort());
	geometryFilter->Update();

	vtkPolyData* output = geometryFilter->GetOutput();
	output->Register(nullptr);
	return output;
}

vtkAlgorithmOutput* Post3dWindowFaceDataItem::getOutputPort()
{
	return m_filter->GetOutputPort();
}

void Post3dWindowFaceDataItem::updateFilter()
{
	m_filter->SetVOI(m_iMin, m_iMax, m_jMin, m_jMax, m_kMin, m_kMax);
	m_filter->Modified();
}

Post3dWindowZoneDataItem* Post3dWindowFaceDataItem::getZoneDataItem()
{
	ProjectDataItem* target = this;
	Post3dWindowZoneDataItem* zdi = nullptr;
	while (zdi == nullptr) {
		target = target->parent();
		if (target == nullptr) {return nullptr;}

		zdi = dynamic_cast<Post3dWindowZoneDataItem*> (target);
	}
	return zdi;
}

void Post3dWindowFaceDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting == true) {return;}
	Post3dWindowContourGroupDataItem* p1 = dynamic_cast<Post3dWindowContourGroupDataItem*>(parent());
	if (p1 != nullptr) {GraphicsWindowDataItem::handleStandardItemChange();}
}

void Post3dWindowFaceDataItem::innerUpdateZScale(double scale)
{
	if (m_actor == nullptr) {return;}
	m_actor->SetScale(1, 1, scale);
}
