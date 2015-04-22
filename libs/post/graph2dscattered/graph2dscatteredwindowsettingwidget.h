#ifndef GRAPH2DSCATTEREDWINDOWSETTINGWIDGET_H
#define GRAPH2DSCATTEREDWINDOWSETTINGWIDGET_H

#include "graph2dscatteredwindowresultsetting.h"

#include <QWidget>

namespace Ui {
class Graph2dScatteredWindowSettingWidget;
}

class Graph2dScatteredWindowSettingWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit Graph2dScatteredWindowSettingWidget(QWidget *parent = nullptr);
	~Graph2dScatteredWindowSettingWidget();
	void setSetting(Graph2dScatteredWindowResultSetting::Setting* setting);
	void clear();

signals:
	void labelChanged(const QString& label);
	void deleteButtonClicked();

private slots:
	void updateLabel(const QString& label);
	void updateAxis();
	void updateSymbolStyle(int index);
	void updateSymbolSize(int size);
	void updateColor(const QColor& color);

private:
	Graph2dScatteredWindowResultSetting::Setting* m_setting;
	Ui::Graph2dScatteredWindowSettingWidget *ui;
};

#endif // GRAPH2DSCATTEREDWINDOWSETTINGWIDGET_H
