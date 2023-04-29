#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowCellScalarGroupTopDataItem;

class Post2dWindowCellScalarGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);
	~Post2dWindowCellScalarGroupDataItem();

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

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void setupActors();
	void updateActorSetting() override;

	void innerUpdateZScale(double scale) override;

	Post2dWindowCellScalarGroupTopDataItem* topDataItem() const;

	class Impl;
	Impl* impl;

	class PropertyDialog;
	class Setting;
	class SettingEditWidget;

public:
	friend class Post2dWindowCellScalarGroupTopDataItem;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_H
