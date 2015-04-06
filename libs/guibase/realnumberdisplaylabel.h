#ifndef REALNUMBERDISPLAYLABEL_H
#define REALNUMBERDISPLAYLABEL_H

#include "guibase_global.h"

#include <QLabel>

class GUIBASEDLL_EXPORT RealNumberDisplayLabel : public QLabel
{
	Q_OBJECT
public:
	enum ClearMode {
		cmEmpty,
		cmLines,
	};
	explicit RealNumberDisplayLabel(QWidget *parent = 0);
	ClearMode clearMode(){return m_clearMode;}
	void setClearMode(ClearMode cm){m_clearMode = cm;}
public slots:
	void setValue(double value);
	void clear();
private:
	ClearMode m_clearMode;
};

#endif // REALNUMBERDISPLAYLABEL_H
