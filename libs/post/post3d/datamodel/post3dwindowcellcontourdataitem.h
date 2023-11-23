#ifndef POST3DWINDOWCELLCONTOURDATAITEM_H
#define POST3DWINDOWCELLCONTOURDATAITEM_H

#include "../post3dwindowcellrangesettingcontainer.h"
#include "../post3dwindowdataitem.h"

class Post3dWindowCellContourGroupDataItem;

class vtkActor;

class Post3dWindowCellContourDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowCellContourDataItem(const QString& label, Post3dWindowDataItem* p);
	~Post3dWindowCellContourDataItem();

	Post3dWindowCellRangeSettingContainer setting() const;
	void setSetting(const Post3dWindowCellRangeSettingContainer& setting);

	void update();

private:
	Post3dWindowCellContourGroupDataItem* groupDataItem() const;
	void updateActorSettings();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void innerUpdateZScale(double scale) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	void updateVisibility(bool visible) override;

	vtkActor* m_actor;

	Post3dWindowCellRangeSettingContainer m_setting;
};

#endif // POST3DWINDOWCELLCONTOURDATAITEM_H
