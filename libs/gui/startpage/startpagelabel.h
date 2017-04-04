#ifndef STARTPAGELABEL_H
#define STARTPAGELABEL_H

#include <QWidget>

class QString;

class StartPageLabel : public QWidget
{
	Q_OBJECT

public:
	static const int IMAGEMARGIN = 20;

	explicit StartPageLabel(QWidget* parent = nullptr);
	~StartPageLabel();

	void setTitle(const QString& title);
	void setSubtitle(const QString& title);

	QSize sizeHint() const override;

protected:
	void mousePressEvent(QMouseEvent*) override;
	void paintEvent(QPaintEvent*) override;

signals:
	void clicked();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include  "private/startpagelabel_impl.h"
#endif // _DEBUG

#endif // STARTPAGELABEL_H
