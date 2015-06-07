#ifndef PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridAttribute;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class PreProcessorGridAttributeCellDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	/// Constructor
	PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);
	void handleStandardItemChange() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
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
	bool addToolBarButtons(QToolBar* toolbar) override;

private slots:
	void editValue();
	void editVariation();
	void showDisplaySettingDialog() {showPropertyDialog();}

signals:
	void changed(PreProcessorGridAttributeCellDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadFromCgnsFile(const int fn) override;

private:
	SolverDefinitionGridAttribute* m_condition;

	bool m_definingBoundingBox;
	bool m_isCustomModified;
	QAction* m_editValueAction;
	QAction* m_editVariationAction;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H
