#include "poststringresultdataitem.h"
#include "poststringresultsettingdialog.h"
#include "private/poststringresultdataitem_movecommand.h"
#include "private/poststringresultdataitem_resizecommand.h"
#include "private/poststringresultdataitem_setsettingcommand.h"

#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/post/postzonedataitem.h>
#include <guicore/postcontainer/poststringresult.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QFont>
#include <QFontMetrics>
#include <QMainWindow>
#include <QMouseEvent>
#include <QIcon>
#include <QPainter>
#include <QString>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkImageMapper.h>
#include <vtkQImageToImageSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

PostStringResultDataItem::Setting::Setting() :
	CompositeContainer ({&autoSize, &hMargin, &vMargin, &left, &top, &width, &height, &fontColor, &font, &backgroundColor}),
	autoSize {"autoSize"},
	hMargin {"hMargin"},
	vMargin {"vMargin"},
	left {"left"},
	top {"top"},
	width {"width"},
	height {"height"},
	fontColor {"fontColor"},
	font {"font"},
	backgroundColor {"backgroundColor"}
{
	left = 10;
	top = 10;
	hMargin = 6;
	vMargin = 3;
	autoSize = true;
	fontColor = Qt::black;
	backgroundColor = Qt::lightGray;
}

PostStringResultDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

PostStringResultDataItem::Setting& PostStringResultDataItem::Setting::operator=(const PostStringResultDataItem::Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

PostStringResultDataItem::PostStringResultDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Label"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setupActors();

	m_stringResult = new PostStringResult(this);

	auto zItem = dynamic_cast<PostZoneDataItem*> (parent);
	m_stringResult->setZoneDataContainer(zItem->dataContainer(), mainWindow());
	m_stringResult->updateValue();
}

PostStringResultDataItem::~PostStringResultDataItem()
{
	renderer()->RemoveActor2D(m_actor);

	m_imageToImage->Delete();
	m_actor->Delete();

	delete m_stringResult;
}

void PostStringResultDataItem::setupActors()
{
	m_actor = vtkActor2D::New();
	m_image = QImage(":/libs/guicore/images/logo.png");

	m_imageToImage = vtkQImageToImageSource::New();
	m_imageToImage->SetQImage(&m_image);

	auto mapper = vtkImageMapper::New();
	mapper->SetColorWindow(255);
	mapper->SetColorLevel(127.5);
	mapper->SetInputConnection(m_imageToImage->GetOutputPort());
	m_actor->SetMapper(mapper);
	mapper->Delete();

	renderer()->AddActor2D(m_actor);
}

void PostStringResultDataItem::updateMouseEventMode(QMouseEvent* event)
{
	m_mouseEventMode = MouseEventMode::Normal;
	QPoint lowerRight(m_setting.left + m_setting.width, m_setting.top + m_setting.height);

	if (iRIC::isNear(lowerRight, event->pos())) {
		m_mouseEventMode = MouseEventMode::ResizeWHPrepare;
		return;
	}
	if (
		std::abs(event->pos().x() - (m_setting.left + m_setting.width)) < iRIC::nearRadius() &&
		event->pos().y() >= m_setting.top &&
		event->pos().y() <= m_setting.top + m_setting.height)
	{
		m_mouseEventMode = MouseEventMode::ResizeWPrepare;
		return;
	}

	if (
		std::abs(event->pos().y() - (m_setting.top + m_setting.height)) < iRIC::nearRadius() &&
		event->pos().x() >= m_setting.left &&
		event->pos().x() <= m_setting.left + m_setting.width)
	{
		m_mouseEventMode = MouseEventMode::ResizeHPrepare;
		return;
	}

	if (
		event->pos().x() >= m_setting.left &&
		event->pos().x() <= m_setting.left + m_setting.width &&
		event->pos().y() >= m_setting.top &&
		event->pos().y() <= m_setting.top + m_setting.height
	) {
		m_mouseEventMode = MouseEventMode::MovePrepare;
		return;
	}
}

void PostStringResultDataItem::updateMouseCursor(VTKGraphicsView* v)
{
	switch (m_mouseEventMode) {
	case MouseEventMode::MovePrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;

	case MouseEventMode::Move:
		v->setCursor(Qt::ClosedHandCursor);
		break;

	case MouseEventMode::ResizeWPrepare:
	case MouseEventMode::ResizeW:
		v->setCursor(Qt::SizeHorCursor);
		break;

	case MouseEventMode::ResizeHPrepare:
	case MouseEventMode::ResizeH:
		v->setCursor(Qt::SizeVerCursor);
		break;

	case MouseEventMode::ResizeWHPrepare:
	case MouseEventMode::ResizeWH:
		v->setCursor(Qt::SizeFDiagCursor);
		break;

	default:
		v->setCursor(Qt::ArrowCursor);
	}
}

