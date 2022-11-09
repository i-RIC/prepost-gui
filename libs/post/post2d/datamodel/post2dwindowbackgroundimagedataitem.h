#ifndef POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H
#define POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/vtktool/vtkactorpolydatamapperpair.h>

class QAction;
class BackgroundImageInfo;

class Post2dWindowBackgroundImageDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowBackgroundImageDataItem(BackgroundImageInfo* image, Post2dWindowDataItem* parent);
	~Post2dWindowBackgroundImageDataItem();

	void addCustomMenuItems(QMenu* menu) override;

	void handleStandardItemChange() override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	void updateMoveUpDownActions(ObjectBrowserView* view) override;
	void updateZDepthRangeItemCount() override;
	BackgroundImageInfo* imageInfo() const;

private slots:
	void applyImageChange();
	void showGeoreferenceDialog();
	void enableObjectBrowserView();

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
	void assignActorZValues(const ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* dialog) override;
	void setEnableObjectBrowserView(bool enabled);
	void doApplyOffset(double x, double y) override;

	QAction* m_georeferenceAction;
	vtkActorPolyDataMapperPair m_actor;
	BackgroundImageInfo* m_imageInfo;
};

#endif // POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H
