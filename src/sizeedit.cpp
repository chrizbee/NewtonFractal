// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "sizeedit.h"

SizeEdit::SizeEdit(QWidget *parent) :
	QLineEdit(parent)
{
	// Connect signal
	connect(this, &SizeEdit::editingFinished, this, &SizeEdit::on_editingFinished);
}

QSize SizeEdit::value() const
{
	// Get size
	return size_;
}

void SizeEdit::setValue(QSize value)
{
	// Static string format
	static QString complexFormat("%1 x %2");

	// Create string from size
	QString sstr = complexFormat.arg(value.width()).arg(value.height());

	// Set size and text
	size_ = value;
	setText(sstr);
}

void SizeEdit::on_editingFinished()
{
	// Create size from string
	QSize newSize = size_;
	QStringList parts = text().split('x');

	// Get parts
	if (parts.length() == 2) {
		newSize.setWidth(parts[0].simplified().toUInt());
		newSize.setHeight(parts[1].simplified().toUInt());
	}

	// Error handling
	if (newSize.width() <= 0)
		newSize.setWidth(size_.width());
	if (newSize.height() <= 0)
		newSize.setHeight(size_.height());

	// Set new root
	setValue(newSize);
	emit sizeChanged(newSize);
}
