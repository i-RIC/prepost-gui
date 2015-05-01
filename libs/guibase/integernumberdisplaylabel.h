#ifndef INTEGERNUMBERDISPLAYLABEL_H
#define INTEGERNUMBERDISPLAYLABEL_H

#include "guibase_global.h"

#include <QLabel>

class GUIBASEDLL_EXPORT IntegerNumberDisplayLabel : public QLabel
{
	Q_OBJECT
public:
	enum ClearMode {
		cmEmpty,
		cmLines
	};
	explicit IntegerNumberDisplayLabel(QWidget* parent = nullptr);
	ClearMode clearMode() {return m_clearMode;}
	void setClearMode(ClearMode cm) {m_clearMode = cm;}

public slots:
	void setValue(unsigned int value);
	void clear();

private:
	ClearMode m_clearMode;
};

#endif // INTEGERNUMBERDISPLAYLABEL_H
