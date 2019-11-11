#include "particle_systems.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;
using namespace FW;

namespace {

	inline Vec3f fGravity(float mass) {
		return Vec3f(0, -9.8f * mass, 0);
	}

	// force acting on particle at pos1 due to spring attached to pos2 at the other end
	inline Vec3f fSpring(const Vec3f& pos1, const Vec3f& pos2, float k, float rest_length) {
		// YOUR CODE HERE (R2)
		float distance = pow((pos1 - pos2).x, 2)+ pow((pos1 - pos2).y, 2)+ pow((pos1 - pos2).z, 2);
		distance = sqrtf(distance);
		distance = FW::abs(distance);
		Vec3f force = -k * (distance - rest_length) * ((pos2 - pos1)/distance);
		return force;
	}

	inline Vec3f fDrag(const Vec3f& v, float k) {
		// YOUR CODE HERE (R2)
		Vec3f drag = -k*v;
		return drag;
	}
	//r5
	inline int getindex(int i,int j,int x_,int y_) {
		// YOUR CODE HERE (R2)
		int index;
		if (i >= 0 && j >= 0 && i<x_ && j<y_)
			index = (j * x_ + i) * 2;
		else
			index = -1;
		return index;
	}
} // namespace

void SimpleSystem::reset() {
	state_ = State(1, Vec3f(0, radius_, 0));
}

State SimpleSystem::evalF(const State& state) const {
	State f(1, Vec3f(-state[0].y, state[0].x, 0));
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H
// using the implicit Euler method, the simple system should converge towards origin -- as opposed to the explicit Euler, which diverges outwards from the origin.
void SimpleSystem::evalJ(const State&, SparseMatrix& result, bool initial) const {
	if (initial) {
		result.coeffRef(1, 0) = 1.0f;
		result.coeffRef(0, 1) = -1.0f;
	}
}
#endif

Points SimpleSystem::getPoints() {
	return Points(1, state_[0]);
}

Lines SimpleSystem::getLines() {
	static const auto n_lines = 50u;
	auto l = Lines(n_lines * 2);
	const auto angle_incr = 2 * FW_PI / n_lines;
	for (auto i = 0u; i < n_lines; ++i) {
		l[2 * i] = l[2 * i + 1] =
			Vec3f(radius_ * FW::sin(angle_incr * i), radius_ * FW::cos(angle_incr * i), 0);
	}
	rotate(l.begin(), l.begin() + 1, l.end());
	return l;
}

void SpringSystem::reset() {
	const auto start_pos = Vec3f(0.1f, -0.5f, 0.0f);
	const auto spring_k = 30.0f;
	state_ = State(4);
	// YOUR CODE HERE (R2)
	// Set the initial state for a particle system with one particle fixed
	// at origin and another particle hanging off the first one with a spring.
	// Place the second particle initially at start_pos.
	state_[0] = Vec3f(0.0f, 0.0f, 0.0f);
	state_[1] = Vec3f(0.0f, 0.0f, 0.0f);
	state_[2] = start_pos;
	state_[3] = Vec3f(0.0f, 0.0f, 0.0f);
}

State SpringSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	State f(4);
	// YOUR CODE HERE (R2)
	// Return a derivative for the system as if it was in state "state".
	// You can use the fGravity, fDramassg and fSpring helper functions for the forces.
	//fSpring(const Vec3f& pos1, const Vec3f& pos2, float k, float rest_length) 

	//inline Vec3f fGravity(float ) {
	//Vec3f fSpring(const Vec3f & pos1, const Vec3f & pos2, float k, float rest_length) {
	//inline Vec3f fDrag(const Vec3f & v, float k) {

	Vec3f gravity = fGravity(mass);
	//float restlength = (state[0]-state[2]).length;
	//set the restlength
	float restlength = 0.1f;
	Vec3f spring = fSpring(state[0], state[2], 10, restlength);
	Vec3f force =	fDrag(state[3], drag_k);
	//the first paticle should be 0 force
	//dx = v
	f[0] = state[1];
	f[2] = state[3];
	// dv = f/m
	f[1] = Vec3f(0.0f, 0.0f, 0.0f);
	f[3] = (gravity+spring+force)/mass;
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

// This is a very useful read for the Jacobians of the spring forces. It deals with spring damping as well, we don't do that -- our drag is simply a linear damping of velocity (that results in some constants in the Jacobian).
// http://blog.mmacklin.com/2012/05/04/implicitsprings/

void SpringSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	// EXTRA: Evaluate the Jacobian into the 'result' matrix here. Only the free end of the spring should have any nonzero values related to it.
}
#endif

Points SpringSystem::getPoints() {
	auto p = Points(2);
	p[0] = state_[0]; p[1] = state_[2];
	return p;
}

