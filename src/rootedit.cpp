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
	// Set new root value
	setValue(string2complex(text()));
	emit rootChanged();
}
