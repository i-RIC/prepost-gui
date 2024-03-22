#include "post2dwindowparticleimagedataitem_impl.h"

#include <misc/mathsupport.h>

#include <vtkPlaneSource.h>
#include <vtkQImageToImageSource.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

Post2dWindowParticleImageDataItem::Impl::Impl()
{}

Post2dWindowParticleImageDataItem::Impl::~Impl()
{}


vtkPoints* Post2dWindowParticleImageDataItem::Impl::buildCornerPoints(const QPointF& pos, double value, double angle)
{
	double targetSize = calcSize(value);
	double origSize = 0;
	if (m_setting.sizeTarget == Setting::SizeTarget::Width) {
		origSize = m_setting.image.width();
	} else if (m_setting.sizeTarget == Setting::SizeTarget::Height) {
		origSize = m_setting.image.height();
	}
	double scale = targetSize / origSize;

	QPointF dx(scale, 0);
	QPointF dy(0, scale);
	auto dx2 = iRIC::rotateVector(dx, angle);
	auto dy2 = iRIC::rotateVector(dy, angle);

	double halfWidth = m_setting.image.width() * 0.5;
	double halfHeight = m_setting.image.height() * 0.5;

	std::vector<QPointF> corners;
	if (m_setting.anchorPosition == Setting::AnchorPosition::Center) {
		corners = buildCornerPointsCenter(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::TopLeft) {
		corners = buildCornerPointsTopLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Top) {
		corners = buildCornerPointsTop(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::TopRight) {
		corners = buildCornerPointsTopRight(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Left) {
		corners = buildCornerPointsLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::TopRight) {
		corners = buildCornerPointsTopRight(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Left) {
		corners = buildCornerPointsLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Right) {
		corners = buildCornerPointsRight(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::BottomLeft) {
		corners = buildCornerPointsBottomLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Bottom) {
		corners = buildCornerPointsBottom(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::BottomRight) {
		corners = buildCornerPointsBottomRight(pos, dx2, dy2, halfWidth, halfHeight);
	}
	auto ret = vtkPoints::New();
	ret->SetDataTypeToDouble();
	for (const auto& corner : corners) {
		ret->InsertNextPoint(corner.x(), corner.y(), 0);
	}
	return ret;
}

QPointF Post2dWindowParticleImageDataItem::Impl::buildBottomLeftCorner(const QPointF& pos, double value, double angle)
{
	double targetSize = calcSize(value);
	double origSize = 0;
	if (m_setting.sizeTarget == Setting::SizeTarget::Width) {
		origSize = m_setting.image.width();
	} else if (m_setting.sizeTarget == Setting::SizeTarget::Height) {
		origSize = m_setting.image.height();
	}
	double scale = targetSize / origSize;

	QPointF dx(scale, 0);
	QPointF dy(0, scale);
	auto dx2 = iRIC::rotateVector(dx, angle);
	auto dy2 = iRIC::rotateVector(dy, angle);

	double halfWidth = m_setting.image.width() * 0.5;
	double halfHeight = m_setting.image.height() * 0.5;

	std::vector<QPointF> corners;
	if (m_setting.anchorPosition == Setting::AnchorPosition::Center) {
		corners = buildCornerPointsCenter(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::TopLeft) {
		corners = buildCornerPointsTopLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Top) {
		corners = buildCornerPointsTop(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::TopRight) {
		corners = buildCornerPointsTopRight(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Left) {
		corners = buildCornerPointsLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::TopRight) {
		corners = buildCornerPointsTopRight(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Left) {
		corners = buildCornerPointsLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Right) {
		corners = buildCornerPointsRight(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::BottomLeft) {
		corners = buildCornerPointsBottomLeft(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::Bottom) {
		corners = buildCornerPointsBottom(pos, dx2, dy2, halfWidth, halfHeight);
	} else if (m_setting.anchorPosition == Setting::AnchorPosition::BottomRight) {
		corners = buildCornerPointsBottomRight(pos, dx2, dy2, halfWidth, halfHeight);
	}

	return corners.at(0);
}

double Post2dWindowParticleImageDataItem::Impl::calcScale(double value)
{
	double targetSize = calcSize(value);
	double origSize = 0;
	if (m_setting.sizeTarget == Setting::SizeTarget::Width) {
		origSize = m_setting.image.width();
	} else if (m_setting.sizeTarget == Setting::SizeTarget::Height) {
		origSize = m_setting.image.height();
	}

	return targetSize / origSize;
}

double Post2dWindowParticleImageDataItem::Impl::calcSize(double value)
{
	const auto& pairs = m_setting.valueSizePairs;
	if (value < pairs.begin()->value) {
		if (! m_setting.fillLower) {return -1;}

		return pairs.begin()->size;
	}
	if (value > pairs.rbegin()->value) {
		if (! m_setting.fillUpper) {return -1;}

		return pairs.rbegin()->size;
	}
	for (int i = 0; i < static_cast<int> (pairs.size()) - 1; ++i) {
		auto pair1 = pairs.at(i);
		auto pair2 = pairs.at(i + 1);

		if (pair1.value <= value && value <= pair2.value) {
			double r = (value - pair1.value) / (pair2.value - pair1.value);
			return pair1.size + (pair2.size - pair1.size) * r;
		}
	}

	return -1;
}

vtkTexture* Post2dWindowParticleImageDataItem::Impl::buildTexture()
{
	auto imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
	imgToImg->SetQImage(&m_setting.image);
	imgToImg->Update();

	auto texture = vtkTexture::New();
	texture->SetInputConnection(imgToImg->GetOutputPort());
	texture->InterpolateOn();
	texture->Update();

	return texture;
}

vtkPolyData* Post2dWindowParticleImageDataItem::Impl::buildPlane()
{
	auto& img = m_setting.image;

	auto planeSource = vtkSmartPointer<vtkPlaneSource>::New();
	// planeSource->SetCenter(-100, -100, -100);
	// planeSource->SetOrigin(-100, -100, -100);
	// planeSource->SetPoint1(-90,  -100, -100);
	// planeSource->SetPoint2(-100,  -90, -100);
	planeSource->SetCenter(0.5, 0.5, 0);
	planeSource->SetPoint1(img.width(), 0, 0);
	planeSource->SetPoint2(0, img.height(), 0);
	planeSource->SetNormal(0, 0, 1);
	/*
	planeSource->SetCenter(0.5, 0.5, 0);
	planeSource->SetOrigin(0, 0, 0);
	planeSource->SetPoint1(img.width(), 0, 0);
	planeSource->SetPoint2(0, img.height(), 0);
	*/

	planeSource->Update();
	auto plane = planeSource->GetOutput();
	plane->Register(nullptr);

	return plane;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsCenter(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfWidth * dx - halfHeight * dy);
	ret.push_back(point + halfWidth * dx - halfHeight * dy);
	ret.push_back(point - halfWidth * dx + halfHeight * dy);
	ret.push_back(point + halfWidth * dx + halfHeight * dy);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsTopLeft(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfHeight * dy * 2);
	ret.push_back(point + halfWidth * dx * 2 - halfHeight * dy * 2);
	ret.push_back(point);
	ret.push_back(point + halfWidth * dx * 2);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsTop(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfWidth * dx - halfHeight * dy * 2);
	ret.push_back(point + halfWidth * dx - halfHeight * dy * 2);
	ret.push_back(point - halfWidth * dx);
	ret.push_back(point + halfWidth * dx);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsTopRight(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfWidth * dx * 2 - halfHeight * dy * 2);
	ret.push_back(point - halfHeight * dy * 2);
	ret.push_back(point - halfWidth * dx * 2);
	ret.push_back(point);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsLeft(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfHeight * dy);
	ret.push_back(point + halfWidth * dx * 2 - halfHeight * dy);
	ret.push_back(point + halfHeight * dy);
	ret.push_back(point + halfWidth * dx * 2 + halfHeight * dy);
	return ret;
}
std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsRight(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfWidth * dx * 2 - halfHeight * dy);
	ret.push_back(point - halfHeight * dy);
	ret.push_back(point - halfWidth * dx * 2 + halfHeight * dy);
	ret.push_back(point + halfHeight * dy);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsBottomLeft(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point);
	ret.push_back(point + halfWidth * dx * 2);
	ret.push_back(point + halfHeight * dy * 2);
	ret.push_back(point + halfWidth * dx * 2 + halfHeight * dy * 2);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsBottom(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfWidth * dx);
	ret.push_back(point + halfWidth * dx);
	ret.push_back(point - halfWidth * dx + halfHeight * dy * 2);
	ret.push_back(point + halfWidth * dx + halfHeight * dy * 2);
	return ret;
}

std::vector<QPointF> Post2dWindowParticleImageDataItem::Impl::buildCornerPointsBottomRight(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight)
{
	std::vector<QPointF> ret;
	ret.push_back(point - halfWidth * dx * 2);
	ret.push_back(point);
	ret.push_back(point - halfWidth * dx * 2 + halfHeight * dy * 2);
	ret.push_back(point + halfHeight * dy * 2);
	return ret;
}
