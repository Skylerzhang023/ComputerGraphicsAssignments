#include "objects.hpp"

#include "hit.hpp"
#include "VecUtils.h"

#include <cassert>

using namespace std;
using namespace FW;

Object3D* Group::operator[](int i) const {
	assert(i >= 0 && size_t(i) < size());
	return objects_[i].get();
}

void Group::insert(Object3D* o) {
	assert(o);
	objects_.emplace_back(o);
}

bool Group::intersect(const Ray& r, Hit& h, float tmin) const {
	// We intersect the ray with each object contained in the group.
	bool intersected = false;
	for (int i = 0; i < int(size()); ++i) {
		Object3D* o = objects_[i].get();
		assert(o != nullptr);
		assert(h.t >= tmin);
		bool tmp = o->intersect(r, h, tmin);
		assert(h.t >= tmin);
		if (tmp)
			intersected = true;
	}
	assert(h.t >= tmin);
	return intersected;
}

bool Box::intersect(const Ray& r, Hit& h, float tmin) const {
// YOUR CODE HERE (EXTRA)
// Intersect the box with the ray!

  return false;
}

bool Plane::intersect( const Ray& r, Hit& h, float tmin ) const {
	// YOUR CODE HERE (R5)
	// Intersect the ray with the plane.
	// Pay attention to respecting tmin and h.t!
	// Equation for a plane:
	// ax + by + cz = d;
	// normal . p - d = 0
	// (plug in ray)
	// origin + direction * t = p(t)
	// origin . normal + t * direction . normal = d;
	// t = (d - origin . normal) / (direction . normal);
	bool intersected = false;
	float t= FLT_MAX;
	//float t = (t_m < tmin) ? t_p : t_m;
	Vec3f normal = this->normal();
	Vec3f r0 = r.origin;
	Vec3f rd = r.direction;
	float d = -offset();

	if(dot(r.direction,normal)!=0)
		t = -(d + dot(normal , r0)) / dot(normal , rd);
	if (t < h.t && t>tmin) {
		intersected = true;
		h.set(t, this->material(), normal);
	}
	return intersected;

}

Transform::Transform(const Mat4f& m, Object3D* o) :
	matrix_(m),
	object_(o)
{
	assert(o != nullptr);
	inverse_ = matrix_.inverted();
	inverse_transpose_ = inverse_.transposed();
}

bool Transform::intersect(const Ray& r, Hit& h, float tmin) const {
	// YOUR CODE HERE (EXTRA)
	// Transform the ray to the coordinate system of the object inside,
	// intersect, then transform the normal back. If you don't renormalize
	// the ray direction, you can just keep the t value and do not need to
	// recompute it!
	// Remember how points, directions, and normals are transformed differently!

	return false; 
}

bool Sphere::intersect( const Ray& r, Hit& h, float tmin ) const {
	// Note that the sphere is not necessarily centered at the origin.
	
	Vec3f tmp = center_ - r.origin;
	Vec3f dir = r.direction;

	float A = dot(dir, dir);
	float B = - 2 * dot(dir, tmp);
	float C = dot(tmp, tmp) - sqr(radius_);
	float radical = B*B - 4*A*C;
	if (radical < 0)
		return false;

	radical = sqrtf(radical);
	float t_m = ( -B - radical ) / ( 2 * A );
	float t_p = ( -B + radical ) / ( 2 * A );
	Vec3f pt_m = r.pointAtParameter( t_m );
	Vec3f pt_p = r.pointAtParameter( t_p );

	assert(r.direction.length() > 0.9f);

	bool flag = t_m <= t_p;
	if (!flag) {
		::printf( "sphere ts: %f %f %f\n", tmin, t_m, t_p );
		return false;
	}
	assert( t_m <= t_p );

	// choose the closest hit in front of tmin
	float t = (t_m < tmin) ? t_p : t_m;

	if (h.t > t  && t > tmin) {
		Vec3f normal = r.pointAtParameter(t);
		normal -= center_;
		normal.normalize();
		h.set(t, this->material(), normal);
		return true;
	}
	return false;
} 

Triangle::Triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c,
	Material *m, const Vec2f& ta, const Vec2f& tb, const Vec2f& tc, bool load_mesh) :
	Object3D(m)
{
	vertices_[0] = a;
	vertices_[1] = b;
	vertices_[2] = c;
	texcoords_[0] = ta;
	texcoords_[1] = tb;
	texcoords_[2] = tc;

	if (load_mesh) {
		preview_mesh.reset((FW::Mesh<FW::VertexPNT>*)FW::importMesh("preview_assets/tri.obj"));
		set_preview_materials();
	}
}

bool Triangle::intersect( const Ray& r, Hit& h, float tmin ) const {
	// YOUR CODE HERE (R6)
	// Intersect the triangle with the ray!
	// Again, pay attention to respecting tmin and h.t!
	float t = _LMAX;
	
	Vec3f a = vertices_[0];
	Vec3f b = vertices_[1];
	Vec3f c = vertices_[2];
	//calculate the normal
	Vec3f ac = c - a;
	Vec3f ab = b - a;
	Vec3f normal = cross(ac,ab).normalized();
	//check whether the ray is parallel with the triangle
	if (dot(normal, r.direction) == 0)
		return false;

	Mat3f target;
	Mat3f target_t,target_b,target_c;
	target.setRow(0, Vec3f(a.x - b.x, a.x - c.x, r.direction.x));
	target.setRow(1, Vec3f(a.y - b.y, a.y - c.y, r.direction.y));
	target.setRow(2, Vec3f(a.z - b.z, a.z - c.z, r.direction.z));


	/*target_t.setRow(0, Vec3f(a.x - b.x, a.x - c.x, a.x - r.origin.x));
	target_t.setRow(1, Vec3f(a.y - b.y, a.y - c.y, a.y - r.origin.y));
	target_t.setRow(2, Vec3f(a.z - b.z, a.z - c.z, a.z - r.origin.z));*/

	target_t.setCol(0, Vec3f(a.x - b.x, a.y - b.y, a.z - b.z));
	target_t.setCol(1, Vec3f(a.x - c.x, a.y - c.y, a.z - c.z));
	target_t.setCol(2, Vec3f(a.x - r.origin.x, a.y - r.origin.y, a.z - r.origin.z));

	target_b.setCol(0, Vec3f(a.x - r.origin.x, a.y - r.origin.y, a.z - r.origin.z));
	target_b.setCol(1, Vec3f(a.x - c.x, a.y - c.y, a.z - c.z));
	target_b.setCol(2, Vec3f(r.direction.x, r.direction.y, r.direction.z));

	target_c.setCol(0, Vec3f(a.x - b.x, a.y - b.y, a.z - b.z));
	target_c.setCol(1, Vec3f(a.x - r.origin.x, a.y - r.origin.y, a.z - r.origin.z));
	target_c.setCol(2, Vec3f(r.direction.x, r.direction.y, r.direction.z));

	float t1, tt,tb,tc;
	t1 = target.det();
	tt = target_t.det();
	tb = target_b.det();
	tc = target_c.det();

	float c1, b1;

	t = tt / t1;
	b1 = tb / t1;
	c1 = tc / t1;
	//also need to check their sum is 1

	if (h.t > t && t > tmin && b1>=0 && b1<=1 && c1 >= 0 && c1 <= 1 && b1+c1<=1) {
		h.set(t, this->material(), -normal);
		return true;
	}
	else {
		return false;
	}
		
}

const Vec3f& Triangle::vertex(int i) const {
	assert(i >= 0 && i < 3);
	return vertices_[i];
}


