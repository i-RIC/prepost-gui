#include "post2dwindowarrowunstructuredsettingdialog.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QSet>
#include <QUndoCommand>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkVertex.h>

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting::Setting() :
	CompositeContainer ({&samplingMode, &samplingRate, &samplingNumber}),
	samplingMode {"samplingMode", smAll},
	samplingRate {"samplingRate", 2},
	samplingNumber {"samplingNumber", 100}
{}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting& Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem(p)
{
	m_arrowMask = vtkSmartPointer<vtkMaskPoints>::New();
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::~Post2dWindowNodeVectorArrowGroupUnstructuredDataItem()
{}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::updateActivePoints()
{
	auto& s = m_arrowSetting;
	m_activePoints->Initialize();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();
	m_activePoints->SetPoints(outPoints);

	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	m_arrowMask->SetInputData(ps);

	if (s.samplingMode == ArrowSettingContainer::SamplingMode::All) {
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	} else if (s.samplingMode == ArrowSettingContainer::SamplingMode::Rate) {
		m_arrowMask->SetOnRatio(s.samplingRate);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	} else if (s.samplingMode == ArrowSettingContainer::SamplingMode::Number) {
		m_arrowMask->RandomModeOn();
		m_arrowMask->SetMaximumNumberOfPoints(s.samplingNumber);
	} else {
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	}
	m_arrowMask->Update();
	vtkPointSet* tmpgrid = m_arrowMask->GetOutput();

	vtkDataArray* da = tmpgrid->GetPointData()->GetArray(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
	vtkIntArray* IBCArray = nullptr;
	if (da != nullptr) {
		IBCArray = vtkIntArray::SafeDownCast(da);
	}
	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(tmpgrid->GetPointData()->GetArray(m_setting.target));
	if (vectorArray == nullptr) {
		m_setting.target = "";
		return;
	}
	QSet<vtkIdType> pointIds;
	double min = m_setting.minimumValue;
	double minlimitsqr = min * min;
	for (vtkIdType i = 0; i < tmpgrid->GetNumberOfPoints(); ++i) {
		bool active = true;
		if (m_setting.regionMode == StructuredGridRegion::rmActive && IBCArray->GetValue(i) == 0) {
			active = false;
		}
		double val = 0;
		for (int j = 0; j < vectorArray->GetNumberOfComponents(); ++j) {
			double tmpval = vectorArray->GetComponent(i, j);
			val += tmpval * tmpval;
		}
		if (val < minlimitsqr) {
			active = false;
		}
		if (active) {
			pointIds.insert(i);
		}
	}
	vtkPointData* inPD = tmpgrid->GetPointData();
	vtkPointData* outPD = m_activePoints->GetPointData();
	vtkPoints* inPoints = tmpgrid->GetPoints();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();

	outPD->CopyAllocate(inPD, pointIds.size());
	vtkIdType newId = 0;
	for (auto it = pointIds.begin(); it != pointIds.end(); ++it) {
		vtkIdType pointid = *it;
		outPoints->InsertNextPoint(inPoints->GetPoint(pointid));
		outPD->CopyData(inPD, pointid, newId);
		ca->InsertNextCell(1, &newId);
		++ newId;
	}
	m_activePoints->SetVerts(ca);
	m_activePoints->Modified();
}

QDialog* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		return nullptr;
	}
	if (! cont->vectorValueExists()) {
		return nullptr;
	}
	Post2dWindowArrowUnstructuredSettingDialog* dialog = new Post2dWindowArrowUnstructuredSettingDialog(p);
	dialog->setZoneData(cont);
	if (! cont->IBCExists()) {
		dialog->disableActive();
	}
	dialog->setSettings(m_setting, m_unsSetting);

	return dialog;
}

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting& unss, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item) :
		QUndoCommand {QObject::tr("Update Arrow Setting")},
		m_newSetting {s},
		m_newUnsSetting {unss},
		m_oldSetting {item->m_setting},
		m_oldUnsSetting {item->m_unsSetting},
		m_item {item}
	{}
	void redo() override {
		m_item->m_setting = m_newSetting;
		m_item->m_unsSetting = m_newUnsSetting;
		m_item->setTarget(m_newSetting.target);
	}
	void undo() override {
		m_item->m_setting = m_oldSetting;
		m_item->m_unsSetting = m_oldUnsSetting;
		m_item->setTarget(m_oldSetting.target);
	}

private:
	Post2dWindowNodeVectorArrowGroupDataItem::Setting m_newSetting;
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting m_newUnsSetting;

	Post2dWindowNodeVectorArrowGroupDataItem::Setting m_oldSetting;
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting m_oldUnsSetting;

	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* m_item;
};

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowArrowUnstructuredSettingDialog* dialog = dynamic_cast<Post2dWindowArrowUnstructuredSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), dialog->unsSetting(), this), this, true);
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(node);
	m_unsSetting.load(node);
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);
	m_unsSetting.save(writer);
}
