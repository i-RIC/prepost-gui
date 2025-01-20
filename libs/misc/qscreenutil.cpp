#include "qscreenutil.h"

#include <QGuiApplication>
#include <QScreen>

namespace {

const int MARGIN_WIDTH = 20;
const int MARGIN_HEIGHT = 100;

}

QSize QScreenUtil::standardSizeWithinScreen()
{
	return sizeWithinScreen(900, 600);
}

QSize QScreenUtil::sizeWithinScreen(int w, int h)
{
	auto geometry = QGuiApplication::primaryScreen()->geometry();
	int width = qMin(w, geometry.width() - MARGIN_WIDTH);
	int height = qMin(h, geometry.height() - MARGIN_HEIGHT);

	return QSize(width, height);
}

QScreenUtil::QScreenUtil()
{

}