Lines SpringSystem::getLines() {
	auto l = Lines(2);
	l[0] = state_[0]; l[1] = state_[2];
	return l;
}

void PendulumSystem::reset() {
	const auto spring_k = 1000.0f;
	const auto start_point = Vec3f(0);
	const auto end_point = Vec3f(0.05, -1.5, 0);
	state_ = State(2 * n_);
	// YOUR CODE HERE (R4)
	// Set the initial state for a pendulum system with n_ particles
	// connected with springs into a chain from start_point to end_point.
	auto step = (end_point - start_point) / (n_-1);
	for(int i=0;i < n_*2; i = i + 2)
	{
		state_[i] = start_point + step*(i / 2);
		state_[i + 1] = Vec3f(0.0f, 0.0f, 0.0f);
	}
}

State PendulumSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 0.5f;
	auto f = State(2 * n_);
	// YOUR CODE HERE (R4)
	// As in R2, return a derivative of the system state "state".

	Vec3f gravity = fGravity(mass);
	float restlength = 0.1f;
	float springStrength = 30.0f;
	//each perticle will be affected by all the other particle

	for (int i = 0; i < n_ * 2; i = i + 2) {
		f[i] = state[i + 1];
		if (i == 0)
		{
			f[i + 1] = Vec3f(0.0f, 0.0f, 0.0f);
		}
		else if (i == n_*2-2)
		{
			Vec3f spring = fSpring(state[i - 2], state[i], springStrength *n_, restlength);
			Vec3f force = fDrag(state[i + 1], drag_k);
			f[i + 1] = (gravity + force + spring) / mass;
		}
		else {
			//for any 
			Vec3f force = fDrag(state[i + 1], drag_k);	
			Vec3f spring1 = fSpring(state[i - 2], state[i], springStrength * n_, restlength);
			Vec3f spring2 = fSpring(state[i + 2], state[i], springStrength * n_, restlength);
			f[i + 1] = (spring1+spring2 + force + gravity) / mass;
		}
		
	}
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void PendulumSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {

	const auto drag_k = 0.5f;
	const auto mass = 0.5f;

	// EXTRA: Evaluate the Jacobian here. Each spring has an effect on four blocks of the matrix -- both of the positions of the endpoints will have an effect on both of the velocities of the endpoints.
}
#endif


Points PendulumSystem::getPoints() {
	auto p = Points(n_);
	for (auto i = 0u; i < n_; ++i) {
		p[i] = state_[i * 2];
	}
	return p;
}

Lines PendulumSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(state_[2 * s.i1]);
		l.push_back(state_[2 * s.i2]);
	}
	return l;
}


void ClothSystem::reset() {
	const auto spring_k = 300.0f;
	const auto width = 1.5f, height = 1.5f; // width and height of the whole grid
	float perwidth = width/(x_-1), perheight = height/(y_-1);
	float restlength = 1.5f / x_;
	float Crosslength = restlength * 1.414f;
	state_ = State(2 * x_*y_);
	// YOUR CODE HERE (R5)
	// Construct a particle system with a x_ * y_ grid of particles,
	// connected with a variety of springs as described in the handout:
	// structural springs, shear springs and flex springs.
	for (int i = 0; i < x_; ++i) {
		for (int j = 0; j < y_ ; ++j) {
			int index = getindex(i, j, x_, y_);
			state_[index] = Vec3f(-0.75f+i * perwidth, 0, 0.75f - j * perheight);
			state_[index + 1] = Vec3f(0.0f, 0.0f, 0.0f);

			//structural
			if (j < y_ - 1) {
				Spring s1 = Spring(getindex(i, j, x_, y_), getindex(i, j + 1, x_, y_), spring_k, restlength);
				springs_.push_back(s1);
			}

			if (i < x_ - 1) {
				Spring s2 = Spring(getindex(i, j, x_, y_), getindex(i + 1, j, x_, y_), spring_k, restlength);
				springs_.push_back(s2);
			}

			// shear
			if (i > 0 && j < y_ - 1) { //has up spring
				Spring s1 = Spring(getindex(i, j, x_, y_), getindex(i - 1, j + 1, x_, y_), spring_k, Crossrestlength);
				springs_.push_back(s1);
			}

			if (i < x_ - 1 && j < y_ - 1) { //has down spring
				Spring s2 = Spring(getindex(i, j, x_, y_), getindex(i + 1, j + 1, x_, y_), spring_k, Crossrestlength);
				springs_.push_back(s2);
			}

			// Flexion
			if (j < y_ - 2) { //has right spring
				Spring s1 = Spring(getindex(i, j, x_, y_), getindex(i, j + 2, x_, y_), spring_k, 2 * restlength);
				springs_.push_back(s1);
			}

			if (i < x_ - 2) { // has down spring
				Spring s2 = Spring(getindex(i, j, x_, y_), getindex(i + 2, j, x_, y_), spring_k, 2 * restlength);
				springs_.push_back(s2);
			}
		}
	}
	springs_.clear();
}

