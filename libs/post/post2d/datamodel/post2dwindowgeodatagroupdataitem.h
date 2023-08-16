#ifndef POST2DWINDOWGEODATAGROUPDATAITEM_H
#define POST2DWINDOWGEODATAGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QMap>

class ColorMapSettingToolBarWidgetController;
class DelegatedColorMapSettingContainer;
class SolverDefinitionGridAttribute;
class Post2dWindowGeoDataDataItem;
class Post2dWindowGridTypeDataItem;

class Post2dWindowGeoDataGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent);
	~Post2dWindowGeoDataGroupDataItem();
	SolverDefinitionGridAttribute* condition() const;
	bool colorBarShouldBeVisible() const;

	DelegatedColorMapSettingContainer* colorMapSetting() const;
	void addCustomMenuItems(QMenu* menu) override;
	void updateChildren();
	void applyColorMapSetting();

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void handleStandardItemChange() override;
	void updateVisibility(bool visible) override;

	bool addToolBarButtons(QToolBar* toolBar) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QMap<QString, Post2dWindowGeoDataDataItem*> m_itemNameMap;
	ColorMapSettingToolBarWidgetController* m_toolBarWidgetController;
	SolverDefinitionGridAttribute* m_condition;

	class ScalarsToColorsEditDialog;
};

#endif // POST2DWINDOWGEODATAGROUPDATAITEM_H
