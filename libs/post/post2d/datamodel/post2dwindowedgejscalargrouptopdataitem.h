#ifndef POST2DWINDOWEDGEJSCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWEDGEJSCALARGROUPTOPDATAITEM_H

//#include "../post2dwindowdataitem.h"
#include "post2dwindowscalargrouptopdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/post2dwindowcontoursetting.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

#include <QMap>
#include <QList>

class Post2dWindowNodeScalarGroupDataItem;


class Post2dWindowEdgeJScalarGroupTopDataItem : public Post2dWindowScalarGroupTopDataItem // Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowEdgeJScalarGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowEdgeJScalarGroupTopDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	bool hasTransparentPart() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

private:
	std::map<std::string, Post2dWindowNodeScalarGroupDataItem*> m_scalarmap; // only used by ctor and doLoadFromProjectMainFile

	class CreateCommand;
};

#endif // POST2DWINDOWEDGEJSCALARGROUPTOPDATAITEM_H
