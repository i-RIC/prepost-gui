#ifndef POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H
#define POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>

class QAction;
class BackgroundImageInfo;

class Post2dWindowBackgroundImageDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowBackgroundImageDataItem(BackgroundImageInfo* image, Post2dWindowDataItem* parent);
	~Post2dWindowBackgroundImageDataItem();

	void handleStandardItemChange() override;
	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;

	void updateMoveUpDownActions(ObjectBrowserView* view) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(2);}
	BackgroundImageInfo* imageInfo() const {return m_imageInfo;}

private slots:
	void applyImageChange();

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}
	void assignActorZValues(const ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* dialog) override;

private:
	vtkSmartPointer<vtkActor> m_actor;
	BackgroundImageInfo* m_imageInfo;
	QString m_filename;
};

#endif // POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H
