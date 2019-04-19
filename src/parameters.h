#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QSize>
#include <complex>

typedef std::complex<double> complex;

struct Parameters {
	Parameters();
	QVector<complex> roots;
	QSize resultSize;
	quint8 maxIterations;
};

#endif // PARAMETERS_H
