#ifndef DATAITEM_H
#define DATAITEM_H

#include <QObject>

#include <memory>
#include <vector>

class Project;
class DataItemController;
class DataItemView;

class QStandardItem;

class DataItem : public QObject
{
	Q_OBJECT

public:
	explicit DataItem(DataItem *parent = nullptr);
	virtual ~DataItem();

	virtual Project* project() const;

	bool isDeletable() const;

	void setParent(DataItem* parent);

	DataItem* parentItem() const;
	const std::vector<DataItem*>& childItems() const;
	std::vector<DataItem*>& childItems();

protected:
	void unregisterChild(DataItem* child);
	void clearChildItems();
	void setDeletable(bool deletable);

	static void setupStandardItem(QStandardItem* item);

private:
	std::vector<DataItem*> m_childItems;
	bool m_isDeletable;
};

#endif // DATAITEM_H
