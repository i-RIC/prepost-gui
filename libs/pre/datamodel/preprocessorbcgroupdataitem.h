#ifndef PREPROCESSORBCGROUPDATAITEM_H
#define PREPROCESSORBCGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>

class PreProcessorBCDataItem;
class ColorSource;
class Grid;

class PreProcessorBCGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorBCGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorBCGroupDataItem();
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
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

public slots:
	void addCondition();

private slots:
	void deleteSelected();
	void deleteAll();

signals:
	void itemsUpdated();
	void itemsLoaded();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void renumberItemsForCgns();

	QList<QAction*> m_addActions;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;

	int m_projectBuildNumber;

	QAction* m_dummyEditAction;
	QAction* m_dummyDeleteAction;
	QAction* m_dummyAssignAction;
	QAction* m_dummyReleaseAction;
	QMenu* m_bcMenu;

	ColorSource* m_colorSource;

public:
	friend class PreProcessorBCDataItem;
};

#endif // PREPROCESSORBCGROUPDATAITEM_H
