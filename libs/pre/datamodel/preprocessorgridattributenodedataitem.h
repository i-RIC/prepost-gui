#ifndef PREPROCESSORGRIDRELATEDCONDITIONNODEDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONNODEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include <guibase/contoursettingwidget.h>

#include <vtkIdList.h>

#include <QList>

class SolverDefinitionGridAttribute;
class QAction;
class vtkMapper;
class vtkStructuredGrid;
class PreProcessorGridCrosssectionWindowProjectDataItem;
class Structured2DGridCrosssectionEditCommand;

class PreProcessorGridAttributeNodeDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeNodeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	~PreProcessorGridAttributeNodeDataItem();

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	ContourSettingWidget::Contour contour() {return m_contour;}
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void addCustomMenuItems(QMenu* menu) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	SolverDefinitionGridAttribute* condition() {return m_condition;}
	void informDataChange();
	int numberOfDivision() {return m_numberOfDivision;}

	void updateCrossectionWindows();
	void requestCrosssectionWindowDelete(PreProcessorGridCrosssectionWindowProjectDataItem* item);
	void unregisterCrosssectionWindow(PreProcessorGridCrosssectionWindowProjectDataItem* item);
	void informSelectedVerticesChanged(const QVector<vtkIdType>& vertices);
	bool addToolBarButtons(QToolBar* toolbar) override;

public slots:
	void openCrossSectionWindow();
	void openVerticalCrossSectionWindow();

private slots:
	void editValue();
	void editVariation();
	void showDisplaySettingDialog() {showPropertyDialog();}
	void exportToFile();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadFromCgnsFile(const int fn) override;
	void doApplyOffset(double x, double y) override;

private:
	SolverDefinitionGridAttribute* m_condition;

	bool m_definingBoundingBox;
	/// temporarily used when loading from project file.
	bool m_isCustomModified;
	QAction* m_editValueAction;
	QAction* m_exportAction;
	QAction* m_editVariationAction;
	QAction* m_openXsectionWindowAction;
	QAction* m_openVXsectionWindowAction;

	int m_numberOfDivision;
	ContourSettingWidget::Contour m_contour;

	void saveContourToProjectMainFile(QXmlStreamWriter& writer);
	void loadContourFromProjectMainFile(const QDomNode& node);

	QList<PreProcessorGridCrosssectionWindowProjectDataItem*> m_crosssectionWindows;

public:
	friend class Structured2DGridCrosssectionEditCommand;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONNODEDATAITEM_H
