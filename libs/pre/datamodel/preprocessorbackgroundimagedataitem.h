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

	void addCustomMenuItems(QMenu* menu) override;

	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;

	void updateMoveUpDownActions(ObjectBrowserView* view) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(2);}
	bool addToolBarButtons(QToolBar* toolbar) override;
	BackgroundImageInfo* imageInfo() {return m_imageInfo;}
	QAction* fixAction();
	void handleStandardItemChange() override;

private slots:
	void applyImageChange();
	void showGeoreferenceDialog();
	void enableObjectBrowserView();

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}
	void assignActorZValues(const ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* dialog) override;
	void setEnableObjectBrowserView(bool enabled);
	void doApplyOffset(double x, double y) override;

	QString m_filename;
	BackgroundImageInfo* m_imageInfo;

private:
	QAction* m_georeferenceAction;
	vtkSmartPointer<vtkActor> m_actor;

	bool m_isCommandExecuting;
};

#endif // PREPROCESSORBACKGROUNDIMAGEDATAITEM_H
