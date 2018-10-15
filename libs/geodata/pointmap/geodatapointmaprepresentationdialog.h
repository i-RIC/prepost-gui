#ifndef GEODATAPOINTMAPREPRESENTATIONDIALOG_H
#define GEODATAPOINTMAPREPRESENTATIONDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataPointmapRepresentationDialog;
}

class GeoDataPointmapRepresentationDialog : public QDialog
{
	Q_OBJECT

public:
	enum Representation {Points, Wireframe, Surface};

	GeoDataPointmapRepresentationDialog(QWidget* parent = nullptr);
	~GeoDataPointmapRepresentationDialog();

	void setRepresentation(Representation r);
	void setOpacityPercent(int opacity);
	void setHideBreakLines(bool hide);
	void setPointSize(int size);
	Representation representation();
	int opacityPercent();
	bool hideBreakLines();
	int pointSize();

private:
	Ui::GeoDataPointmapRepresentationDialog* ui;
};

#endif // GEODATAPOINTMAPREPRESENTATIONDIALOG_H
