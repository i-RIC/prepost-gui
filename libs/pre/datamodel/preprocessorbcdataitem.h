#ifndef PREPROCESSORBCDATAITEM_H
#define PREPROCESSORBCDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/edge.h>

#include <QString>
#include <QColor>
#include <QSet>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextMapper.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkExtractCells.h>

class BoundaryConditionDialog;
class SolverDefinition;
class SolverDefinitionBoundaryCondition;
class QAction;
class VTKGraphicsView;
class Grid;

class PreProcessorBCDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, GraphicsWindowDataItem* parent, bool hideSetting = false);
	virtual ~PreProcessorBCDataItem();
	void loadFromCgnsFile(const int fn);
	void saveToCgnsFile(const int fn);
	void handleStandardItemDoubleClicked() {showDialog();}
	void handleStandardItemChange();
	void setName(const QString& name);
	void setProjectNumber(int num);
	void setCgnsNumber(int num);
	int projectNumber() const {return m_projectNumber;}
	int cgnsNumber() const {return m_cgnsNumber;}
	void setColor(const QColor& color);
	QColor color() const;
	int opacityPercent() const;
	SolverDefinitionBoundaryCondition* condition() {return m_condition;}
	bool isValid() const;
	QString uniqueName() const;

	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*);
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*);
	void addCustomMenuItems(QMenu* menu) override;
	void setGrid(Grid* grid);
	void clearPoints();
	void assignIndices(const QSet<vtkIdType>& vertices);
	bool isCustomModified() const {return m_isCustomModified;}
	QString caption() const;
	void setMapped(bool mapped) {m_mapped = mapped;}
	bool mapped() const {return m_mapped;}

	QAction* editAction() {return m_editAction;}
	QAction* deleteAction() {return PreProcessorDataItem::deleteAction();}
	QAction* assignAction() {return m_assignAction;}
	QAction* releaseAction() {return m_releaseAction;}
	bool hideSetting() const {return m_hideSetting;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void doApplyOffset(double x, double y);
	virtual void loadExternalData(const QString& filename);
	virtual void saveExternalData(const QString& filename);
public slots:
	bool showDialog();

private slots:
	void setModified() {
		PreProcessorDataItem::setModified();
	}
	void assignSelectedElements();
	void releaseSelectedElements();

signals:
	void itemUpdated();

protected:
	void assignActorZValues(const ZDepthRange& range) override;
private:
	void setupActors();
	void updateActorSettings();
	void updateNameActorSettings();
	void updateElements();
	int buildNumber() const;

	vtkSmartPointer<vtkUnstructuredGrid> m_verticesGrid;
	vtkSmartPointer<vtkDataSetMapper> m_verticesMapper;
	vtkSmartPointer<vtkActor> m_verticesActor;
	vtkSmartPointer<vtkTextMapper> m_nameMapper;
	vtkSmartPointer<vtkActor2D> m_nameActor;

	vtkSmartPointer<vtkExtractCells> m_cellsGrid;
	vtkSmartPointer<vtkDataSetMapper> m_cellsMapper;
	vtkSmartPointer<vtkActor> m_cellsActor;

	vtkSmartPointer<vtkPolyData> m_edgesPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_edgesMapper;
	vtkSmartPointer<vtkActor> m_edgesActor;

	QSet<vtkIdType> m_indices;
	QSet<Edge> m_edges;

	SolverDefinitionBoundaryCondition* m_condition;
	QAction* m_editAction;
	QAction* m_assignAction;
	QAction* m_releaseAction;
	int m_projectNumber;
	int m_cgnsNumber;

	int m_opacityPercent;
	bool m_mapped;
	bool m_definingBoundingBox;
	bool m_showName;
	bool m_isCustomModified;

	bool m_hideSetting;
	BoundaryConditionDialog* m_dialog;
};

#endif // PREPROCESSORBCDATAITEM_H
