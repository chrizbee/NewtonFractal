// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "rootedit.h"
#include "parameters.h"

RootEdit::RootEdit(complex value, QWidget *parent) :
	QLineEdit(parent)
{
	// Set geometry and connect signal
	setValue(value);
	setMaximumWidth(110);
	setAlignment(Qt::AlignRight);
	connect(this, &RootEdit::editingFinished, this, &RootEdit::on_editingFinished);
}

complex RootEdit::value() const
{
	// Return current value
	return root_;
}

void RootEdit::setValue(complex value)
{
	// Set root and text
	root_ = value;
	setText(complex2string(value));
}

void RootEdit::on_editingFinished()
{
	// Create complex number from string
	bool sign = false;
	double real = 0, imag = 0;
	QStringList parts = text().split('i');
	QString rstr = parts.first().simplified();

	// Get imag sign from first part
	int si = rstr.indexOf('-', 1);
	if (si > 0) {
		sign = true;
		rstr.replace(si, 1, ' ');
	} else rstr.replace('+', ' ');

	// Get real
	real = rstr.simplified().toDouble();

	// Get imag
	if (parts.length() >= 2) {
		imag = parts[1].simplified().toDouble();
		if (sign) imag = -imag;
	}

	// Set new root value
	setValue(complex(real, imag));
	emit rootChanged();
}
