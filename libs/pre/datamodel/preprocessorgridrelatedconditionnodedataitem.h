#ifndef PREPROCESSORGRIDRELATEDCONDITIONNODEDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONNODEDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guibase/contoursettingwidget.h>

#include <vtkIdList.h>

#include <QList>

class SolverDefinitionGridRelatedCondition;
class QAction;
class vtkMapper;
class vtkStructuredGrid;
class PreProcessorGridCrosssectionWindowProjectDataItem;
class Structured2DGridCrosssectionEditCommand;

class PreProcessorGridRelatedConditionNodeDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridRelatedConditionNodeDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent);
	void handleStandardItemChange();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	ContourSettingWidget::Contour contour() {return m_contour;}
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*);
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*);
	void addCustomMenuItems(QMenu* menu) override;
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	SolverDefinitionGridRelatedCondition* condition() {return m_condition;}
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
signals:
	void changed(PreProcessorGridRelatedConditionNodeDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void loadFromCgnsFile(const int fn);
	void doApplyOffset(double x, double y);
private:
	SolverDefinitionGridRelatedCondition* m_condition;

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
