#include "parameters.h"

Parameters::Parameters()
{
	// Initialize with defaults
	roots.append(complex(1, 0));
	roots.append(complex(-0.5, 0.5 * sqrt(3)));
	roots.append(complex(-0.5, -0.5 * sqrt(3)));
	resultSize = QSize(128, 128);
	maxIterations = 20;
}
