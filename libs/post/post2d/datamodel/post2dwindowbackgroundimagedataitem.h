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

	void handleStandardItemChange();
	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);

	void updateMoveUpDownActions(ObjectBrowserView* view);
	void updateZDepthRangeItemCount(){m_zDepthRange.setItemCount(2);}
	BackgroundImageInfo* imageInfo(){return m_imageInfo;}
	QAction* fixAction();
private slots:
	void applyImageChange();

protected:
	void doLoadFromProjectMainFile(const QDomNode& ){}
	void doSaveToProjectMainFile(QXmlStreamWriter& ){}
	void assignActionZValues(const ZDepthRange &range);
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* dialog);

private:
	vtkSmartPointer<vtkActor> m_actor;
	BackgroundImageInfo* m_imageInfo;
	QString m_filename;
};

#endif // POST2DWINDOWBACKGROUNDIMAGEDATAITEM_H