State ClothSystem::evalF(const State& state) const {
	const auto drag_k = 0.08f;
	const auto n = x_ * y_;
	static const auto mass = 0.025f;
	auto f = State(2 * n);
	// YOUR CODE HERE (R5)
	// This will be much like in R2 and R4.
	//add structural spring
	float springStrength = 10000.0f;
	float restlength = 0.075f;
	float Crossrestlength = 0.075f*1.414f;
	int index, indexup1, indexup2, indexdown1, indexdown2, indexright1, indexright2, indexleft1, indexleft2;
	int	upleft, upright, downleft, downright;
	
	for (int i = 0; i < x_ ; i = i + 1) {
		for (int j = 0; j < y_; j = j + 1) {
			index = getindex(i, j, x_, y_);
			//for structural
			indexup1 = getindex(i, j - 1, x_, y_);
			indexdown1 = getindex(i, j + 1, x_, y_);
			indexright1 = getindex(i + 1, j, x_, y_);
			indexleft1 = getindex(i - 1, j, x_, y_);
			//for shear
			upright = getindex(i + 1, j - 1, x_, y_);
			upleft = getindex(i - 1, j - 1, x_, y_);
			downleft = getindex(i - 1, j + 1, x_, y_);
			downright = getindex(i + 1, j + 1, x_, y_);
			//for Flexion
			indexup2 = getindex(i, j - 2, x_, y_);
			indexdown2 = getindex(i, j + 2, x_, y_);
			indexright2 = getindex(i + 2, j, x_, y_);
			indexleft2 = getindex(i - 2, j, x_, y_);

			Vec3f gravity = fGravity(mass);
			Vec3f force, structual,shear,flexion;
			force = fDrag(state[index + 1], drag_k);

			f[index] = state[index + 1];
			//these two are still
			if (i == 0 && j == 0) {
				f[index+1] =Vec3f(0,0,0) ;
			}
			else if (i == x_-1 && j == 0) {
				f[index + 1] = Vec3f(0, 0, 0);
			}
			
			else
			{
				structual = Vec3f(0.0f, 0.0f, 0.0f);
				if (indexup1 != -1)
					structual += fSpring(state[indexup1], state[index], springStrength, restlength);
				if (indexdown1 != -1)
					structual += fSpring(state[indexdown1], state[index], springStrength, restlength);
				if (indexright1 != -1)
					structual += fSpring(state[indexright1], state[index], springStrength, restlength);
				if (indexleft1 != -1)
					structual += fSpring(state[indexleft1], state[index], springStrength, restlength);
				//shear
				shear = Vec3f(0.0f, 0.0f, 0.0f);
				if (upleft != -1)
					flexion += fSpring(state[upleft], state[index], springStrength, Crossrestlength);
				if (upright != -1)
					flexion += fSpring(state[upright], state[index], springStrength, Crossrestlength);
				if (downleft != -1)
					flexion += fSpring(state[downleft], state[index], springStrength, Crossrestlength);
				if (downright != -1)
					flexion += fSpring(state[downright], state[index], springStrength, Crossrestlength);
				//flexion
				flexion = Vec3f(0.0f, 0.0f, 0.0f);
				if (indexup2 != -1)
					flexion += fSpring(state[indexup2], state[index], springStrength, restlength);
				if (indexdown2 != -1)
					flexion += fSpring(state[indexdown2], state[index], springStrength, restlength);
				if (indexright2 != -1)
					flexion += fSpring(state[indexright2], state[index], springStrength, restlength);
				if (indexleft2 != -1)
					flexion += fSpring(state[indexleft2], state[index], springStrength, restlength);
				f[index + 1] = (gravity + force + structual) / mass;
			}
			
		}
	}
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void ClothSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.08f;
	static const auto mass = 0.025f;

	// EXTRA: Evaluate the Jacobian here. The code is more or less the same as for the pendulum.
}

#endif

Points ClothSystem::getPoints() {
	auto n = x_ * y_;
	auto p = Points(n);
	for (auto i = 0u; i < n; ++i) {
		p[i] = state_[2 * i];
	}
	return p;
}

Lines ClothSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(state_[2 * s.i1]);
		l.push_back(state_[2 * s.i2]);
	}
	return l;
}
State FluidSystem::evalF(const State&) const {
	return State();
}

