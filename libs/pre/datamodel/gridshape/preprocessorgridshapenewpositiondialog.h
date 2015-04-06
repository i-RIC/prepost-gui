#ifndef PREPROCESSORGRIDSHAPENEWPOSITIONDIALOG_H
#define PREPROCESSORGRIDSHAPENEWPOSITIONDIALOG_H

#include <QDialog>
#include <QVector2D>

namespace Ui {
	class PreProcessorGridShapeNewPositionDialog;
}

class PreProcessorGridShapeNewPositionDialog : public QDialog {
	Q_OBJECT
public:
	PreProcessorGridShapeNewPositionDialog(QWidget *parent = 0);
	~PreProcessorGridShapeNewPositionDialog();

	const QVector2D position();
	void setPosition(const QVector2D& position){setPosition(position.x(), position.y());}
	void setPosition(double x, double y);
	void setIndex(int index);
	void setIJ(int i, int j);
private:
	Ui::PreProcessorGridShapeNewPositionDialog *ui;
};

#endif // PREPROCESSORGRIDSHAPENEWPOSITIONDIALOG_H
