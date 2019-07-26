// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "root.h"

Root::Root(complex value, QColor color) :
	value_(value),
	color_(color)
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

QVector2D Root::valueVec2() const
{
	// Get complex number as vec2
	return QVector2D(value_.real(), value_.imag());
}

QVector3D Root::colorVec3() const
{
	// Get root color as vec3
	return QVector3D(color_.red() / 255.0, color_.green() / 255.0, color_.blue() / 255.0);
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
