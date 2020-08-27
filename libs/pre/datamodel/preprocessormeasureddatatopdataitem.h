#ifndef PREPROCESSORMEASUREDDATATOPDATAITEM_H
#define PREPROCESSORMEASUREDDATATOPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>

#include <QMap>

class QMenu;
class QAction;
class MeasuredDataFileDataItem;

class PreProcessorMeasuredDataTopDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorMeasuredDataTopDataItem(GraphicsWindowDataItem* parent);
	~PreProcessorMeasuredDataTopDataItem() override;
	const QList<MeasuredDataFileDataItem*> fileDataItems() const;
	void setupActors();
	void updateActorSettings();
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void addChildItem();
	void deleteChildItem(int index);

private slots:
	void deleteSelected();
	void deleteAll();

signals:
	void selectMeasuredData(const QModelIndex& current);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QAction* m_importAction;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;
};

#endif // PREPROCESSORMEASUREDDATATOPDATAITEM_H
