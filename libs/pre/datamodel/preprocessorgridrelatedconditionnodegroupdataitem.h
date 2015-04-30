#ifndef PREPROCESSORGRIDRELATEDCONDITIONNODEGROUPDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONNODEGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <QMap>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkContourFilter.h>

class QAction;
class PreProcessorGridRelatedConditionNodeDataItem;

class PreProcessorGridRelatedConditionNodeGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridRelatedConditionNodeGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridRelatedConditionNodeGroupDataItem();
	void updateActorSettings();
	const QString& currentCondition(){return m_currentCondition;}
	void informDataChange(const QString& name);
	void setupActors();
	void updateZDepthRangeItemCount();
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void assignActionZValues(const ZDepthRange& range);
	void informGridUpdate();
	void setCurrentCondition(const QString& currentCond);
	const QList<PreProcessorGridRelatedConditionNodeDataItem*> conditions() const;
	PreProcessorGridRelatedConditionNodeDataItem* nodeDataItem(const QString& name){return m_nameMap.value(name, 0);}
	void handleStandardItemChange();
	void setOpacityPercent(int o){m_opacityPercent = o;}
	int opacityPercent(){return m_opacityPercent;}
	void informSelectedVerticesChanged(const QVector<vtkIdType> &vertices);
	QAction* showAttributeBrowserAction() const {return m_showAttributeBrowserAction;}
	void addCustomMenuItems(QMenu* menu);
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar *toolbar);

public slots:
	void exclusivelyCheck(PreProcessorGridRelatedConditionNodeDataItem*);
	void showAttributeBrowser();

protected:
	PreProcessorGridRelatedConditionNodeDataItem* activeChildItem();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);

private:
	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);

	QString m_currentCondition;
	vtkSmartPointer<vtkPolyData> m_contourPolyData;
	vtkSmartPointer<vtkActor> m_contourActor;
	vtkSmartPointer<vtkDataSetMapper> m_contourMapper;
	vtkSmartPointer<vtkActor> m_isolineActor;
	vtkSmartPointer<vtkPolyDataMapper> m_isolineMapper;
	vtkSmartPointer<vtkContourFilter> m_isolineFilter;
	vtkSmartPointer<vtkActor> m_fringeActor;
	vtkSmartPointer<vtkDataSetMapper> m_fringeMapper;

	QAction* m_showAttributeBrowserAction;
	int m_opacityPercent;
	bool m_attributeBrowserFixed;
	QMap<QString, PreProcessorGridRelatedConditionNodeDataItem*> m_nameMap;
public:
	friend class PreProcessorSelectCondition;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONNODEGROUPDATAITEM_H
