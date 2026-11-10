#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include <span>
#include <functional>
#include <numbers>

#include "../strongin-pqueue/pqueue.h"
#include "../include/solver.h"
#include "../include/evolvent.h"

using namespace std;

struct RInfo {
	double R;
	double arg;
	//pair<double, double>* info;
	RInfo(double nR = 0, double nArg = 0) : R(nR), arg(nArg) {}
	//RInfo(double nR, pair<double, double>* di) : R(nR), info(di) {}
	auto operator<=>(const RInfo& o) const { return R <=> o.R; }
};

double f(span<const double> y) {
	return (y[0] - 6) * (y[0] - 6) + (y[1] - 4) * (y[1] - 4);
}

double rastrigin(span<const double> y) {
	const int N = y.size();
	const double A = 10.0;
	double res = A * N;
	for (int i = 0; i < N; i++) {
		res += (y[i] * y[i] - A * cos(2 * numbers::pi * y[i]));
	}
	return res;
}

info AGP(TEvolvent& ev, const function<double(span<const double>)>& func, double r, double E, int ITERMAX) {
	// инициализация
	const double a = 0, b = 1;
	const int N = ev.GetDimension();
	auto delta = [N](double left, double right) { return pow(right - left, 1.0 / N); };

	map<double, double> funcValue; // мапа из аргумента в значение функции 

	vector<double> left_dot(N);
	vector<double> right_dot(N);
	ev.GetImage(a, left_dot.data());
	ev.GetImage(b, right_dot.data());

	funcValue[a] = func(left_dot);
	funcValue[b] = func(right_dot);

	double rightFuncVal = funcValue[b], leftFuncVal = funcValue[a];
	double M = fabs(rightFuncVal - leftFuncVal) / delta(a, b);
	double m;
	double prevm = 0;

	if (M > 0) {
		m = r * M;
	}
	else {
		m = 1;
	}

	pqueue<RInfo> Rqueue;

	double firstR = m * delta(a, b);
	firstR += (rightFuncVal - leftFuncVal) * (rightFuncVal - leftFuncVal) / (m * delta(a, b));
	firstR -= 2 * (rightFuncVal + leftFuncVal);
	Rqueue.insert({ firstR, a });

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {
		// Добавление новой точки
		RInfo maxInfo = Rqueue.get();
		double ldot = maxInfo.arg; // левая граница подразбиваемого интервала
		double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
		double dz = funcValue[rdot] - funcValue[ldot];
		double mu = m / r;                       // the estimate itself, without r
		double newDot = 0.5 * (rdot + ldot) - ((dz > 0) - (dz < 0)) * pow(fabs(dz) / mu, N) / (2 * r);

		vector<double> newNDimDot(N);
		ev.GetImage(newDot, newNDimDot.data());
		funcValue[newDot] = func(newNDimDot);

		if (pow(rdot - newDot, 1.0 / N) < E || pow(newDot - ldot, 1.0 / N) < E)
			break;

		// Пересчет M для нового интервала
		ldot = maxInfo.arg;
		double mdot = (*next(funcValue.find(ldot))).first;
		rdot = (*next(funcValue.find(mdot))).first;
		double Mcandidate1 = fabs(funcValue[mdot] - funcValue[ldot]) / delta(ldot, mdot);
		double Mcandidate2 = fabs(funcValue[rdot] - funcValue[mdot]) / delta(mdot, rdot);
		M = max({ M,Mcandidate1,Mcandidate2 });

		if (M > 0) {
			m = r * M;
		}
		else {
			m = 1;
		}

		if (prevm != m) {
			auto prev = funcValue.begin();
			auto cur = next(funcValue.begin());

			Rqueue.clear();
			for (int i = 0; i < funcValue.size() - 1; i++) {
				double leftdot = (*prev).first, rightdot = (*cur).first;
				double lval = funcValue[leftdot], rval = funcValue[rightdot];
				double newR = m * delta(leftdot, rightdot)
					+ (rval - lval) * (rval - lval) / (m * delta(leftdot, rightdot))
					- 2 * (rval + lval);
				Rqueue.insert({ newR, leftdot });
				prev = next(prev);
				cur = next(cur);
			}

		}
		else {

			double lArg = Rqueue.get().arg; // аргумент, для которого будем пересчитывать R
			double mArg = newDot; // этот аргумент только что появился, для него нужно посчитать R
			double rArg = (*next(funcValue.find(newDot))).first; // правая граница нового интервала

			Rqueue.pop();

			double newR1 = m * delta(lArg, mArg)
				+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * delta(lArg, mArg))
				- 2 * (funcValue[mArg] + funcValue[lArg]);
			double newR2 = m * delta(mArg, rArg)
				+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * delta(mArg, rArg))
				- 2 * (funcValue[rArg] + funcValue[mArg]);

			Rqueue.insert({ newR1, lArg });
			Rqueue.insert({ newR2, mArg });
		}

		prevm = m;
	}

	double extrArg = (*funcValue.begin()).first;
	double funcMin = funcValue[extrArg];
	double prevArg = -INFINITY;
	double closestArg1 = -INFINITY, closestArg2 = INFINITY;
	for (auto p : funcValue) {
		if (prevArg != -INFINITY) {
			if ((p.first - prevArg) < (closestArg2 - closestArg1)) {
				closestArg1 = prevArg;
				closestArg2 = p.first;
			}
		}
		if (p.second < funcMin) {
			funcMin = p.second;
			extrArg = p.first;
		}
		prevArg = p.first;
	}
	info res = { extrArg, funcMin, {closestArg1, closestArg2} ,funcValue.size() - 2 };
	return res;
}



int main() {
	try {
		const int N = 2, m = 14;
		double E = 0.0001;
		double x_min = -8, x_max = 8;
		double y_min = -30, y_max = 30;
		double A[] = { x_min, y_min }, B[] = { x_max , y_max };

		TEvolvent ev(N, m);
		ev.SetBounds(A, B);


		int number_of_calls = 0;

		auto res = AGP(
			ev,
			[&number_of_calls](span<const double> x) { ++number_of_calls; return rastrigin(x); },
			2.0,
			E,
			3000
		);
		std::cout << std::fixed;

		vector<double> resDot(N);
		ev.GetImage(res.extremumArg, resDot.data());
		cout << resDot[0] << " " << resDot[1] << "\n";
		cout << res.iterCount << "\n";
		cout << number_of_calls;
	}
	catch (const char* msg) {
		cerr << "Error: " << msg << "\n";
		return 1;
	}
	return 0;
}