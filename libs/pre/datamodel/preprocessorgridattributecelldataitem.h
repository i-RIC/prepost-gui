#ifndef PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H
#define PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include <misc/boolcontainer.h>

class SolverDefinitionGridAttribute;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class PreProcessorGridAttributeCellDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	~PreProcessorGridAttributeCellDataItem();

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

protected:
	void loadFromCgnsFile(const int fn) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	SolverDefinitionGridAttribute* m_condition;

	BoolContainer m_isCustomModified;

	bool m_definingBoundingBox;
	QAction* m_editValueAction;
	QAction* m_editVariationAction;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONCELLDATAITEM_H
