// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

var nf = {
	EPS: 	1e-3,					// Max error allowed
	RIR:	5,						// Root indicator radius
	ZMF:	0.05,					// Zoom factor
	MRC:	10,						// Maximum root count
	DRC:	5,						// Default root count
	DDP:	1.0,					// Default damping factor
	DMI:	160,					// Default max. iterations
	DSF:	1.0 / 700.0,			// Default size factor for limits
	predefColors: [
		[1.0, 0, 0], [0, 1.0, 0], [0, 0, 1.0],
		[0, 1.0, 1.0], [1.0, 0, 1.0], [1.0, 1.0, 0],
		[1.0, 0.5, 0], [1.0, 0, 0.5], [0, 1.0, 0.5], [0.5, 0.5, 0.5]
	]
}

export default nf;