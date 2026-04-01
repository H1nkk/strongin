#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include "pqueue.h"

#include "../include/solver.h"

using namespace std;

struct dotInfo {
	double arg;
	double funcVal;
	double R;
};

struct RInfo {
	double R;
	double arg;
	//pair<double, double>* info;
	RInfo(double nR = 0, double nArg = 0) : R(nR), arg(nArg) {}
	//RInfo(double nR, pair<double, double>* di) : R(nR), info(di) {}
	bool operator<(const RInfo& rvalue) {
		return R < rvalue.R;
	}
	bool operator<=(const RInfo& rvalue) {
		return R <= rvalue.R;
	}
	bool operator>(const RInfo& rvalue) {
		return R > rvalue.R;
	}
	bool operator>=(const RInfo& rvalue) {
		return R >= rvalue.R;
	}
	bool operator==(const RInfo& rvalue) {
		return (R == rvalue.R);
	}
};

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	// èíèöèàëèçàöèÿ
	map<double, double> funcValue; // ìàïà èç àðãóìåíòà â çíà÷åíèå ôóíêöèè 
	double firstM = fabs((func(b, SLOWINGITERS) - func(a, SLOWINGITERS)) / (b - a));
	funcValue[a] = func(a, SLOWINGITERS);
	funcValue[b] = func(b, SLOWINGITERS);

	double rightFuncVal = funcValue[b], leftFuncVal = funcValue[a];
	double M = fabs((rightFuncVal - leftFuncVal) / (b - a));
	double m;
	double prevm = 0;

	if (M > 0) {
		m = r * M;
	}
	else {
		m = 1;
	}

	pqueue<RInfo> Rqueue;

	double firstR = m * (b - a);
	firstR += (rightFuncVal - leftFuncVal) * (rightFuncVal - leftFuncVal) / (m * (b - a));
	firstR -= 2 * (rightFuncVal - leftFuncVal);
	Rqueue.insert({ firstR, a });

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {
		// Äîáàâëåíèå íîâîé òî÷êè
		RInfo maxInfo = Rqueue.get();
		double ldot = maxInfo.arg; // ëåâàÿ ãðàíèöà ïîäðàçáèâàåìîãî èíòåðâàëà
		double rdot = (*next(funcValue.find(ldot))).first; // ïðàâàÿ ãðàíèöà ïîäðàçáèâàåìîãî èíòåðâàëà
		double newDot = 0.5 * (rdot + ldot) - (funcValue[rdot] - funcValue[ldot]) * 0.5 / m;
		funcValue[newDot] = func(newDot, SLOWINGITERS);

		if ((rdot - newDot) < E || (newDot - ldot) < E)
			break;

		// Ïåðåñ÷åò M äëÿ íîâîãî èíòåðâàëà
		ldot = maxInfo.arg;
		double mdot = (*next(funcValue.find(ldot))).first;
		rdot = (*next(funcValue.find(mdot))).first;
		double Mcandidate1 = fabs((funcValue[mdot] - (funcValue[ldot])) / (mdot - ldot));
		double Mcandidate2 = fabs((funcValue[rdot] - (funcValue[mdot])) / (rdot - mdot));
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
				double ldot = (*prev).first, rdot = (*cur).first;
				double lval = funcValue[ldot], rval = funcValue[rdot];
				double newR = m * (rdot - ldot)
					+ (rval - lval) * (rval - lval) / (m * (rdot - ldot))
					- 2 * (rval - lval);
				Rqueue.insert({ newR, ldot });
				prev = next(prev);
				cur = next(cur);
			}

		}
		else {

			double lArg = Rqueue.get().arg; // àðãóìåíò, äëÿ êîòîðîãî áóäåì ïåðåñ÷èòûâàòü R
			double mArg = newDot; // ýòîò àðãóìåíò òîëüêî ÷òî ïîÿâèëñÿ, äëÿ íåãî íóæíî ïîñ÷èòàòü R
			double rArg = (*next(funcValue.find(newDot))).first; // ïðàâàÿ ãðàíèöà íîâîãî èíòåðâàëà

			double RToRecalculate1 = m * (rArg - lArg)
				+ (funcValue[rArg] - funcValue[lArg]) * (funcValue[rArg] - funcValue[lArg]) / (m * (rArg - lArg))
				- 2 * (funcValue[rArg] - funcValue[lArg]);

			Rqueue.pop();

			double newR1 = m * (mArg - lArg)
				+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * (mArg - lArg))
				- 2 * (funcValue[mArg] - funcValue[lArg]);
			double newR2 = m * (rArg - mArg)
				+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * (rArg - mArg))
				- 2 * (funcValue[rArg] - funcValue[mArg]);

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
	Solver solver(AGP, "omp-pqueue");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
