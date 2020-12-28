#ifndef PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H
#define PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H

#include "preprocessorgeodatagroupdataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataiteminterface.h>

#include <QColor>

#include <vector>

class GridComplexConditionDialog;
class QAction;

class PreProcessorGeoDataComplexGroupDataItem : public PreProcessorGeoDataGroupDataItem, public PreProcessorGeoDataComplexGroupDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGeoDataComplexGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);
	~PreProcessorGeoDataComplexGroupDataItem();

	void loadFromCgnsFile(const int fn) override;
	void saveComplexGroupsToCgnsFile(const int fn) override;
	void addCustomMenuItems(QMenu* menu) override;

	SolverDefinitionGridAttribute* condition() override;
	ProjectData* projectData() const override;

	void setupStringConverter(GridAttributeStringConverter* converter) override;
	void setupEditWidget(GridAttributeEditWidget* widget) override;

	void applyScalarsToColorsSetting();

	void setupGroups(int count) override;
	std::vector<GridComplexConditionGroup*> groups() const override;

public slots:
	void showEditGroupDialog();
	void editScalarsToColors() override;

protected:
	void addBackground();

private:
	void updateColorMap();
	void applySettingsToScalarBar();
	void createDefaultGroup();
	void clearGroups();

	bool m_isCustomModified;
	QColor m_undefinedColor;

	std::vector<GridComplexConditionGroup*> m_groups;
	GridComplexConditionDialog* m_dialog;

	QAction* m_editGroupAction;
};

#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEM_H
