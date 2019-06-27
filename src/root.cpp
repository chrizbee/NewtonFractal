// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "root.h"
#include <QRandomGenerator>

Root::Root(complex value) :
	value_(value),
	color_(QColor::fromRgb(QRandomGenerator::global()->generate()))
{
}

complex Root::value() const
{
	// Get complex number
	return value_;
}

QColor Root::color() const
{
	// Get root color
	return color_;
}

void Root::setColor(QColor color)
{
	// Set root color
	color_ = color;
}

void Root::setValue(complex value)
{
	// Set complex number
	value_ = value;
}

Root &Root::operator=(const Root &other)
{
	// Assign operator overloading
	value_ = other.value();
	color_ = other.color();
	return *this;
}

Root &Root::operator=(complex value)
{
	// Assign operator overloading
	value_ = value;
	return *this;
}

Root &Root::operator+=(complex value)
{
	// Addition assign operator overloading
	value_ += value;
	return *this;
}

bool Root::operator==(const Root &other) const
{
	// Compare roots
	return (value_ == other.value() && color_ == other.color());
}

bool Root::operator==(complex value) const
{
	// Compare root to complex value
	return (value_ == value);
}

bool Root::operator!=(const Root &other) const
{
	// Compare roots
	return (value_ != other.value() || color_ != other.color());
}

bool Root::operator!=(complex value) const
{
	// Compare root to complex value
	return (value_ != value);
}
