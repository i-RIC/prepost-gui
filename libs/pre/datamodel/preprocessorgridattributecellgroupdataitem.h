#ifndef PREPROCESSORGRIDRELATEDCONDITIONCELLGROUPDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONCELLGROUPDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <QMap>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkContourFilter.h>

class QAction;
class PreProcessorGridAttributeCellDataItem;

class PreProcessorGridAttributeCellGroupDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGridAttributeCellGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeCellGroupDataItem();
	void updateActorSettings();
	const QString& currentCondition() {return m_currentCondition;}
	void informDataChange(const QString& name);
	void setupActors();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void informGridUpdate();
	void setCurrentCondition(const QString& currentCond);
	const QList<PreProcessorGridAttributeCellDataItem*> conditions() const;
	PreProcessorGridAttributeCellDataItem* cellDataItem(const QString& name) {return m_nameMap.value(name, 0);}
	void handleStandardItemChange() override;
	void setOpacityPercent(int o) {m_opacityPercent = o;}
	int opacityPercent() {return m_opacityPercent;}
	QAction* showAttributeBrowserAction() const {return m_showAttributeBrowserAction;}
	void addCustomMenuItems(QMenu* menu) override;
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar* toolbar) override;

public slots:
	void exclusivelyCheck(PreProcessorGridAttributeCellDataItem*);
	void showAttributeBrowser();

protected:
	PreProcessorGridAttributeCellDataItem* activeChildItem();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);

	QString m_currentCondition;
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;
	QMap<QString, PreProcessorGridAttributeCellDataItem*> m_nameMap;

	QAction* m_showAttributeBrowserAction;
	int m_opacityPercent;
	bool m_attributeBrowserFixed;

public:
	friend class PreProcessorGridRelatedConditionCellGroupSelectCondition;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONCELLGROUPDATAITEM_H
