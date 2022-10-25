#define _USE_MATH_DEFINES
#include <math.h>

#include "../base/iricmainwindowinterface.h"
#include "../pre/base/preprocessordatamodelinterface.h"
#include "../pre/base/preprocessorgraphicsviewinterface.h"
#include "../pre/base/preprocessorwindowinterface.h"
#include "addiblegcptablemodel.h"
#include "backgroundimageinfo.h"
#include "backgroundimageinfodialog.h"
#include "backgroundimageinfogeoreferencedialog.h"
#include "gcptablerow.h"
#include "projectdata.h"
#include "projectmainfile.h"

#include "private/backgroundimageinfo_impl.h"
#include "private/backgroundimageinfo_setactorpropertycommand.h"

#include <misc/iricundostack.h>

const int BackgroundImageInfo::MAXWIDTH = 2048;

BackgroundImageInfo::BackgroundImageInfo(const QString& filename, ProjectDataItem* d) :
	ProjectDataItem(d),
	m_georeferenceDialog {nullptr},
	impl {new Impl {this}}
{
	QFileInfo finfo(filename);
	impl->m_setting.fileName = finfo.fileName();

	m_hide = false; // for georeference

	connect(impl->m_fixAction, SIGNAL(triggered()), this, SLOT(toggleFixState()));
	connect(impl->m_fixActionWithIcon, SIGNAL(triggered()), this, SLOT(toggleFixState()));
	impl->updateFixActionIcon();

	impl->loadImageData(filename);
}

BackgroundImageInfo::~BackgroundImageInfo()
{
	delete impl;
}

void BackgroundImageInfo::initializePosition(const QString& originalFileName)
{
	QString worldExt;

	QFileInfo finfo(originalFileName);
	auto ext = finfo.suffix().toLower();

	if (ext == "jpg" || ext == "jpeg") {
		worldExt = "jgw";
	} else if (ext == "png") {
		worldExt = "pgw";
	} else if (ext == "tif") {
		worldExt = "tfw";
	}
	auto worldFileName = QString("%1/%2.%3")
			.arg(finfo.absolutePath())
			.arg(finfo.baseName())
			.arg(worldExt);

	QFile worldFile(worldFileName);
	if (worldFile.exists())	{
		impl->applyWorldFileSetting(worldFileName);
	} else {
		fitImageToData();
	}
}

QString BackgroundImageInfo::fileName() const
{
	return impl->m_setting.fileName;
}

QString BackgroundImageInfo::fullFileName() const
{
	return QString("%1/%2/%3")
			.arg(projectData()->workDirectory())
			.arg(ProjectMainFile::BGDIR)
			.arg(fileName());
}

int BackgroundImageInfo::originalImageWidth() const
{
	return impl->m_originalImageWidth;
}

int BackgroundImageInfo::originalImageHeight() const
{
	return impl->m_originalImageHeight;
}

double BackgroundImageInfo::angle() const
{
	return impl->m_setting.angle;
}

void BackgroundImageInfo::informChange()
{
	emit isChanged();
	show();
}

void BackgroundImageInfo::fitImageToData()
{
	// fit to the data in the preprocessor window.
	double bounds[6];
	int vis = 0;
	auto preActor = impl->m_preProcessorActor;

	if (preActor != nullptr) {
		vis = preActor->GetVisibility();
		preActor->SetVisibility(0);
	}

	iricMainWindow()->preProcessorWindow()->dataModel()->graphicsView()->mainRenderer()->ComputeVisiblePropBounds(bounds);
	if (preActor != nullptr) {
		preActor->SetVisibility(vis);
	}

	auto o = offset();
	for (int i = 0; i < 2; ++i) {
		bounds[0 + i] += o.x();
		bounds[2 + i] += o.y();
	}

	impl->fitToRegion(bounds[0], bounds[1], bounds[2], bounds[3]);
}

void BackgroundImageInfo::mousePressEvent(vtkActor* /*actor*/, QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_setting.fixed) {return;}
	if (impl->m_isGeoReferencing) {return;}

	switch (event->button()) {
	case Qt::LeftButton:
		impl->m_mouseEventMode = Impl::MouseEventMode::Translating;
		v->setCursor(impl->m_moveCursor);
		break;
	case Qt::MidButton:
		impl->m_mouseEventMode = Impl::MouseEventMode::Zooming;
		v->setCursor(impl->m_zoomCursor);
		break;
	case Qt::RightButton:
		impl->m_mouseEventMode = Impl::MouseEventMode::Rotating;
		v->setCursor(impl->m_rotateCursor);
		break;
	default:
		// do nothing
		break;
	}

	impl->m_settingBeforeMouseEventStarts = impl->m_setting;

	impl->m_previousMousePosition = event->pos();
}

