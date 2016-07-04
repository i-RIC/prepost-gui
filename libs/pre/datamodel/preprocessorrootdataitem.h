#ifndef PREPROCESSORROOTDATAITEM_H
#define PREPROCESSORROOTDATAITEM_H

#include "../pre_global.h"
#include "../preprocessordatamodel.h"

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include <QList>
#include <QMap>

class QAction;
class GeoData;

class PreProcessorBackgroundImagesDataItem;
class PreProcessorGridTypeDataItem;
class PreProcessorGridDataItem;
class PreProcessorInputConditionDataItem;
class Axis2dDataItem;
class QStandardItemModel;
class PreProcessorWindow;
class PreProcessorRootDataItemSetMappingSettingCommand;
class PreProcessorMeasuredDataTopDataItem;
class DistanceMeasureGroupDataItem;
class AttributeBrowserTargetDataItem;
class TmsImageGroupDataItem;

class PREDLL_EXPORT PreProcessorRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT

public:
	PreProcessorRootDataItem(PreProcessorWindow* window, ProjectDataItem* parent);
	~PreProcessorRootDataItem();

	PreProcessorBackgroundImagesDataItem* backgroundImagesDataItem() const;
	TmsImageGroupDataItem* tmsGroupDataItem() const;
	PreProcessorMeasuredDataTopDataItem* measuredDataTopDataItem() const;
	PreProcessorGridTypeDataItem* gridTypeDataItem(const std::string& name) const;
	const QList<PreProcessorGridTypeDataItem*>& gridTypeDataItems() const;
	PreProcessorInputConditionDataItem* inputConditionDataItem() const;
	AttributeBrowserTargetDataItem* attributeBrowserTargetDataItem() const;

	void setupStandardModel(QStandardItemModel* model);

	QAction* editGridAttributeMappingSettingAction() const;

	bool isGridEdited() const;
	void setGridEdited();

	void deleteItem(QStandardItem* item) override;
	void saveToCgnsFile(const int fn) override;

private slots:
	void editGridAttributeMappingSetting();

protected:
	PreProcessorDataModel* dataModel() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	PreProcessorBackgroundImagesDataItem* m_backgroundImagesDataItem;
	TmsImageGroupDataItem* m_tmsGroupDataItem;
	PreProcessorMeasuredDataTopDataItem* m_measuredDataTopDataItem;
	PreProcessorInputConditionDataItem* m_inputConditionDataItem;
	Axis2dDataItem* m_axesDataItem;
	DistanceMeasureGroupDataItem* m_distanceMeasureGroupDataItem;
	AttributeBrowserTargetDataItem* m_attributeBrowserTargetDataItem;
	QList<PreProcessorGridTypeDataItem*> m_gridTypeDataItems;

	QAction* m_editGridAttributeMappingSettingAction;

public:
	friend class PreProcessorWindowProjectDataItem;
	friend class PreProcessorDataModel;
	friend class PreProcessorRootDataItemSetMappingSettingCommand;
	friend class GeoData;
};

#endif // PREPROCESSORROOTDATAITEM_H
