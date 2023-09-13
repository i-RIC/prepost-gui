#include "windowsizestandardsetting.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

WindowSizeStandardSetting::WindowSizeStandardSetting() :
	WindowSizeStandardSetting {640, 480}
{}

WindowSizeStandardSetting::WindowSizeStandardSetting(int w, int h, const QString& c) :
	width {w},
	height {h},
	comment {c}
{}

QString WindowSizeStandardSetting::toString() const
{
	QJsonObject object;
	object.insert("w", width);
	object.insert("h", height);
	object.insert("c", comment);
	QJsonDocument doc(object);
	return QString(doc.toJson());
}

void WindowSizeStandardSetting::fromString(const QString setting)
{
	QJsonParseError error;
	auto data = QJsonDocument::fromJson(setting.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError) {return;}

	auto obj = data.object();
	width = obj.value("w").toInt();
	height = obj.value("h").toInt();
	comment = obj.value("c").toString();
}

QString WindowSizeStandardSetting::labelString() const
{
	auto ret = QString("%1 x %2").arg(width).arg(height);

	if (!comment.isEmpty()) {
		ret = QString("%1 (%2)").arg(ret).arg(comment);
	}

	return ret;
}

QSize WindowSizeStandardSetting::size() const
{
	return QSize(width, height);
}

bool WindowSizeStandardSetting::operator<(const WindowSizeStandardSetting& s) const
{
	if (width != s.width) {return width < s.width;}
	return height < s.height;
}

bool WindowSizeStandardSetting::operator==(const WindowSizeStandardSetting& s) const
{
	if (width != s.width) {return false;}
	if (height != s.height) {return false;}
	if (comment != s.comment) {return false;}

	return true;
}

std::vector<WindowSizeStandardSetting> WindowSizeStandardSetting::buildDefaultSettings()
{
	std::vector<WindowSizeStandardSetting> ret;

	ret.push_back(WindowSizeStandardSetting(640, 480, "VGA 4:3"));
	ret.push_back(WindowSizeStandardSetting(800, 600, "SVGA 4:3"));
	ret.push_back(WindowSizeStandardSetting(960, 540, "QHD 16:9"));
	ret.push_back(WindowSizeStandardSetting(1024, 576, "WSVGA 16:9"));
	ret.push_back(WindowSizeStandardSetting(1024, 768, "XGA 4:3"));
	ret.push_back(WindowSizeStandardSetting(1280, 720, "HD 16:9"));
	ret.push_back(WindowSizeStandardSetting(1920, 1080, "FHD 16:9"));
	ret.push_back(WindowSizeStandardSetting(3200, 2400, "QUXGA 4:3"));
	ret.push_back(WindowSizeStandardSetting(3840, 2160, "4K 16:9"));

	return ret;
}
