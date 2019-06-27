// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef ROOT_H
#define ROOT_H

#include "defaults.h"
#include <QColor>

class Root
{
public:
	Root(complex value = complex(0, 0));
	complex value() const;
	QColor color() const;
	void setColor(QColor color);
	void setValue(complex value);
	Root& operator=(const Root& other);
	Root& operator=(complex value);
	Root& operator+=(complex value);
	bool operator==(const Root& other) const;
	bool operator==(complex value) const;
	bool operator!=(const Root& other) const;
	bool operator!=(complex value) const;

private:
	complex value_;
	QColor color_;
};

#endif // ROOT_H
