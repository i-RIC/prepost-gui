#ifndef PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridRelatedCondition;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class PreProcessorGridRelatedConditionCellDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridRelatedConditionCellDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorDataItem* parent);
	void handleStandardItemChange();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void keyPressEvent(QKeyEvent *, VTKGraphicsView *);
	void keyReleaseEvent(QKeyEvent *, VTKGraphicsView *);
	void addCustomMenuItems(QMenu* menu);
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	SolverDefinitionGridRelatedCondition* condition(){return m_condition;}
	void informDataChange();
	bool addToolBarButtons(QToolBar *toolbar);

private slots:
	void editValue();
	void editVariation();
	void showDisplaySettingDialog(){showPropertyDialog();}

signals:
	void changed(PreProcessorGridRelatedConditionCellDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void loadFromCgnsFile(const int fn);

private:
	SolverDefinitionGridRelatedCondition* m_condition;

	bool m_definingBoundingBox;
	bool m_isCustomModified;
	QAction* m_editValueAction;
	QAction* m_editVariationAction;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H
