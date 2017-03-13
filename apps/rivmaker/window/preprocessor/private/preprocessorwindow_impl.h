#ifndef PREPROCESSORWINDOW_IMPL_H
#define PREPROCESSORWINDOW_IMPL_H

#include "../preprocessormodel.h"
#include "../preprocessorview.h"
#include "../preprocessorwindow.h"

class ObjectBrowser;

class PreProcessorWindow::Impl
{
public:
	Impl(PreProcessorWindow* w);
	~Impl();

	ObjectBrowser* m_objectBrowser;
	PreProcessorView m_view;
	PreProcessorModel m_model;
};

#endif // PREPROCESSORWINDOW_IMPL_H
