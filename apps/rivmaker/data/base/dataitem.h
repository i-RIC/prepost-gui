#ifndef DATAITEM_H
#define DATAITEM_H

#include <QObject>

#include <memory>
#include <vector>

class Project;
class DataItemController;
class DataItemView;

class QDomElement;
class QStandardItem;
class QXmlStreamWriter;

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

	virtual void loadFromMainFile(const QDomElement& node);
	virtual void saveToMainFile(QXmlStreamWriter* writer) const;
	virtual QStringList containedFiles() const;

protected:
	void unregisterChild(DataItem* child);
	void clearChildItems();
	void setDeletable(bool deletable);

	static void setupStandardItem(QStandardItem* item);

private:
	virtual void doLoadFromMainFile(const QDomElement& node);
	virtual void doSaveToMainFile(QXmlStreamWriter* writer) const;

	virtual void loadExternalData(const QString& filename);
	virtual void saveExternalData(const QString& filename) const;

	QString filename() const;
	virtual QString relativeFilename() const;

	std::vector<DataItem*> m_childItems;
	bool m_isDeletable;
};

#endif // DATAITEM_H
