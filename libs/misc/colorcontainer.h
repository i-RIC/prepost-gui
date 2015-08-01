#ifndef COLORCONTAINER_H
#define COLORCONTAINER_H

#include "misc_global.h"
#include "xmlattributecontainer.h"

#include <QColor>

class MISCDLL_EXPORT ColorContainer : public XmlAttributeContainer
{

public:
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	ColorContainer(const QString& name);
	/// Constructor (with default value)
	ColorContainer(const QString& name, const QColor& defaultVal);
	/// Constructor (copy)
	ColorContainer(const ColorContainer& c);
	/// Destructor
	virtual ~ColorContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	/// Save data into XML node as attributes
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	//@{
	/// Copy operator
	ColorContainer& operator=(const ColorContainer& c);
	ColorContainer& operator=(const QColor& v);
	/// Convert operator against QColor
	operator QColor() const;
	/// Convert operator against double pointer
	operator double*() const;
	//@}

	/// @name Getter and Setter
	//@{
	/// Value getter
	const QColor& value() const;
	/// Value setter
	void setValue(const QColor& val);
	//@}

	/// @name Prefix
	//@{
	const QString& prefix() const override;
	/// Set prefix for XML attributes
	void setPrefix(const QString& prefix) override;
	/// Attributes name (Prefix added if set)
	QString attName(const QString& name) const override;
	//@}

private:
	void copyValue(const XmlAttributeContainer& c) override;

	void setupVtkValue() const;
	void updateVtkValue() const;

	class Impl;
	Impl* m_impl;
	mutable double* m_double {nullptr};
};

#endif // COLORCONTAINER_H
