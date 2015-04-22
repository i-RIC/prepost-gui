#ifndef GRAPH2DSCATTEREDWINDOWCONTROLWIDGET_H
#define GRAPH2DSCATTEREDWINDOWCONTROLWIDGET_H

#include <QWidget>
//#include "graph2dscatteredwindowresultsetting.h"

class QDomNode;
class QXmlStreamWriter;

namespace Ui {
    class Graph2dScatteredWindowControlWidget;
}

class Graph2dScatteredWindowControlWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Graph2dScatteredWindowControlWidget(QWidget *parent = nullptr);
	~Graph2dScatteredWindowControlWidget();
	void fitSize();

signals:
	void dataSourceButtonClicked();
	void axisButtonClicked();
	void drawButtonClicked();

private:
	Ui::Graph2dScatteredWindowControlWidget *ui;
};

#endif // GRAPH2DSCATTEREDWINDOWCONTROLWIDGET_H