void BackgroundImageInfo::mouseMoveEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_setting.fixed) {return;}
	if (impl->m_isGeoReferencing) {return;}
	if (impl->m_mouseEventMode == Impl::MouseEventMode::Normal) {return;}

	auto view = dynamic_cast<VTK2DGraphicsView*>(v);

	auto currentPos = view->viewportToWorld(event->pos());
	auto previousPos = view->viewportToWorld(impl->m_previousMousePosition);

	if (impl->m_mouseEventMode == Impl::MouseEventMode::Rotating) {
		auto &s = impl->m_setting;

		auto origin = QPointF(s.positionX, s.positionY) - offset();

		auto p = previousPos - origin;
		auto q = currentPos - origin;

		double theta = p.x() * q.y() - p.y() * q.x();
		theta /= sqrt(p.x() * p.x() + p.y() * p.y()) * sqrt(q.x() * q.x() + q.y() * q.y());
		theta = asin(theta) * 180 / M_PI;

		// rotate theta degrees about z axis.
		impl->m_setting.angle += theta;
	} else if (impl->m_mouseEventMode == Impl::MouseEventMode::Zooming) {
		double scaleFactor = 1 + (impl->m_previousMousePosition.y() - event->y()) / 100.0;
		impl->m_setting.scale *= scaleFactor;
	} else if (impl->m_mouseEventMode == Impl::MouseEventMode::Translating) {
		auto diff = currentPos - previousPos;
		impl->m_setting.positionX += diff.x();
		impl->m_setting.positionY += diff.y();
	}
	applySettingToActor(actor);
	impl->m_previousMousePosition = event->pos();
	view->GetRenderWindow()->Render();
}

void BackgroundImageInfo::mouseReleaseEvent(vtkActor* /*actor*/, QMouseEvent* event, VTKGraphicsView* v)
{
	if (impl->m_setting.fixed) {return;}

	if (impl->m_isGeoReferencing && impl->m_isGeoReferenceSelectingPoint) {
		if (event->button() != Qt::LeftButton) {return;}

		double worldX = event->x();
		double worldY = event->y();
		auto view = dynamic_cast<VTK2DGraphicsView*>(v);
		view->viewportToWorld(worldX, worldY);
		emit gcpDefined(QPointF(worldX, worldY));

		std::unordered_set<std::vector<GcpTableRow>::size_type> indices;
		updateGeoReferencePointsActor(indices);
		return;
	}

	impl->m_mouseEventMode = Impl::MouseEventMode::Normal;

	iRICUndoStack::instance().push(new SetActorPropertyCommand(impl->m_setting, impl->m_settingBeforeMouseEventStarts, this));
	v->unsetCursor();
}

void BackgroundImageInfo::toggleFixState()
{
	auto& fixed = impl->m_setting.fixed;
	
	fixed = !fixed;
	impl->m_fixAction->setChecked(fixed);
	impl->updateFixActionIcon();
}

void BackgroundImageInfo::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
}

void BackgroundImageInfo::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void BackgroundImageInfo::setupActor(vtkActor* actor)
{
	actor->SetTexture(impl->m_texture);
	applySettingToActor(actor);
}

void BackgroundImageInfo::setupMapper(vtkPolyDataMapper* mapper)
{
	mapper->SetInputConnection(impl->m_mapToPlane->GetOutputPort());
}

void BackgroundImageInfo::applySettingToActor(vtkActor* actor)
{
	double pos[3];
	actor->GetPosition(pos);

	auto o = offset();

	pos[0] = impl->m_setting.positionX - o.x();
	pos[1] = impl->m_setting.positionY - o.y();

	double scale = impl->m_setting.scale / impl->m_resizeScale;

	actor->SetPosition(pos);
	actor->SetScale(scale, scale, 1);
	actor->SetOrientation(0, 0, impl->m_setting.angle);
}

void BackgroundImageInfo::setPreProcessorActor(vtkActor* actor)
{
	impl->m_preProcessorActor = actor;
}

void BackgroundImageInfo::deleteImageFile()
{
	QFile imagefile(fullFileName());
	if (imagefile.exists()) {
		imagefile.remove();
	}

	QFile scaledImageFile(Impl::scaledImageFileName(fullFileName()));
	if (scaledImageFile.exists()) {
		scaledImageFile.remove();
	}
}

QAction* BackgroundImageInfo::fixAction() const
{
	return impl->m_fixAction;
}

QAction* BackgroundImageInfo::fixActionWithIcon() const
{
	return impl->m_fixActionWithIcon;
}

QDialog* BackgroundImageInfo::propertyDialog()
{
	impl->m_settingBeforeMouseEventStarts = impl->m_setting;

	auto dialog = new BackgroundImageInfoDialog(this);
	dialog->setImageWidth(impl->m_originalImageWidth);
	dialog->setSetting(impl->m_setting);
	return dialog;
}

void BackgroundImageInfo::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<BackgroundImageInfoDialog*>(d);
	iRICUndoStack::instance().push(new SetActorPropertyCommand(dialog->setting(), impl->m_settingBeforeMouseEventStarts, this));
}

