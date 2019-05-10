// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "limits.h"
#include "defaults.h"

Limits::Limits(bool original) :
	left_(-1.0),
	right_(1.0),
	top_(1.0),
	bottom_(-1.0),
	original_(nullptr)
{
	// Create original
	if (!original) {
		original_ = new Limits(true);
	}
}

bool Limits::operator==(const Limits &other) const
{
	// Check if limits are the same
	return (
		left_ == other.left() &&
		right_ == other.right() &&
		top_ == other.top() &&
		bottom_ == other.bottom()
	);
}

void Limits::move(QPoint distance, const QSize &ref)
{
	// Move limits by distance
	double dx = distance.x() * (right_ - left_) / (ref.width() - 1);
	double dy = distance.y() * (bottom_ - top_) / (ref.height() - 1);
	left_ += dx;
	right_ += dx;
	top_ += dy;
	bottom_ += dy;

	// Move original limits
	if (original_ != nullptr) {
		original_->move(distance, ref);
	}
}

void Limits::zoom(bool in, double xw, double yw)
{
	// Zoom limits in / out
	double zoom = in ? -ZMF : ZMF;
	double wZoom = (right_ - left_) * zoom;
	double hZoom = (top_ - bottom_) * zoom;
	left_ -= xw * wZoom;
	right_ += (1.0 - xw) * wZoom;
	top_ += yw * hZoom;
	bottom_ -= (1.0 - yw) * hZoom;
}

void Limits::reset(QSize size)
{
	// Reset limits
	left_ = -DSF * size.width();
	right_ = DSF * size.width();
	top_ = DSF * size.height();
	bottom_ = -DSF * size.height();

	// Reset original limits
	if (original_ != nullptr) {
		original_->reset(size);
	}
}

void Limits::resize(QSize delta)
{
	// Resize limits
	right_ += DSF * delta.width();
	left_ -= DSF * delta.width();
	top_ += DSF * delta.height();
	bottom_ -= DSF * delta.height();

	// Resize original limits
	if (original_ != nullptr) {
		original_->resize(delta);
	}
}

double Limits::width() const
{
	// Return width
	return right_ - left_;
}

double Limits::height() const
{
	// Return height
	return top_ - bottom_;
}

double Limits::left() const
{
	// Return left limit
	return left_;
}

double Limits::right() const
{
	// Return right limit
	return right_;
}

double Limits::top() const
{
	// Return top limit
	return top_;
}

double Limits::bottom() const
{
	// Return bottom limit
	return bottom_;
}

double Limits::zoomFactor() const
{
	// Return zoomFactor
	return original_->width() / width();
}

void Limits::setZoomFactor(double zoomFactor)
{
	// Set zoomFactor
	double w2 = 0.5 * original_->width() / zoomFactor;
	double h2 = 0.5 * original_->height() / zoomFactor;
	double xMid = 0.5 * right_ + 0.5 * left_;
	double yMid = 0.5 * top_ + 0.5 * bottom_;
	right_ = xMid + w2;
	left_ = xMid - w2;
	top_ = yMid + h2;
	bottom_ = yMid - h2;
}