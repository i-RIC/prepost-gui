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
	PreProcessorBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, GraphicsWindowDataItem* parent, bool hideSetting = false);
	~PreProcessorBCDataItem();

	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void handleStandardItemDoubleClicked() override;
	void handleStandardItemChange() override;
	void setName(const QString& name);
	void setProjectNumber(int num);
	void setCgnsNumber(int num);
	int projectNumber() const;
	int cgnsNumber() const;
	void setColor(const QColor& color);
	QColor color() const;
	int opacityPercent() const;
	SolverDefinitionBoundaryCondition* condition() const;
	bool isValid() const;
	std::string uniqueName() const;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void addCustomMenuItems(QMenu* menu) override;
	void setGrid(Grid* grid);
	void clearPoints();
	void assignIndices(const QSet<vtkIdType>& vertices);
	bool isCustomModified() const;
	QString caption() const;
	void setMapped(bool mapped);
	bool mapped() const;

	QAction* editAction() const;
	QAction* deleteAction() const;
	QAction* assignAction() const;
	QAction* releaseAction() const;
	bool hideSetting() const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x, double y) override;
	virtual void loadExternalData(const QString& filename) override;
	virtual void saveExternalData(const QString& filename) override;

public slots:
	bool showDialog();

private slots:
	void setModified(bool modified = true) override;
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
