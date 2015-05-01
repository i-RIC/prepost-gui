#ifndef RAWDATAPOINTMAPREPRESENTATIONDIALOG_H
#define RAWDATAPOINTMAPREPRESENTATIONDIALOG_H

#include <QDialog>

namespace Ui
{
	class RawDataPointmapRepresentationDialog;
}

class RawDataPointmapRepresentationDialog : public QDialog
{
	Q_OBJECT

public:
	enum Representation {Points, Wireframe, Surface};
	RawDataPointmapRepresentationDialog(QWidget* parent = nullptr);
	~RawDataPointmapRepresentationDialog();
	void setRepresentation(Representation r);
	void setOpacityPercent(int opacity);
	void setHideBreakLines(bool hide);
	void setPointSize(int size);
	Representation representation();
	int opacityPercent();
	bool hideBreakLines();
	int pointSize();

private:
	Ui::RawDataPointmapRepresentationDialog* ui;
};

#endif // RAWDATAPOINTMAPREPRESENTATIONDIALOG_H