void PostStringResultDataItem::update()
{
	auto zItem = dynamic_cast<PostZoneDataItem*> (parent());
	auto c = zItem->dataContainer();
	if (c == nullptr) {
		m_actor->VisibilityOff();
		return;
	}

	m_stringResult->setZoneDataContainer(c, mainWindow());
	m_stringResult->updateValue();
	auto v = m_stringResult->value();

	actor2DCollection()->RemoveAllItems();

	if (v.isEmpty()) {
		m_actor->VisibilityOff();
		return;
	}

	QFontMetrics metrics(m_setting.font);

	auto lines = v.split("\n");
	int maxWidth = 0;
	for (auto l : lines) {
		auto rect = metrics.boundingRect(l);
		if (rect.width() > maxWidth) {
			maxWidth = rect.width();
		}
	}
	int height = metrics.height() + (lines.size() - 1) * metrics.lineSpacing();

	QSize size(maxWidth + m_setting.hMargin * 2, height + m_setting.vMargin * 2);
	if (! m_setting.autoSize) {
		size = QSize(m_setting.width, m_setting.height);
	}
	QImage image(size.width(), size.height(), QImage::Format_RGB888);
	QPainter painter;
	painter.begin(&image);

	QBrush brush(m_setting.backgroundColor);
	painter.fillRect(0, 0, size.width(), size.height(), brush);
	QPen pen(m_setting.fontColor);
	painter.setPen(pen);
	painter.setFont(m_setting.font);
	int lnum = 0;
	for (auto l : lines) {
		painter.drawText(QRect(m_setting.hMargin, m_setting.vMargin + metrics.lineSpacing() * lnum , maxWidth, metrics.lineSpacing()), Qt::AlignLeft, l);
		++ lnum;
	}
	painter.end();
	m_image = image;
	m_imageToImage->SetQImage(&m_image);
	int* wsize = dataModel()->graphicsView()->GetRenderWindow()->GetSize();
	int wheight = *(wsize + 1);

	int vPos = wheight - size.height() - m_setting.top;
	m_actor->SetPosition(m_setting.left, vPos);

	actor2DCollection()->AddItem(m_actor);
	updateVisibilityWithoutRendering();

	if (m_setting.autoSize) {
		m_setting.width = size.width();
		m_setting.height = size.height();
	}
}

void PostStringResultDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	switch (m_mouseEventMode) {
	case MouseEventMode::Move:
		pushRenderCommand(new MoveCommand(false, m_oldPosition, event->pos(), this), this);
		break;

	case MouseEventMode::ResizeW:
		pushRenderCommand(new ResizeCommand(false, m_oldPosition, QPoint(event->pos().x(), m_oldPosition.y()), this), this);
		break;

	case MouseEventMode::ResizeH:
		pushRenderCommand(new ResizeCommand(false, m_oldPosition, QPoint(m_oldPosition.x(), event->pos().y()), this), this);
		break;

	case MouseEventMode::ResizeWH:
		pushRenderCommand(new ResizeCommand(false, m_oldPosition, event->pos(), this), this);
		break;

	default:
		updateMouseEventMode(event);
		updateMouseCursor(v);
		break;
	}

	m_oldPosition = event->pos();
}

void PostStringResultDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	switch (m_mouseEventMode) {
	case MouseEventMode::MovePrepare:
		m_mouseEventMode = MouseEventMode::Move;
		pushRenderCommand(new MoveCommand(true, event->pos(), event->pos(), this), this);
		updateMouseCursor(v);
		break;

	case MouseEventMode::ResizeWPrepare:
		m_mouseEventMode = MouseEventMode::ResizeW;
		pushRenderCommand(new ResizeCommand(false, event->pos(), event->pos(), this), this);
		updateMouseCursor(v);
		break;

	case MouseEventMode::ResizeHPrepare:
		m_mouseEventMode = MouseEventMode::ResizeH;
		pushRenderCommand(new ResizeCommand(false, event->pos(), event->pos(), this), this);
		updateMouseCursor(v);
		break;

	case MouseEventMode::ResizeWHPrepare:
		m_mouseEventMode = MouseEventMode::ResizeWH;
		pushRenderCommand(new ResizeCommand(false, event->pos(), event->pos(), this), this);
		updateMouseCursor(v);
		break;

	default:
		break;
	}
	m_oldPosition = event->pos();
}

void PostStringResultDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateMouseEventMode(event);
	updateMouseCursor(v);
}

QDialog* PostStringResultDataItem::propertyDialog(QWidget* w)
{
	auto zItem = dynamic_cast<PostZoneDataItem*> (parent());
	auto c = zItem->dataContainer();
	if (c == nullptr) {return nullptr;}

	auto dialog = new PostStringResultSettingDialog(w);
	dialog->setZoneDataContainer(c);
	dialog->setStringResult(m_stringResult);
	dialog->setDisplaySetting(m_setting);
	return dialog;
}

void PostStringResultDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<PostStringResultSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->stringResult(), dialog->displaySetting(), this), this);
}

void PostStringResultDataItem::doHandleResize(VTKGraphicsView*)
{
	update();
}

void PostStringResultDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	auto srNode = iRIC::getChildNode(node, "StringResult");
	if (! srNode.isNull()) {
		m_stringResult->loadFromProjectMainFile(srNode);
	}
	update();
}

void PostStringResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	writer.writeStartElement("StringResult");
	m_stringResult->saveToProjectMainFile(writer);
	writer.writeEndElement(); // StringResult
}
