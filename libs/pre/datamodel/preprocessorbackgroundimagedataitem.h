#ifndef PREPROCESSORBACKGROUNDIMAGEDATAITEM_H
#define PREPROCESSORBACKGROUNDIMAGEDATAITEM_H

#include <guibase/vtktool/vtkactorpolydatamapperpair.h>
#include <guicore/pre/base/preprocessordataitem.h>

class BackgroundImageInfo;
class QAction;

class PreProcessorBackgroundImageDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorBackgroundImageDataItem(BackgroundImageInfo* image, PreProcessorDataItem* parent);
	~PreProcessorBackgroundImageDataItem();

	void addCustomMenuItems(QMenu* menu) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	void updateMoveUpDownActions(ObjectBrowserView* view) override;
	void updateZDepthRangeItemCount() override;
	bool addToolBarButtons(QToolBar* toolbar) override;

	BackgroundImageInfo* imageInfo() const;
	QAction* fixAction();
	void handleStandardItemChange() override;

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

#endif // PREPROCESSORBACKGROUNDIMAGEDATAITEM_H
