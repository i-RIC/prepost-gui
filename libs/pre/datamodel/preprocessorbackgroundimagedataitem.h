#ifndef PREPROCESSORBACKGROUNDIMAGEDATAITEM_H
#define PREPROCESSORBACKGROUNDIMAGEDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>

class BackgroundImageInfo;
class QAction;

class PreProcessorBackgroundImageDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	PreProcessorBackgroundImageDataItem(BackgroundImageInfo* image, PreProcessorDataItem* parent);
	~PreProcessorBackgroundImageDataItem();

	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);

	void updateMoveUpDownActions(ObjectBrowserView* view);
	void updateZDepthRangeItemCount(){m_zDepthRange.setItemCount(2);}
	bool addToolBarButtons(QToolBar* toolbar);
	BackgroundImageInfo* imageInfo(){return m_imageInfo;}
	QAction* fixAction();
	void handleStandardItemChange();
private slots:
	void applyImageChange();

protected:
	void doLoadFromProjectMainFile(const QDomNode& ){}
	void doSaveToProjectMainFile(QXmlStreamWriter& ){}
	void assignActionZValues(const ZDepthRange &range);
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* dialog);
	void doApplyOffset(double x, double y);

	QString m_filename;
	BackgroundImageInfo* m_imageInfo;
private:
	vtkSmartPointer<vtkActor> m_actor;

	bool m_isCommandExecuting;
};

#endif // PREPROCESSORBACKGROUNDIMAGEDATAITEM_H
