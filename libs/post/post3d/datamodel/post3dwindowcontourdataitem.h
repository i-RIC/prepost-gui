#ifndef POST3DWINDOWCONTOURDATAITEM_H
#define POST3DWINDOWCONTOURDATAITEM_H

#include "../post3dwindowfacesettingcontainer.h"
#include "../post3dwindowdataitem.h"

class Post3dWindowContourGroupDataItem;

class vtkActor;

class Post3dWindowContourDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowContourDataItem(const QString& label, Post3dWindowDataItem* p);
	~Post3dWindowContourDataItem();

	Post3dWindowFaceSettingContainer setting() const;
	void setSetting(const Post3dWindowFaceSettingContainer& setting);

	void update();

private:
	Post3dWindowContourGroupDataItem* groupDataItem() const;
	void updateActorSettings();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void innerUpdateZScale(double scale) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	vtkActor* m_actor;

	Post3dWindowFaceSettingContainer m_setting;
};

#endif // POST3DWINDOWCONTOURDATAITEM_H
