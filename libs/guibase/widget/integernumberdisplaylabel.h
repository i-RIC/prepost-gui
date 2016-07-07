#ifndef INTEGERNUMBERDISPLAYLABEL_H
#define INTEGERNUMBERDISPLAYLABEL_H

#include "../guibase_global.h"

#include <QLabel>

/// Label to show integer number
class GUIBASEDLL_EXPORT IntegerNumberDisplayLabel : public QLabel
{
	Q_OBJECT
public:
	enum ClearMode {
		cmEmpty,    ///< Display nothing
		cmLines     ///< Display lines, like "-----"
	};
	explicit IntegerNumberDisplayLabel(QWidget* parent = nullptr);

	ClearMode clearMode() const;
	void setClearMode(ClearMode cm);

public slots:
	void setValue(unsigned int value);
	void clear();

private:
	ClearMode m_clearMode;
};

#endif // INTEGERNUMBERDISPLAYLABEL_H
