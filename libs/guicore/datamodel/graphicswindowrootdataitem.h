#ifndef GRAPHICSWINDOWROOTDATAITEM_H
#define GRAPHICSWINDOWROOTDATAITEM_H

#include "../guicore_global.h"
#include "graphicswindowdataitem.h"

#include <QMap>

class GraphicsWindowDataModel;
class QMainWindow;

class GUICOREDLL_EXPORT GraphicsWindowRootDataItem : public GraphicsWindowDataItem
{

public:
	GraphicsWindowRootDataItem(QMainWindow* window, ProjectDataItem* parent);
	virtual ~GraphicsWindowRootDataItem();

	void handleItemChange(QStandardItem* changedItem);
	void handleItemClick(QStandardItem* clickedItem);
	void handleItemDoubleClick(QStandardItem* clickedItem);

	void moveUpItem(QStandardItem* item);
	void moveDownItem(QStandardItem* item);
	virtual void deleteItem(QStandardItem* item, bool noDraw = false);

	void updateItemMap() override;
	GraphicsWindowDataItem* modelItemFromItem(QStandardItem* item);

	void loadFromCgnsFile(const int fn) override;
	void closeCgnsFile() override;

protected:
	bool isAncientChecked() const override;
	GraphicsWindowDataModel* dataModel() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QMap<QStandardItem*, GraphicsWindowDataItem*> m_itemMap;
	QMainWindow* m_mainWindow;
	GraphicsWindowDataModel* m_dataModel;

public:
	friend class GraphicsWindowDataModel;

private:
	class MoveUpCommand;
	class MoveDownCommand;
};

#endif // GRAPHICSWINDOWROOTDATAITEM_H
