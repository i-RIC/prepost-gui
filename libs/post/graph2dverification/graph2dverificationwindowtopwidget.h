#ifndef GRAPH2DVERIFICATIONWINDOWTOPWIDGET_H
#define GRAPH2DVERIFICATIONWINDOWTOPWIDGET_H

#include <QWidget>
#include "graph2dverificationwindowresultsetting.h"

namespace Ui {
class Graph2dVerificationWindowTopWidget;
}

class Graph2dVerificationWindowTopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Graph2dVerificationWindowTopWidget(QWidget *parent = 0);
    ~Graph2dVerificationWindowTopWidget();
	void setSetting(const Graph2dVerificationWindowResultSetting& setting);

signals:
	void graphTypeChanged(int index);

private slots:
	void handleIndexChanged(int index);

private:
    Ui::Graph2dVerificationWindowTopWidget *ui;

};

#endif // GRAPH2DVERIFICATIONWINDOWTOPWIDGET_H
