#ifndef PREPROCESSORVIEW_H
#define PREPROCESSORVIEW_H

#include "../../data/base/topview.h"

class PreProcessorView : public TopView
{
	Q_OBJECT

public:
	PreProcessorView(QWidget* parent = nullptr);
	~PreProcessorView();

	QSize sizeHint() const override;
};

#endif // PREPROCESSORVIEW_H
