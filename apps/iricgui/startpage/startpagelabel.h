#ifndef STARTPAGELABEL_H
#define STARTPAGELABEL_H

#include <QWidget>
#include <QString>

class StartPageLabel : public QWidget
{
	Q_OBJECT
public:
	static const int IMAGEMARGIN = 20;
	explicit StartPageLabel(QWidget* parent = nullptr);
	~StartPageLabel();
	void setTitle(const QString& title) {m_title = title;}
	void setSubtitle(const QString& title) {m_subtitle = title;}
	QSize sizeHint() const;
protected:
	void mousePressEvent(QMouseEvent*);
	void paintEvent(QPaintEvent*);
signals:
	void clicked();
private:
	QString m_title;
	QString m_subtitle;
};

#endif // STARTPAGELABEL_H
