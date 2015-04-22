#include "post2dwindowarrowunstructuredsettingdialog.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
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

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(Post2dWindowDataItem* p)
	: Post2dWindowNodeVectorArrowGroupDataItem(p)
{
	m_samplingMode = smAll;
	m_samplingRate = 2;
	m_samplingNumber = 100;

	m_arrowMask = vtkSmartPointer<vtkMaskPoints>::New();
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::~Post2dWindowNodeVectorArrowGroupUnstructuredDataItem()
{

}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::updateActivePoints()
{
	m_activePoints->Initialize();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	outPoints->SetDataTypeToDouble();
	m_activePoints->SetPoints(outPoints);

	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	m_arrowMask->SetInputData(ps);
	if (m_samplingMode == smAll){
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	} else if (m_samplingMode == smRate){
		m_arrowMask->SetOnRatio(m_samplingRate);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	} else if (m_samplingMode == smNumber){
		m_arrowMask->RandomModeOn();
		m_arrowMask->SetMaximumNumberOfPoints(m_samplingNumber);
	} else{
		// default. same as smAll.
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(ps->GetNumberOfPoints());
	}
	m_arrowMask->Update();
	vtkPointSet* tmpgrid = m_arrowMask->GetOutput();

	vtkDataArray* da = tmpgrid->GetPointData()->GetArray(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
	vtkIntArray* IBCArray = 0;
	if (da != 0){
		IBCArray = vtkIntArray::SafeDownCast(da);
	}
	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(tmpgrid->GetPointData()->GetArray(iRIC::toStr(m_currentSolution).c_str()));
	if (vectorArray == 0){
		m_currentSolution = "";
		return;
	}
	QSet<vtkIdType> pointIds;
	double minlimitsqr = m_minimumValue * m_minimumValue;
	for (vtkIdType i = 0; i < tmpgrid->GetNumberOfPoints(); ++i){
		bool active = true;
		if (m_regionMode == StructuredGridRegion::rmActive && IBCArray->GetValue(i) == 0){
			active = false;
		}
		double val = 0;
		for (int j = 0; j < vectorArray->GetNumberOfComponents(); ++j){
			double tmpval = vectorArray->GetComponent(i, j);
			val += tmpval * tmpval;
		}
		if (val < minlimitsqr){
			active = false;
		}
		if (active){
			pointIds.insert(i);
		}
	}
	vtkPointData* inPD = tmpgrid->GetPointData();
	vtkPointData* outPD = m_activePoints->GetPointData();
	vtkPoints* inPoints = tmpgrid->GetPoints();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();

	outPD->CopyAllocate(inPD, pointIds.size());
	vtkIdType newId = 0;
	for (auto it = pointIds.begin(); it != pointIds.end(); ++it){
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
	if (cont == 0 || cont->data() == 0){
		return 0;
	}
	if (! cont->vectorValueExists()){
		return 0;
	}
	Post2dWindowArrowUnstructuredSettingDialog * dialog = new Post2dWindowArrowUnstructuredSettingDialog(p);
	dialog->setZoneData(cont);
	dialog->setMapping(m_mapping);
	dialog->setColor(m_color);
	dialog->setScalarValue(m_scalarValueName);
	dialog->setSamplingMode(m_samplingMode);
	dialog->setSamplingRate(m_samplingRate);
	dialog->setSamplingNumber(m_samplingNumber);
	if (! cont->IBCExists()){
		dialog->disableActive();
	}
	dialog->setRegionMode(m_regionMode);
	dialog->setLengthMode(m_lengthMode);
	dialog->setStandardValue(m_standardValue);
	dialog->setLegendLength(m_legendLength);
	dialog->setMinimumValue(m_minimumValue);
	dialog->setArrowSetting(m_arrowSetting);

	return dialog;
}

class Post2dWindowArrowUnstructuredSetProperty : public QUndoCommand
{
public:
	Post2dWindowArrowUnstructuredSetProperty(const QString& solutionName, Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping, const QColor& color, const QString& scalarName, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SamplingMode sm, int rate, int num, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::LengthMode lm, double stdLen, int legendLen, double minVal, const ArrowSettingContainer& arrowSetting, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item)
		: QUndoCommand(QObject::tr("Update Arrow Setting"))
	{
		m_newSolutionName = solutionName;
		m_newMapping = mapping;
		m_newColor = color;
		m_newScalarName = scalarName;
		m_newSamplingMode = sm;
		m_newSamplingRate = rate;
		m_newSamplingNumber = num;
		m_newLengthMode = lm;
		m_newStandardValue = stdLen;
		m_newLegendLength = legendLen;
		m_newMinimumValue = minVal;
		m_newArrowSetting = arrowSetting;

		m_oldSolutionName = item->m_currentSolution;
		m_oldMapping = item->m_mapping;
		m_oldColor = item->m_color;
		m_oldScalarName = item->m_scalarValueName;
		m_oldSamplingMode = item->m_samplingMode;
		m_oldSamplingRate = item->m_samplingRate;
		m_oldSamplingNumber = item->m_samplingNumber;
		m_oldLengthMode = item->m_lengthMode;
		m_oldStandardValue = item->m_standardValue;
		m_oldLegendLength = item->m_legendLength;
		m_oldMinimumValue = item->m_minimumValue;
		m_oldArrowSetting = item->m_arrowSetting;

		m_item = item;
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_newSolutionName);
		m_item->m_mapping = m_newMapping;
		m_item->m_color = m_newColor;
		m_item->m_scalarValueName = m_newScalarName;
		m_item->m_samplingMode = m_newSamplingMode;
		m_item->m_samplingRate = m_newSamplingRate;
		m_item->m_samplingNumber = m_newSamplingNumber;
		m_item->m_lengthMode = m_newLengthMode;
		m_item->m_standardValue = m_newStandardValue;
		m_item->m_legendLength = m_newLegendLength;
		m_item->m_minimumValue = m_newMinimumValue;
		m_item->m_arrowSetting = m_newArrowSetting;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_oldSolutionName);
		m_item->m_mapping = m_oldMapping;
		m_item->m_color = m_oldColor;
		m_item->m_scalarValueName = m_oldScalarName;
		m_item->m_samplingMode = m_oldSamplingMode;
		m_item->m_samplingRate = m_oldSamplingRate;
		m_item->m_samplingNumber = m_oldSamplingNumber;
		m_item->m_lengthMode = m_oldLengthMode;
		m_item->m_standardValue = m_oldStandardValue;
		m_item->m_legendLength = m_oldLegendLength;
		m_item->m_minimumValue = m_oldMinimumValue;
		m_item->m_arrowSetting = m_oldArrowSetting;

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_newSolutionName;
	Post2dWindowNodeVectorArrowGroupDataItem::Mapping m_newMapping;
	QColor m_newColor;
	QString m_newScalarName;
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SamplingMode m_newSamplingMode;
	int m_newSamplingRate;
	int m_newSamplingNumber;
	Post2dWindowNodeVectorArrowGroupDataItem::LengthMode m_newLengthMode;
	double m_newStandardValue;
	int m_newLegendLength;
	double m_newMinimumValue;
	ArrowSettingContainer m_newArrowSetting;

	QString m_oldSolutionName;
	Post2dWindowNodeVectorArrowGroupDataItem::Mapping m_oldMapping;
	QColor m_oldColor;
	QString m_oldScalarName;
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SamplingMode m_oldSamplingMode;
	int m_oldSamplingRate;
	int m_oldSamplingNumber;
	Post2dWindowNodeVectorArrowGroupDataItem::LengthMode m_oldLengthMode;
	double m_oldStandardValue;
	int m_oldLegendLength;
	double m_oldMinimumValue;
	ArrowSettingContainer m_oldArrowSetting;

	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* m_item;
};

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	Post2dWindowArrowUnstructuredSettingDialog* dialog = dynamic_cast<Post2dWindowArrowUnstructuredSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post2dWindowArrowUnstructuredSetProperty(dialog->solution(), dialog->mapping(), dialog->color(), dialog->scalarValue(), dialog->samplingMode(), dialog->samplingRate(), dialog->samplingNumber(), dialog->lengthMode(), dialog->standardValue(), dialog->legendLength(), dialog->minimumValue(), dialog->arrowSetting(), this));
}


void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(node);

	QDomElement elem = node.toElement();
	m_samplingMode = static_cast<SamplingMode>(elem.attribute("samplingMode").toInt());
	m_samplingRate = elem.attribute("samplingRate").toInt();
	m_samplingNumber = elem.attribute("samplingNumber").toInt();
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);

	writer.writeAttribute("samplingMode", QString::number(static_cast<int>(m_samplingMode)));
	writer.writeAttribute("samplingRate", QString::number(m_samplingRate));
	writer.writeAttribute("samplingNumber", QString::number(m_samplingNumber));
}

