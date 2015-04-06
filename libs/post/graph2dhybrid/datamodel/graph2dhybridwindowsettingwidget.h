#ifndef GRAPH2DHYBRIDWINDOWSETTINGWIDGET_H
#define GRAPH2DHYBRIDWINDOWSETTINGWIDGET_H

#include "../graph2dhybridwindowresultsetting.h"

#include <QWidget>

namespace Ui {
class Graph2dHybridWindowSettingWidget;
}

class Graph2dHybridWindowSettingWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit Graph2dHybridWindowSettingWidget(QWidget *parent = 0);
	~Graph2dHybridWindowSettingWidget();
	void setSetting(Graph2dHybridWindowResultSetting::Setting* setting);
	void setId(const QString& id);
	void setupForResult();
	void setupForResultCopy();
	void setupForExternalData();
	void clear();
signals:
	void labelChanged(const QString& label);
	void deleteButtonClicked();
private slots:
	void updateLabel(const QString& label);
	void updateAxis();
	void updateDisplayStyle();
	void updateLineStyle(int index);
	void updateLineWidth(int width);
	void updateSymbolStyle(int index);
	void updateColor(const QColor& color);
    void updateBarChart(bool bar);

private:
	Graph2dHybridWindowResultSetting::Setting* m_setting;
	Ui::Graph2dHybridWindowSettingWidget *ui;
};

#endif // GRAPH2DHYBRIDWINDOWSETTINGWIDGET_H
