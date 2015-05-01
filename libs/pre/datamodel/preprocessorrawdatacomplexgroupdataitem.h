#ifndef PREPROCESSORRAWDATACOMPLEXGROUPDATAITEM_H
#define PREPROCESSORRAWDATACOMPLEXGROUPDATAITEM_H

#include "preprocessorrawdatagroupdataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/pre/base/preprocessorrawdatacomplexgroupdataiteminterface.h>

#include <QList>
#include <QColor>

class GridComplexConditionDialog;
class GridComplexConditionWidget;
class QAction;
class RawDataBackgroundComplex;

class PreProcessorRawDataComplexGroupDataItem : public PreProcessorRawDataGroupDataItem, public PreProcessorRawDataComplexGroupDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorRawDataComplexGroupDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent);
	~PreProcessorRawDataComplexGroupDataItem();
	void loadFromCgnsFile(const int fn);
	void saveComplexGroupsToCgnsFile(const int fn);
	void addCustomMenuItems(QMenu* menu);
	ProjectData* projectData() {return dynamic_cast<ProjectData*>(ProjectDataItem::projectData());}
	void setupEditWidget(GridRelatedConditionEditWidget* widget);
	void applyScalarsToColorsSetting();
	SolverDefinitionGridRelatedCondition* condition();

public slots:
	void showEditGroupDialog();
	void editScalarsToColors();

protected:
	void addBackground();

private:
	void clear();
	void updateColorMap();
	void applySettingsToScalarBar();
	void createDefaultGroup();

	bool m_isCustomModified;
	QColor m_undefinedColor;
	GridComplexConditionDialog* m_dialog;

	QAction* m_editGroupAction;

public:
	friend class RawDataBackgroundComplex;
};

#endif // PREPROCESSORRAWDATACOMPLEXGROUPDATAITEM_H
