
#include "utility.hpp"
#include "particle_systems.hpp"
#include "integrators.hpp"

void eulerStep(ParticleSystem& ps, float step) {
	// YOUR CODE HERE (R1)
	// Implement an Euler integrator.
	//step = 0.001f;
	const auto& x0 = ps.state();
	auto n = x0.size();
	auto f0 = ps.evalF(x0); //f0 is now the derivative of f(X,t0)
	auto x1 = State(n);
	for (auto i = 0u; i < n; ++i) {
		x1[i] = x0[i] + (1.0f * step) * f0[i];
	}
	ps.set_state(x1);
};

void trapezoidStep(ParticleSystem& ps, float step) {
	// YOUR CODE HERE (R3)
	// Implement a trapezoid integrator.
	const auto& x0 = ps.state();
	auto n = x0.size();
	auto f0 = ps.evalF(x0);
	auto xm = State(n), x1 = State(n);
	for (auto i = 0u; i < n; ++i) {
		xm[i] = x0[i] +  step * f0[i];
	}
	auto fm = ps.evalF(xm);
	for (auto i = 0u; i < n; ++i) {
		x1[i] = x0[i] + step/2 * (fm[i]+f0[i]);
	}
	ps.set_state(x1);
}

void midpointStep(ParticleSystem& ps, float step) {
	const auto& x0 = ps.state();
	auto n = x0.size();
	auto f0 = ps.evalF(x0);
	auto xm = State(n), x1 = State(n);
	for (auto i = 0u; i < n; ++i) {
		xm[i] = x0[i] + (0.5f * step) * f0[i];
	}
	auto fm = ps.evalF(xm);
	for (auto i = 0u; i < n; ++i) {
		x1[i] = x0[i] + step * fm[i];
	}
	ps.set_state(x1);
}

State addstate(const State& s1, float a, const State& s2, float b) {
	auto n = s1.size();
	State result;
	if (n != s2.size()) {

		return result;
	}
	result.resize(n);
	for (auto i = 0u; i < n; ++i) {
		result[i] = a * s1[i] + b * s2[i];
	}
	return result;
}

void rk4Step(ParticleSystem& ps, float step) {
	// EXTRA: Implement the RK4 Runge-Kutta integrator.
	auto& x0 = ps.state();
	int n = x0.size();
	State k1(n), k2(n), k3(n), k4(n), x1(n);

	k1 = ps.evalF(x0);
	k2 = ps.evalF(addstate(x0, 1, k1, 0.5f * step));
	k3 = ps.evalF(addstate(x0, 1, k2, 0.5f * step));
	k3 = ps.evalF(addstate(x0, 1, k3, step));

	for (int i = 0; i < n; ++i) {
		x1[i] = x0[i] + (step / 6.0f) * (k1[i] + 2.0f * k2[i] + 2.0f * k3[i] + k4[i]);
	}

	ps.set_state(x1);
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void implicit_euler_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit Euler integrator. (Note that the related formula on page 134 on the lecture slides is missing a 'h'; the formula should be (I-h*Jf(Yi))DY=-F(Yi))
}

void implicit_midpoint_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit midpoint integrator.
}
#endif
