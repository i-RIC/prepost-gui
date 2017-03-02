#ifndef PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H
#define PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H

#include "preprocessorgeodatagroupdataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataiteminterface.h>

#include <QList>
#include <QColor>

class GridComplexConditionDialog;
class GridComplexConditionWidget;
class QAction;
class GeoDataBackgroundComplex;

class PreProcessorGeoDataComplexGroupDataItem : public PreProcessorGeoDataGroupDataItem, public PreProcessorGeoDataComplexGroupDataItemInterface
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGeoDataComplexGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);
	~PreProcessorGeoDataComplexGroupDataItem();
	void loadFromCgnsFile(const int fn) override;
	void saveComplexGroupsToCgnsFile(const int fn) override;
	void addCustomMenuItems(QMenu* menu) override;
	ProjectData* projectData() const override {return dynamic_cast<ProjectData*>(ProjectDataItem::projectData());}
	void setupEditWidget(GridAttributeEditWidget* widget) override;
	void applyScalarsToColorsSetting();
	SolverDefinitionGridAttribute* condition() override;

	void setupWidgets(int widgetCount) override;
	QList<GridComplexConditionWidget*> widgets() const override;

public slots:
	void showEditGroupDialog();
	void editScalarsToColors() override;

protected:
	void addBackground();

private:
	void clear();
	void updateColorMap();
	void applySettingsToScalarBar();
	void createDefaultGroup();
	void clearWidgets();

	bool m_isCustomModified;
	QColor m_undefinedColor;

	QList<GridComplexConditionWidget*> m_widgets;
	GridComplexConditionDialog* m_dialog;

	QAction* m_editGroupAction;

public:
	friend class GeoDataBackgroundComplex;
};

#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H
