#include "preprocessorview.h"

PreProcessorView::PreProcessorView(QWidget* parent) :
	TopView(parent)
{
}

PreProcessorView::~PreProcessorView()
{}


QSize PreProcessorView::sizeHint() const
{
	return QSize(640, 480);
}
