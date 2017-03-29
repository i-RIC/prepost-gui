#ifndef MOUSEPOSITIONWIDGET_H
#define MOUSEPOSITIONWIDGET_H

#include <QWidget>

class Project;

class QPointF;

namespace Ui
{
	class MousePositionWidget;
}

class MousePositionWidget : public QWidget
{
	Q_OBJECT

public:
	MousePositionWidget(QWidget* parent = nullptr);
	~MousePositionWidget();

	void setProject(Project* p);

public slots:
	void updatePosition(const QPointF& pos);
	void clear();

private:
	Project* m_project;
	Ui::MousePositionWidget* ui;
};

#endif // MOUSEPOSITIONWIDGET_H
