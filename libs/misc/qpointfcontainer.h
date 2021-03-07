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
	QPointFContainer();
	QPointFContainer(const QString& name);
	QPointFContainer(const QPointF& val);
	QPointFContainer(const QPointFContainer& c);
	~QPointFContainer();
	//@}

	/// @name Operators
	//@{
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
	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Getter and Setter
	//@{
	QPointF value() const;
	void setValue(const QPointF& val);
	//@}

	void addPrefix(const QString& prefix) override;

private:
	void copyValue(const XmlAttributeContainer& c) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/qpointfcontainer_impl.h"
#endif // _DEBUG

#endif // QPOINTFCONTAINER_H
