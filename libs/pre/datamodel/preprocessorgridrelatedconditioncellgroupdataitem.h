#ifndef PREPROCESSORGRIDRELATEDCONDITIONCELLGROUPDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONCELLGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <QMap>
#include <vtkLODActor.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkContourFilter.h>

class QAction;
class PreProcessorGridRelatedConditionCellDataItem;

class PreProcessorGridRelatedConditionCellGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridRelatedConditionCellGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridRelatedConditionCellGroupDataItem();
	void updateActorSettings();
	const QString& currentCondition(){return m_currentCondition;}
	void informDataChange(const QString& name);
	void setupActors();
	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void informGridUpdate();
	void setCurrentCondition(const QString& currentCond);
	const QList<PreProcessorGridRelatedConditionCellDataItem*> conditions() const;
	PreProcessorGridRelatedConditionCellDataItem* cellDataItem(const QString& name){return m_nameMap.value(name, 0);}
	void handleStandardItemChange();
	void setOpacityPercent(int o){m_opacityPercent = o;}
	int opacityPercent(){return m_opacityPercent;}
	QAction* showAttributeBrowserAction() const {return m_showAttributeBrowserAction;}
	void addCustomMenuItems(QMenu* menu);
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar *toolbar);

public slots:
	void exclusivelyCheck(PreProcessorGridRelatedConditionCellDataItem*);
	void showAttributeBrowser();

protected:
	PreProcessorGridRelatedConditionCellDataItem* activeChildItem();
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);

	QString m_currentCondition;
	vtkSmartPointer<vtkLODActor> m_actor;
//	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;
	QMap<QString, PreProcessorGridRelatedConditionCellDataItem*> m_nameMap;

	QAction* m_showAttributeBrowserAction;
	int m_opacityPercent;
	bool m_attributeBrowserFixed;

public:
	friend class PreProcessorGridRelatedConditionCellGroupSelectCondition;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONCELLGROUPDATAITEM_H
