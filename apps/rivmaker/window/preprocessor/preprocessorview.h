#ifndef PREPROCESSORVIEW_H
#define PREPROCESSORVIEW_H

#include <QWidget>

class PreProcessorView : public QWidget
{
	Q_OBJECT

public:
	PreProcessorView(QWidget* parent = nullptr);
	~PreProcessorView();
};

#endif // PREPROCESSORVIEW_H
