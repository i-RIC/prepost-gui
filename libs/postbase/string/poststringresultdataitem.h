#ifndef POSTSTRINGRESULTDATAITEM_H
#define POSTSTRINGRESULTDATAITEM_H

#include "../postbase_global.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/qfontcontainer.h>

#include <QFont>
#include <QImage>

class PostStringResult;

class vtkActor2D;
class vtkQImageToImageSource;
class vtkTextActor;

class POSTBASEDLL_EXPORT PostStringResultDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	enum class MouseEventMode {
		Normal,
		MovePrepare,
		Move,
		ResizeWHPrepare,
		ResizeWH,
		ResizeWPrepare,
		ResizeW,
		ResizeHPrepare,
		ResizeH,
	};

	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);
		Setting& operator=(const Setting& s);

		BoolContainer autoSize;
		IntContainer hMargin;
		IntContainer vMargin;
		IntContainer left;
		IntContainer top;
		IntContainer width;
		IntContainer height;

		ColorContainer fontColor;
		QFontContainer font;
		ColorContainer backgroundColor;
	};

	PostStringResultDataItem(GraphicsWindowDataItem* parent);
	~PostStringResultDataItem() override;

	void update();

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	void setupActors();
	void updateMouseEventMode(QMouseEvent* event);
	void updateMouseCursor(VTKGraphicsView* v);

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doHandleResize(VTKGraphicsView* v) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	PostStringResult* m_stringResult;
	Setting m_setting;

	QImage m_image;
	vtkQImageToImageSource* m_imageToImage;
	vtkActor2D* m_actor;

	MouseEventMode m_mouseEventMode;
	QPoint m_oldPosition;

	class SetSettingCommand;
	class MoveCommand;
	class ResizeCommand;
};

#endif // POSTSTRINGRESULTDATAITEM_H
