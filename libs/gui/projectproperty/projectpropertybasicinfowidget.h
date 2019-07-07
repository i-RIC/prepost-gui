#ifndef PROJECTPROPERTYBASICINFOWIDGET_H
#define PROJECTPROPERTYBASICINFOWIDGET_H

#include <QWidget>

namespace Ui
{
	class ProjectPropertyBasicInfoWidget;
}

class ProjectData;

class QDateTime;

class ProjectPropertyBasicInfoWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ProjectPropertyBasicInfoWidget(QWidget* parent = nullptr);
	~ProjectPropertyBasicInfoWidget();
	void setProjectData(ProjectData* data);

private slots:
	void showSelectCoordinateSystemDialog();
	void showSetOffsetDialog();
	void showTimeDialog();

private:
	void updateCoordinateSystem();
	void updateCoordinateOffset();
	void updateTimeString();

	ProjectData* m_projectData;
	Ui::ProjectPropertyBasicInfoWidget* ui;
};

#endif // PROJECTPROPERTYBASICINFOWIDGET_H
