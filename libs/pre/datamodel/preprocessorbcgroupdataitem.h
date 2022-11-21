#ifndef PREPROCESSORBCGROUPDATAITEM_H
#define PREPROCESSORBCGROUPDATAITEM_H

#include <guibase/vtktextpropertysettingcontainer.h>
#include <guicore/pre/base/preprocessordataitem.h>

class PreProcessorBCDataItem;
class ColorSource;
class Grid;

namespace iRICLib {
	class H5CgnsZone;
} // namespace iRICLib

class PreProcessorBCGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorBCGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorBCGroupDataItem();

	const vtkTextPropertySettingContainer& nameSetting() const;

	int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone);
	int saveToCgnsFile(iRICLib::H5CgnsZone* zone);
	void informGridUpdate();
	const QList<PreProcessorBCDataItem*> conditions() const;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	QMenu* bcMenu() const;
	void updateBCMenu(PreProcessorBCDataItem* item);
	void clear();
	void clearPoints();
	PreProcessorBCDataItem* addCondition(int index, bool hideSetting = false);
	void renumberItemsForProject();
	const QList<QAction*> addActions() const;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

public slots:
	void addCondition();

private slots:
	void deleteSelected();
	void deleteAll();

signals:
	void itemsUpdated();
	void itemsLoaded();

private:
	void renumberItemsForCgns();
	void updateNameActorSettingsOfChildren();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QList<QAction*> m_addActions;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;

	int m_projectBuildNumber;

	QAction* m_dummyEditAction;
	QAction* m_dummyDeleteAction;
	QAction* m_dummyAssignAction;
	QAction* m_dummyReleaseAction;
	QMenu* m_bcMenu;

	vtkTextPropertySettingContainer m_nameSetting;
	ColorSource* m_colorSource;

	class SetSettingCommand;

public:
	friend class PreProcessorBCDataItem;
};

#endif // PREPROCESSORBCGROUPDATAITEM_H
