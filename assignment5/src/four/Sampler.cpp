#include "Sampler.h"

#include <cassert>

Sampler::Sampler(int nSamples) :
	m_nSamples(nSamples)
{}

Sampler* Sampler::constructSampler(Args::SamplePatternType t, int numsamples)
{
	if( t == Args::Pattern_Uniform ) {
		return new UniformSampler(numsamples);
	} else if ( t == Args::Pattern_Regular ) {
		return new RegularSampler(numsamples);
	} else if ( t == Args::Pattern_Jittered ) {
		return new JitteredSampler(numsamples);
	} else {
		assert(false && "Bad sampler type");
		return nullptr;
	}
}

UniformSampler::UniformSampler(int nSamples) :    
	Sampler(nSamples)
{}

Vec2f UniformSampler::getSamplePosition(int i) {
	// make the compiler shut up by referencing the variable
	(void)i;	
	// YOUR CODE HERE (R9)
	// Return a uniformly distributed random 2-vector within the unit square [0,1]^2
<<<<<<< HEAD
	
	return Vec2f(float(rand() % 100) / 100.0f, float(rand() % 100) / 100.0f);
=======

	return Vec2f();
>>>>>>> parent of dcbd1c4... finish a9
}

RegularSampler::RegularSampler(int nSamples) :
	Sampler(nSamples)
{
	// test that it's a perfect square
	int dim = (int)sqrtf(float(nSamples));
	assert(nSamples == dim * dim);
	m_dim = dim;
}

Vec2f RegularSampler::getSamplePosition(int n) {
	// YOUR CODE HERE (R9)
	// Return a sample through the center of the Nth subpixel.
	// The starter code only supports one sample per pixel.
<<<<<<< HEAD
	//assert(n == 0);
	int j = n / m_dim;
	int i = n % m_dim;
	float gridsize = 1.0f / m_dim;

	return Vec2f(gridsize*(i+1)/2, gridsize * (j+1)/2);

=======
	assert(n == 0);
	return Vec2f(0.5f, 0.5f);
>>>>>>> parent of dcbd1c4... finish a9
}

JitteredSampler::JitteredSampler(int nSamples) :
	Sampler(nSamples)
{
	// test that it's a perfect square
	int dim = (int)sqrtf(float(nSamples));
	assert(nSamples == dim * dim);
	m_dim = dim;
}

Vec2f JitteredSampler::getSamplePosition(int n) {
	// YOUR CODE HERE (R9)
	// Return a randomly generated sample through Nth subpixel.
<<<<<<< HEAD

	int j = n / m_dim;
	int i = n % m_dim;
	float gridsize = 1.0f / m_dim;
	//(rand() % (b - a + 1)) + a;
	float subi = i * gridsize  + double(gridsize) * float(rand() % 100) / 100.0f;
	float subj = j * gridsize + double(gridsize) * float(rand() % 100) / 100.0f;

	return Vec2f(subi, subj);
=======
	return Vec2f(0,0);
>>>>>>> parent of dcbd1c4... finish a9
}

