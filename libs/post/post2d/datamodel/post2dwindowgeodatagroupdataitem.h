#ifndef POST2DWINDOWGEODATAGROUPDATAITEM_H
#define POST2DWINDOWGEODATAGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QMap>

class DelegatedColorMapSettingContainer;
class SolverDefinitionGridAttribute;
class Post2dWindowGeoDataDataItem;

class Post2dWindowGeoDataGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent);
	SolverDefinitionGridAttribute* condition() const;

	DelegatedColorMapSettingContainer* colorMapSetting() const;
	void addCustomMenuItems(QMenu* menu) override;
	void updateChildren();
	void applyColorMapSetting();

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private slots:
	void editScalarsToColors();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QMap<QString, Post2dWindowGeoDataDataItem*> m_itemNameMap;
	SolverDefinitionGridAttribute* m_condition;

	QAction* m_editColorMapAction;

	class ScalarsToColorsEditDialog;
};

#endif // POST2DWINDOWGEODATAGROUPDATAITEM_H
