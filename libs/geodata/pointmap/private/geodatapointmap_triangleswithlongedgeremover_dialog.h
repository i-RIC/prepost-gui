#ifndef GEODATAPOINTMAP_TRIANGLESWITHLONGEDGEREMOVER_DIALOG_H
#define GEODATAPOINTMAP_TRIANGLESWITHLONGEDGEREMOVER_DIALOG_H

#include "geodatapointmap_triangleswithlongedgeremover.h"

#include <QDialog>

namespace Ui {
class GeoDataPointmap_TrianglesWithLongEdgeRemover_Dialog;
}

class QAbstractButton;

class GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog : public QDialog
{
	Q_OBJECT

public:
	explicit Dialog(TrianglesWithLongEdgeRemover* remover);
	~Dialog();

	void setlimitLength(double length);

public slots:
	void accept() override;
	void handleButtonClick(QAbstractButton* button);

private:
	double limitLength() const;
	void preview();

	TrianglesWithLongEdgeRemover* m_remover;
	Ui::GeoDataPointmap_TrianglesWithLongEdgeRemover_Dialog *ui;
};

#endif // GEODATAPOINTMAP_TRIANGLESWITHLONGEDGEREMOVER_DIALOG_H
