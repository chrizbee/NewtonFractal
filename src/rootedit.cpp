// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Föehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "rootedit.h"

RootEdit::RootEdit(QWidget *parent) :
	QLineEdit(parent)
{
	// Connect signal
	connect(this, &RootEdit::editingFinished, this, &RootEdit::on_editingFinished);
}

complex RootEdit::root() const
{
	// Get root
	return root_;
}

void RootEdit::setRoot(complex root)
{
	// Static string format
	static QString complexFormat("%1 %2 i%3");

	// Create string from complex number
	QString real = QString::number(root.real(), 'f', 2);
	QString imag = QString::number(abs(root.imag()), 'f', 2);
	QString sign = root.imag() >= 0 ? '+' : '-';
	QString zstr = complexFormat.arg(real, sign, imag);

	// Set root and text
	root_ = root;
	setText(zstr);
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

	// Set new root
	setRoot(complex(real, imag));
	emit rootChanged();
}
