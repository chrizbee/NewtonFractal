// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef ROOTEDIT_H
#define ROOTEDIT_H

#include "defaults.h"
#include <QLineEdit>

class RootEdit : public QLineEdit
{
	Q_OBJECT

public:
	RootEdit(QWidget *parent = nullptr);
	complex value() const;
	void setValue(complex value);

signals:
	void valueChanged();

private:
	complex root_;
};

#endif // ROOTEDIT_H
