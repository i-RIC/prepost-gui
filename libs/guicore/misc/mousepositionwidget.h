#ifndef MOUSEPOSITIONWIDGET_H
#define MOUSEPOSITIONWIDGET_H

#include "../guicore_global.h"
#include "../project/projectdata.h"
#include "../project/projectmainfile.h"

#include <QWidget>

class ProjectData;

class QPointF;

namespace Ui
{
	class MousePositionWidget;
}

class GUICOREDLL_EXPORT MousePositionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MousePositionWidget(QWidget* parent = nullptr);
	~MousePositionWidget();

	void setProjectData(ProjectData* data);
	void setAxisLabels(const QString& xLabel, const QString& yLabel);

public slots:
	void updatePosition(const QPointF& position);
	void updateCrossSectionPosition(const QPointF& position);
	void clear();

private:
	ProjectData* m_projectData;
	QString m_xLabel;
	QString m_yLabel;
	Ui::MousePositionWidget* ui;
};

#endif // MOUSEPOSITIONWIDGET_H
