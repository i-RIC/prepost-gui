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
	/// Constructor
	PreProcessorMeasuredDataTopDataItem(GraphicsWindowDataItem* parent);
	/// Destructor
	~PreProcessorMeasuredDataTopDataItem();
	const QList<MeasuredDataFileDataItem*> fileDataItems() const;
	void setupActors();
	void updateActorSettings();
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void addChildItem();
	void deleteChildItem(int index);

signals:
	void selectMeasuredData(const QModelIndex& current);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	bool m_visible;

	QAction* m_importAction;
};

#endif // PREPROCESSORMEASUREDDATATOPDATAITEM_H
