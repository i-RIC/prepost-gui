#ifndef PREPROCESSORGRIDATTRIBUTECELLDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTECELLDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include <misc/boolcontainer.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditwidget.h>

class ColorMapSettingContainerI;
class PreProcessorGeoDataGroupDataItemInterface;
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
	ColorMapSettingContainerI* colorMapSettingContainer() const;

private slots:
	void editValue();
	void editDifference();
	void editRatio();
	void showDisplaySettingDialog() {showPropertyDialog();}
	void exportToFile();
	void generatePointMap();

private:
	PreProcessorGeoDataGroupDataItemInterface* geoDataGroup() const;
	void editVariation(GridAttributeVariationEditWidget::Mode mode, const QString& typeName);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	int loadFromCgnsFile() override;

	SolverDefinitionGridAttribute* m_condition;

	BoolContainer m_isCustomModified;

	bool m_definingBoundingBox;
	QAction* m_editValueAction;
	QAction* m_exportAction;
	QAction* m_generatePointMapAction;
	QAction* m_editDifferenceAction;
	QAction* m_editRatioAction;

	class PropertyDialog;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLDATAITEM_H
