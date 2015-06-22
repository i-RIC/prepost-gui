#ifndef PREPROCESSORGEODATATOPDATAITEM_H
#define PREPROCESSORGEODATATOPDATAITEM_H

#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include "preprocessorgridtypedataitem.h"
#include <guibase/vtklegendboxwidget.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkScalarBarWidget.h>
#include <vtkBoxWidget.h>
#include <vtkLegendBoxActor.h>
#include <vtkTextMapper.h>

#include <QMap>

class PreProcessorGeoDataGroupDataItem;
class SolverDefinitionGridAttribute;

class PreProcessorGeoDataTopDataItem : public PreProcessorGeoDataTopDataItemInterface
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGeoDataTopDataItem(PreProcessorDataItem* parent);
	/// Destructor
	~PreProcessorGeoDataTopDataItem();
	SolverDefinitionGridType* gridType() override {
		return dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent())->gridType();
	}
	const QList<PreProcessorGeoDataGroupDataItemInterface*> groupDataItems() const override;
	PreProcessorGeoDataGroupDataItemInterface* groupDataItem(const QString& name) override;
	void informValueRangeChange(const QString& name);
	void informDataChange();
	void setupActors();
	void updateActorSettings();
	void updateLegendBoxItems();
	QStringList getGeoDatasNotMapped();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void saveToCgnsFile(const int fn) override;
	/// Set all dimensions to the first.
	void setDimensionsToFirst() override;

public slots:
	void setupScalarBar();

signals:
	void valueRangeChanged(const QString& name);
	void dataChanged();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QMap<QString, PreProcessorGeoDataGroupDataItemInterface*> m_itemNameMap;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	vtkSmartPointer<vtkLegendBoxWidget> m_legendBoxWidget;
	bool m_visible;
	SolverDefinitionGridAttribute* m_condition;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;
};

#endif // PREPROCESSORGEODATATOPDATAITEM_H
