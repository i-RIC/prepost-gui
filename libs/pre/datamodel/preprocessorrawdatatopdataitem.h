#ifndef PREPROCESSORRAWDATATOPDATAITEM_H
#define PREPROCESSORRAWDATATOPDATAITEM_H

#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include "preprocessorgridtypedataitem.h"
#include <guibase/vtklegendboxwidget.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkScalarBarWidget.h>
#include <vtkBoxWidget.h>
#include <vtkLegendBoxActor.h>
#include <vtkTextMapper.h>

#include <QMap>

class PreProcessorRawDataGroupDataItem;
class SolverDefinitionGridRelatedCondition;

class PreProcessorRawDataTopDataItem : public PreProcessorRawDataTopDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorRawDataTopDataItem(PreProcessorDataItem* parent);
	/// Destructor
	~PreProcessorRawDataTopDataItem();
	SolverDefinitionGridType* gridType(){
		return dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent())->gridType();
	}
	const QList<PreProcessorRawDataGroupDataItemInterface*> groupDataItems() const;
	PreProcessorRawDataGroupDataItemInterface* groupDataItem(const QString& name);
	void informValueRangeChange(const QString& name);
	void informDataChange();
	void setupActors();
	void updateActorSettings();
	void updateLegendBoxItems();
	QStringList getRawDatasNotMapped();
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void saveToCgnsFile(const int fn);
	/// Set all dimensions to the first.
	void setDimensionsToFirst();

public slots:
	void setupScalarBar();

signals:
	void valueRangeChanged(const QString& name);
	void dataChanged();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

	QMap<QString, PreProcessorRawDataGroupDataItemInterface*> m_itemNameMap;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	vtkSmartPointer<vtkLegendBoxWidget> m_legendBoxWidget;
	bool m_visible;
	SolverDefinitionGridRelatedCondition* m_condition;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;
};

#endif // PREPROCESSORRAWDATATOPDATAITEM_H
