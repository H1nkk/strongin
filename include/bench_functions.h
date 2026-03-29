#include <numbers>
#include <cmath>

extern int SLOWINGITERS;

double funcSlower(double x) {
	double k = 1;
	for (int i = 0; i < SLOWINGITERS; i++) {
		k *= (cos(x) * cos(x) + sin(x) * sin(x));
	}
	return k;
}

double benchFunc1(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0);

	res *= k;
	return res;
}

double benchFunc2(double x) {
	double k = funcSlower(x);
	double res = 0;
	for (double k = 1.0; k <= 5.0; k += 1.0) {
		res += k * sin((k + 1) * x + k);
	}
	res *= -1;

	res *= k;
	return res;
}

double benchFunc3(double x) {
	double k = funcSlower(x);
	double res = (3.0 * x - 1.4) * sin(18.0 * x);

	res *= k;
	return res;
}

double benchFunc4(double x) {
	double k = funcSlower(x);
	double res = -(x + sin(x));
	res *= exp(-(x * x));

	res *= k;
	return res;
}

double benchFunc5(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0) + log(x) - 0.84 * x + 3.0;

	res *= k;
	return res;
}

double benchFunc6(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0) + log(x) - 0.84 * x + 3.0;

	res *= k;
	return res;
}

double benchFunc7(double x) {
	double k = funcSlower(x);
	double res = -sin(2 * std::numbers::pi_v<double> *x) * exp(-x);

	res *= k;
	return res;
}

double benchFunc8(double x) {
	double k = funcSlower(x);
	double res = (x * x - 5.0 * x + 6.0);
	res /= (x * x + 1.0);

	res *= k;
	return res;
}

double benchFunc9(double x) {
	double k = funcSlower(x);
	double res = -x + sin(3.0 * x) - 1.0;

	res *= k;
	return res;
}

double benchFunc10(double x) {
	double k = funcSlower(x);
	double res = 2.0 * (x - 3.0) * (x - 3.0) + exp(x * x * 0.5);

	res *= k;
	return res;
}