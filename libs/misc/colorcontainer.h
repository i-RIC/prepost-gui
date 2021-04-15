#ifndef COLORCONTAINER_H
#define COLORCONTAINER_H

#include "xmlattributecontainer.h"

class QColor;

class MISCDLL_EXPORT ColorContainer : public XmlAttributeContainer
{

public:
	/// @name Constructors and Destructor
	//@{
	ColorContainer(const QString& name);
	ColorContainer(const QString& name, const QColor& defaultVal);
	ColorContainer(const ColorContainer& c);
	virtual ~ColorContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	//@{
	ColorContainer& operator=(const ColorContainer& c);
	ColorContainer& operator=(const QColor& v);
	operator QColor() const;
	operator double*() const;
	//@}

	/// @name Getter and Setter
	//@{
	const QColor& value() const;
	void setValue(const QColor& val);
	//@}

	/// @name Prefix
	//@{
	const QString& prefix() const override;
	void addPrefix(const QString& prefix) override;
	/// Attributes name (Prefix added if set)
	QString attName(const QString& name) const override;
	//@}

private:
	void copyValue(const XmlAttributeContainer& c) override;

	void setupVtkValue() const;
	void updateVtkValue() const;

	mutable double* m_double {nullptr};

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/colorcontainer_impl.h"
#endif // _DEBUG

#endif // COLORCONTAINER_H
