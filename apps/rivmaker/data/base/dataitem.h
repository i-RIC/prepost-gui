#ifndef DATAITEM_H
#define DATAITEM_H

#include <QObject>

#include <memory>

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

	DataItem* parentItem() const;
	const std::vector<DataItem*>& childItems() const;

protected:
	void unregisterChild(DataItem* child);
	void clearChildItems();
	std::vector<DataItem*>& childItems();

private:
	std::vector<DataItem*> m_childItems;
};

#endif // DATAITEM_H
