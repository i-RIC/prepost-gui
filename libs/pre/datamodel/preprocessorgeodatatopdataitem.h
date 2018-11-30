#ifndef PREPROCESSORGEODATATOPDATAITEM_H
#define PREPROCESSORGEODATATOPDATAITEM_H

#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include "preprocessorgridtypedataitem.h"
#include <guibase/vtklegendboxwidget.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/boolcontainer.h>

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
	PreProcessorGeoDataGroupDataItemInterface* groupDataItem(const std::string& name) override;
	void informValueRangeChange(const std::string& name);
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
	void valueRangeChanged(const std::string& name);
	void dataChanged();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void buildReferenceInformationAttribute();

	std::map<std::string, PreProcessorGeoDataGroupDataItemInterface*> m_itemNameMap;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	vtkSmartPointer<vtkLegendBoxWidget> m_legendBoxWidget;
	SolverDefinitionGridAttribute* m_condition;

	BoolContainer m_visible;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	SolverDefinitionGridAttribute* m_referenceInformationAttribute;
};

#endif // PREPROCESSORGEODATATOPDATAITEM_H
