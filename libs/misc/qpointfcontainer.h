#ifndef QPOINTFCONTAINER_H
#define QPOINTFCONTAINER_H

#include "misc_global.h"
#include "doublecontainer.h"
#include "xmlattributecontainer.h"

#include <QPointF>

class MISCDLL_EXPORT QPointFContainer : public XmlAttributeContainer
{
public:
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	QPointFContainer();
	/// Constructor
	QPointFContainer(const QString& name);
	/// Constructor
	QPointFContainer(const QPointF& val);
	/// Constructor
	QPointFContainer(const QPointFContainer& c);
	/// Destructor
	~QPointFContainer();
	//@}

	/// @name Operators
	//@{
	/// Copy operator
	QPointFContainer& operator=(const QPointFContainer& c);
	QPointFContainer& operator=(const QPointF& v);
	QPointFContainer& operator+=(const QPointF& v);
	QPointFContainer& operator-=(const QPointF& v);
	bool operator==(const QPointF& v) const;
	bool operator!=(const QPointF& v) const;
	operator QPointF() const;
	//@}

	/// @name XML file I/O functions
	//@{
	/// Load data from XML attributes
	void load(const QDomNode&) override;
	/// Save data into XML node as attributes
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Getter and Setter
	//@{
	/// Value getter
	QPointF value() const;
	/// Value setter
	void setValue(const QPointF& val);
	//@}

	void setPrefix(const QString& prefix) override;

private:
	void copyValue(const XmlAttributeContainer& c) override;

	DoubleContainer m_xValue {"x"};
	DoubleContainer m_yValue {"y"};
};

#endif // QPOINTFCONTAINER_H
