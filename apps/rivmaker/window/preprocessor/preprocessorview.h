#ifndef PREPROCESSORVIEW_H
#define PREPROCESSORVIEW_H

#include "../../data/base/view.h"

class PreProcessorView : public View
{
	Q_OBJECT

public:
	PreProcessorView(QWidget* parent = nullptr);
	~PreProcessorView();
};

#endif // PREPROCESSORVIEW_H
