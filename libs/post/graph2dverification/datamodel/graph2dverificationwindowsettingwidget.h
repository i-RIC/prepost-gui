#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWSETTINGWIDGET_H
#define GRAPH2DVERIFICATIONWINDOWSETTINGWIDGET_H

#include "../graph2dverificationwindowresultsetting.h"

#include <QWidget>

namespace Ui
{
	class Graph2dVerificationWindowSettingWidget;
}

class Graph2dVerificationWindowSettingWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Graph2dVerificationWindowSettingWidget(QWidget* parent = nullptr);
	~Graph2dVerificationWindowSettingWidget();
	void setSetting(Graph2dVerificationWindowResultSetting::Setting* setting);
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
	Graph2dVerificationWindowResultSetting::Setting* m_setting;
	Ui::Graph2dVerificationWindowSettingWidget* ui;
};

#endif // GRAPH2DVERIFICATIONWINDOWSETTINGWIDGET_H
#endif
