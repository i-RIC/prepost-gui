#ifndef POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_H
#define POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/postcontainer/v4solutiongrid.h>

#include <memory>

class Post2dWindowAbstractCellScalarGroupTopDataItem;
class ValueRangeContainer;
class vtkPointSetExtended;

class Post2dWindowAbstractCellScalarGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowAbstractCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);
	~Post2dWindowAbstractCellScalarGroupDataItem();

	void update();

	const std::string& target() const;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	bool hasTransparentPart() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	void updateMoveUpDownActions(ObjectBrowserView* view) override;

protected:
	Post2dWindowAbstractCellScalarGroupTopDataItem* topDataItem() const;

	class Impl;
	std::unique_ptr<Impl> impl;

	class SettingEditWidget;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void setupActors();
	void updateActorSetting() override;

	void innerUpdateZScale(double scale) override;

	virtual void getDimensions(int* dimI, int* dimJ) const = 0;
	virtual v4SolutionGrid::Position position() const = 0;
	virtual vtkPointSet* buildRegionFilteredData() const = 0;
	virtual const ValueRangeContainer& valueRange(const::std::string& name) const = 0;
	virtual vtkPointSetExtended* data() const = 0;
	virtual vtkPointSet* filteredData() const = 0;

	class PropertyDialog;
	class Setting;

public:
	friend class Post2dWindowAbstractCellScalarGroupTopDataItem;
};

#endif // POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_H
