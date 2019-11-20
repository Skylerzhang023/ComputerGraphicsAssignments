#include "raytracer.hpp"

#include "args.hpp"
#include "Camera.h"
#include "hit.hpp"
#include "lights.hpp"
#include "material.hpp"
#include "objects.hpp"
#include "ray.hpp"
#include "SceneParser.h"

#define EPSILON 0.001f

using namespace std;
using namespace FW;

namespace {

// Compute the mirror direction for the incoming direction, given the surface normal.
Vec3f mirrorDirection(const Vec3f& normal, const Vec3f& incoming) {
	// YOUR CODE HERE (R8)
	// Pay attention to the direction which things point towards, and that you only
	// pass in normalized vectors.
	Vec3f Refdirection =incoming - 2 * dot(incoming, normal) * normal;
	return Refdirection;
}

bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming, 
		float index_i, float index_t, Vec3f& transmitted) {
	// YOUR CODE HERE (EXTRA)
	// Compute the transmitted direction for the incoming direction, given the surface normal
	// and indices of refraction. Pay attention to the direction which things point towards!
	// You should only pass in normalized vectors!
	// The function should return true if the computation was successful, and false
	// if the transmitted direction can't be computed due to total internal reflection.
	return true;
}

} // namespace

Vec3f RayTracer::traceRay(Ray& ray, float tmin, int bounces, float refr_index, Hit& hit, FW::Vec3f debug_color) const {
	// initialize a hit to infinitely far away
	hit = Hit(FLT_MAX);

	// Ask the root node (the single "Group" in the scene) for an intersection.
	bool intersect = false;
	if(scene_.getGroup())
		intersect = scene_.getGroup()->intersect(ray, hit, tmin);

	// Write out the ray segment if visualizing (for debugging purposes)
	if (debug_trace)
		debug_rays.push_back(RaySegment(ray.origin, ray.direction.normalized()*min(100.0f,hit.t), hit.normal, debug_color));

	// if the ray missed, we return the background color.
	if (!intersect)
		return scene_.getBackgroundColor();
	
	Material* m = hit.material;
	assert(m != nullptr);

	// get the intersection point and normal.
	Vec3f normal = hit.normal;
	Vec3f point = ray.pointAtParameter(hit.t);

	// YOUR CODE HERE (R1)
	// Apply ambient lighting using the ambient light of the scene
	// and the diffuse color of the material.
	Vec3f answer = scene_.getAmbientLight() * m->diffuse_color(point);

	// YOUR CODE HERE (R4 & R7)
	// For R4, loop over all the lights in the scene and add their contributions to the answer.
	// If you wish to include the shadow rays in the debug visualization, insert the segments
	// into the debug_rays list similar to what is done to the primary rays after intersection.
	// For R7, if args_.shadows is on, also shoot a shadow ray from the hit point to the light
	// to confirm it isn't blocked; if it is, ignore the contribution of the light.
	//r4
	for (int i = 0; i < scene_.getNumLights(); ++i) {
		Vec3f dir_to_light, incident_intensity;
		float distance;
		scene_.getLight(i)->getIncidentIllumination(ray.pointAtParameter(hit.t), dir_to_light, incident_intensity, distance);
		//for r7 
		bool blocked = false;
		Hit hit2 = Hit();
		if (args_.shadows) {
			Ray shadowray(ray.pointAtParameter(hit.t-0.001f),dir_to_light);
			blocked = scene_.getGroup()->intersect(shadowray, hit2, tmin);

			if (!blocked) {
				if (dot(dir_to_light, hit.normal))
					answer = answer + m->shade(ray, hit, dir_to_light, incident_intensity, false);
				else
					answer = answer + m->shade(ray, hit, dir_to_light, incident_intensity, true);
			}
		}
		else {
			if (dot(dir_to_light, hit.normal))
				answer = answer + m->shade(ray, hit, dir_to_light, incident_intensity, false);
			else
				answer = answer + m->shade(ray, hit, dir_to_light, incident_intensity, true);

		}

			
	}

	

	// are there bounces left?
	if (bounces >= 1) {
		// reflection, but only if reflective coefficient > 0!
		if (m->reflective_color(point).length() > 0.0f) {
			// YOUR CODE HERE (R8)
			// Generate and trace a reflected ray to the ideal mirror direction and add
			// the contribution to the result. Remember to modulate the returned light
			// by the reflective color of the material of the hit point.

			float epsilon=0.01f; //for avoiding self reflection
			Vec3f Refdirection = mirrorDirection(normal, ray.direction);
			Ray RefRay(point, Refdirection);
			Hit Ref = Hit();
			bounces--;
			answer = answer + refr_index * m->reflective_color(point)*traceRay(RefRay, epsilon, bounces, refr_index, Ref, debug_color);

		}

		// refraction, but only if surface is transparent!
		if (m->transparent_color(point).length() > 0.0f) {
			// YOUR CODE HERE (EXTRA)
			// Generate a refracted direction and trace the ray. For this, you need
			// the index of refraction of the object. You should consider a ray going through
			// the object "against the normal" to be entering the material, and a ray going
			// through the other direction as exiting the material to vacuum (refractive index=1).
			// (Assume rays always start in vacuum, and don't worry about multiple intersecting
			// refractive objects!) Remembering this will help you figure out which way you
			// should use the material's refractive index. Remember to modulate the result
			// with the material's refractiveColor().
			// REMEMBER you need to account for the possibility of total internal reflection as well.
		}
	}
	return answer;
}
