#ifndef PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H
#define PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H

#include "preprocessorgeodatagroupdataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataitemi.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>

#include <QColor>

#include <vector>

class ColorMapSettingToolBarWidgetController;
class GridComplexConditionDialog;
class QAction;

class PreProcessorGeoDataComplexGroupDataItem : public PreProcessorGeoDataGroupDataItem, public PreProcessorGeoDataComplexGroupDataItemI
{
	Q_OBJECT

public:
	PreProcessorGeoDataComplexGroupDataItem(SolverDefinitionGridComplexAttribute* cond, PreProcessorDataItem* parent);
	~PreProcessorGeoDataComplexGroupDataItem();

	int loadFromCgnsFile() override;
	int saveComplexGroupsToCgnsFile() override;
	void addCustomMenuItems(QMenu* menu) override;

	SolverDefinitionGridAttribute* condition() override;
	ProjectData* projectData() const override;

	void setupStringConverter(GridAttributeStringConverter* converter) override;
	void setupEditWidget(GridAttributeEditWidget* widget) override;

	void applyScalarsToColorsSetting();

	void setupGroups(int count) override;
	std::vector<GridComplexConditionGroup*> groups() const override;
	bool addToolBarButtons(QToolBar* parent) override;

	void showPropertyDialog() override;

public slots:
	void showEditGroupDialog();

protected:
	void addBackground();

private:
	void updateColorMap();

	bool m_isCustomModified;

	ColorMapSettingToolBarWidgetController* m_toolBarWidgetController;
	GridComplexConditionDialog* m_dialog;

	QAction* m_editGroupAction;
};

#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H
