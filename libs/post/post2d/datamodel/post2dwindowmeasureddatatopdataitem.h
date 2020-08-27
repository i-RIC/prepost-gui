#ifndef POST2DWINDOWMEASUREDDATATOPDATAITEM_H
#define POST2DWINDOWMEASUREDDATATOPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/scalarbarsetting.h>

#include <vtkActor2D.h>
#include <vtkSmartPointer.h>
#include <vtkLegendBoxActor.h>
#include <vtkTextMapper.h>

#include <QMap>

class QMenu;
class QAction;
class MeasuredDataFileDataItem;
class SolverDefinitionGridAttribute;

class Post2dWindowMeasuredDataTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowMeasuredDataTopDataItem(GraphicsWindowDataItem* parent);
	~Post2dWindowMeasuredDataTopDataItem() override;
	const QList<MeasuredDataFileDataItem*> fileDataItems() const;
	void setupActors();
	void updateActorSettings();
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void addChildItem();
	void setupChildItem();
	void deleteChildItem(int index);

private slots:
	void deleteSelected();
	void deleteAll();

signals:
	void selectMeasuredData(const QModelIndex& current);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkSmartPointer<vtkLegendBoxActor> m_legendBoxActor;
	vtkSmartPointer<vtkActor2D> m_titleActor;
	vtkSmartPointer<vtkTextMapper> m_titleMapper;

	QAction* m_importAction;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;
};

#endif // POST2DWINDOWMEASUREDDATATOPDATAITEM_H
