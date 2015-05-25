#ifndef BACKGROUNDIMAGEINFODIALOG_H
#define BACKGROUNDIMAGEINFODIALOG_H

#include <QDialog>

namespace Ui
{
	class BackgroundImageInfoDialog;
}

class QAbstractButton;
class BackgroundImageInfo;

class BackgroundImageInfoDialog : public QDialog
{
	Q_OBJECT

public:
	explicit BackgroundImageInfoDialog(QWidget* parent = nullptr);
	~BackgroundImageInfoDialog();
	void setInfo(BackgroundImageInfo* info) {m_info = info;}
	void setImageWidth(int width) {m_imageWidth = width;}
	void setProperties(double lbX, double lbY, double scale, double angle);
	double leftBottomX() const {return m_leftbottomX;}
	double leftBottomY() const {return m_leftbottomY;}
	double scale() const {return m_scale;}
	double angle() const {return m_angle;}

	double origLeftBottomX() const {return m_origLeftbottomX;}
	double origLeftBottomY() const {return m_origLeftbottomY;}
	double origScale() const {return m_origScale;}
	double origAngle() const {return m_origAngle;}

	void updateScaleAndAngle();
	void updateRightBottom();
	void accept() override;
	void reject() override;
	void reset();
	void apply();

private slots:
	void update();
	void handleButtonBoxClick(QAbstractButton* button);

private:
	double m_leftbottomX;
	double m_leftbottomY;
	double m_scale;
	double m_angle;
	int m_imageWidth;

	double m_origLeftbottomX;
	double m_origLeftbottomY;
	double m_origScale;
	double m_origAngle;

	BackgroundImageInfo* m_info;
	Ui::BackgroundImageInfoDialog* ui;
};

#endif // BACKGROUNDIMAGEINFODIALOG_H