void BackgroundImageInfo::applyOffset(double /*x_diff*/, double /*y_diff*/)
{
	// do nothing
}

BackgroundImageInfoGeoreferenceDialog* BackgroundImageInfo::georeferenceDialog(QWidget* w)
{
	impl->m_geoReferenceParentWindow = w;
	return new BackgroundImageInfoGeoreferenceDialog(this, w);
}

void BackgroundImageInfo::updateGeoReferencePointsActor(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indice)
{
	auto table = gcpTable();
	std::vector<QPointF> points;
	for (GcpTableRow row : *table) {
		QPointF p(row.destX, row.destY);
		points.push_back(p);
	}
	impl->m_geoReferencePointsActor.setLine(points);

	std::vector<QPointF> selectedPoints;
	for (auto idx : indice) {
		if (! (idx < table->size())) {continue;}
		auto row = table->at(idx);
		QPointF p(row.destX, row.destY);
		selectedPoints.push_back(p);
	}
	impl->m_geoReferenceSelectedPointsActor.setLine(selectedPoints);

	impl->m_geoReferenceGraphicsView->render();
}

std::vector<GcpTableRow>* BackgroundImageInfo::gcpTable()
{
	return m_georeferenceDialog->gcpTable();
}

GcpTableModel* BackgroundImageInfo::gcpTableModel()
{
	return m_georeferenceDialog->gcpTableModel();
}

void BackgroundImageInfo::handleGeoreferenceDialogAccepted(BackgroundImageInfoGeoreferenceDialog* d)
{
	iRICUndoStack::instance().push(new SetActorPropertyCommand(d->setting(), impl->m_settingBeforeMouseEventStarts , this));
}

void BackgroundImageInfo::selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices)
{
	updateGeoReferencePointsActor(indices);
}

void BackgroundImageInfo::startGcpSelect()
{
	QMessageBox::information(impl->m_geoReferenceParentWindow, tr("Information"), tr("Select a corresponding point on Main Window."));
	impl->m_isGeoReferenceSelectingPoint = true;
}

void BackgroundImageInfo::showGeoreferenceDialog(vtkActor* actor, VTKGraphicsView* v, double minDepth, double maxDepth, QWidget* w)
{
	if (m_georeferenceDialog == nullptr) {
		m_georeferenceDialog = georeferenceDialog(w);

		auto f = [&]() {handleGeoreferenceDialogAccepted(m_georeferenceDialog);};
		connect(m_georeferenceDialog, &BackgroundImageInfoGeoreferenceDialog::accepted, this, f);
		connect(m_georeferenceDialog, SIGNAL(destroyed()), this, SLOT(handleGeoreferenceDialogClosed()));
	}
	impl->m_geoReferenceActor = actor;
	impl->m_geoReferenceGraphicsView = v;
	impl->m_geoReferencePointsActor.pointsActor()->SetPosition(0, 0, maxDepth);
	impl->m_geoReferenceGraphicsView->mainRenderer()->AddActor(impl->m_geoReferencePointsActor.pointsActor());
	impl->m_geoReferenceSelectedPointsActor.pointsActor()->SetPosition(0, 0, (minDepth + maxDepth) * 0.5);
	impl->m_geoReferenceGraphicsView->mainRenderer()->AddActor(impl->m_geoReferenceSelectedPointsActor.pointsActor());

	std::unordered_set<std::vector<GcpTableRow>::size_type> indices;
	updateGeoReferencePointsActor(indices);

	impl->m_isGeoReferencing = true;
	impl->m_isGeoReferenceSelectingPoint = false;

	m_georeferenceDialog->show();
	m_georeferenceDialog->raise();
	m_georeferenceDialog->activateWindow();
}

void BackgroundImageInfo::handleGeoreferenceDialogClosed()
{
	impl->m_isGeoReferencing = false;
	impl->m_geoReferenceGraphicsView->mainRenderer()->RemoveActor(impl->m_geoReferencePointsActor.pointsActor());
	impl->m_geoReferenceGraphicsView->mainRenderer()->RemoveActor(impl->m_geoReferenceSelectedPointsActor.pointsActor());

	impl->m_geoReferenceActor = nullptr;
	m_georeferenceDialog = nullptr;
	show();

	emit isGeoreferenceDialogClosed();
}

void BackgroundImageInfo::show()
{
	if (! impl->m_isGeoReferencing) {return;}

	m_hide = false;
	impl->m_geoReferenceActor->VisibilityOn();
	impl->m_geoReferenceGraphicsView->render();
	emit isVisibilityChanged();
}

void BackgroundImageInfo::toggleVisibility()
{
	if (! impl->m_isGeoReferencing) {return;}

	m_hide = ! m_hide;
	int vis = 1;
	if (m_hide) {vis = 0;}

	impl->m_geoReferenceActor->SetVisibility(vis);
	impl->m_geoReferenceGraphicsView->render();
	emit isVisibilityChanged();
}

bool BackgroundImageInfo::isVisible()
{
	return ! m_hide;
}
