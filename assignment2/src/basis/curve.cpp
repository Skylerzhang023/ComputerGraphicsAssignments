#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;
using namespace FW;

namespace {

// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vec3f& lhs, const Vec3f& rhs) {
	const float eps = 1e-8f;
	return (lhs - rhs).lenSqr() < eps;
}


// This is the core routine of the curve evaluation code. Unlike
// evalBezier, this is only designed to work on 4 control points.
// Furthermore, it requires you to specify an initial binormal
// Binit, which is iteratively propagated throughout the curve as
// the curvepoints are generated. 
// Any other function that creates cubic splines can use this function by a corresponding change of basis.
Curve coreBezier(const Vec3f& p0,
				 const Vec3f& p1,
				 const Vec3f& p2,
				 const Vec3f& p3,
				 const Vec3f& Binit,
				 unsigned steps,
				bool isBezier)
{
	Curve R(steps + 1);

	// YOUR CODE HERE (R1): build the basis matrix and loop the given number of steps,
	// computing points on the spline

	Mat4f G, T;
	Mat4f B;
	Mat4f BsplineMatrix;
	// ...
	// set the bias matrix for bezier curve 

	B.setRow(0, Vec4f(1.0f, -3.0f, 3.0f, -1.0f));
	B.setRow(1, Vec4f(0.0f, 3.0f, -6.0f, 3.0f));
	B.setRow(2, Vec4f(0.0f, 0.0f, 3.0f, -3.0f));
	B.setRow(3, Vec4f(0.0f, 0.0f, 0.0f, 1.0f));

	// set the BsplineMatrix
	BsplineMatrix.setRow(0, 1.0f / 6.0f * Vec4f(1.0f, -3.0f, 3.0f, -1.0f));
	BsplineMatrix.setRow(1, 1.0f / 6.0f * Vec4f(4.0f, 0.0f, -6.0f, 3.0f));
	BsplineMatrix.setRow(2, 1.0f / 6.0f * Vec4f(1.0f, 3.0f, 3.0f, -3.0f));
	BsplineMatrix.setRow(3, 1.0f / 6.0f * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	

	//set the geometry matrix
	G.setRow(0, Vec4f(p0.x, p1.x, p2.x, p3.x));
	G.setRow(1, Vec4f(p0.y, p1.y, p2.y, p3.y));
	G.setRow(2, Vec4f(p0.z, p1.z, p2.z, p3.z));
	G.setRow(3, Vec4f(0,0, 0, 0));

	if (isBezier) {
		for (unsigned i = 0; i <= steps; ++i) {
			// ...
			// i is the index of spline point
			float steplength = 1.0f / steps * i;
			//Vec4f Tcol0 = (1, steplength,pow(steplength, 2), pow(steplength, 3));
			T.setZero();
			T.setCol(0, Vec4f(1, steplength, pow(steplength, 2), pow(steplength, 3)));
			R[i].V.x = (G * B * T).m00;
			R[i].V.y = (G * B * T).m10;
			R[i].V.z = (G * B * T).m20;
		}
	}
	else
	{
		for (unsigned i = 0; i <= steps; ++i) {
			// ...
			// i is the index of spline point
			float steplength = 1.0f / steps * i;
			//Vec4f Tcol0 = (1, steplength,pow(steplength, 2), pow(steplength, 3));
			T.setZero();
			T.setCol(0, Vec4f(1, steplength, pow(steplength, 2), pow(steplength, 3)));
			R[i].V.x = (G * BsplineMatrix * T).m00;
			R[i].V.y = (G * BsplineMatrix * T).m10;
			R[i].V.z = (G * BsplineMatrix * T).m20;
		}
	}

	return R;
}    

} // namespace

Curve coreBezier(const Vec3f& p0,
	const Vec3f& p1,
	const Vec3f& p2,
	const Vec3f& p3,
	const Vec3f& Binit,
	const float begin, const float end, const float errorbound, const float minstep) {

	// YOUR CODE HERE(EXTRA): Adaptive tessellation

	return Curve();
}
    
// the P argument holds the control points and steps gives the amount of uniform tessellation.
// the rest of the arguments are for the adaptive tessellation extra.
Curve evalBezier(const vector<Vec3f>& P, unsigned steps, bool adaptive, float errorbound, float minstep) {
    // Check
    if (P.size() < 4 || P.size() % 3 != 1) {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
		_CrtDbgBreak();
		exit(0);
	}

    // YOUR CODE HERE (R1):
    // You should implement this function so that it returns a Curve
    // (e.g., a vector<CurvePoint>).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

	// EXTRA CREDIT NOTE:
    // Also compute the other Vec3fs for each CurvePoint: T, N, B.
    // A matrix [N, B, T] should be unit and orthogonal.
    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity. The T, N and B vectors will not
	// have to be defined at points where this does not hold.

	//P is the amout of control points

	Curve R(0);
	Curve tmp(0);
	R.clear();
	tmp.clear();

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

	// to append to a std::vector, use std::insert.
	// for example, to add 'b' to the end of 'a', we'd call 'a.insert(a.end(), b.begin(), b.end());'

    cerr << "\t>>> Control points (type vector<Vec3f>): "<< endl;
    for (unsigned i = 0; i < P.size()/3; i++) {
		int p = i * 3;
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
		tmp = coreBezier(P[p], P[p + 1], P[p + 2], P[p + 3],(0,0,0),steps,true);
		R.insert(R.end(), tmp.begin(),tmp.end());
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    //cerr << "\t>>> Returning empty curve." << endl;

    // Right now this will just return this empty curve.
    return R;
}
const vector<Vec3f> BsplinetoBezier(const vector<Vec3f>& P) {
	Mat4f B, Binvert;
	Mat4f BsplineMatrix;
	Mat4f points;
	vector<Vec3f> Pnew(0);
	
	Pnew.clear();
	// set the original Bezier matrix

	B.setRow(0, Vec4f(1.0f, -3.0f, 3.0f, -1.0f));
	B.setRow(1, Vec4f(0.0f, 3.0f, -6.0f, 3.0f));
	B.setRow(2, Vec4f(0.0f, 0.0f, 3.0f, -3.0f));
	B.setRow(3, Vec4f(0.0f, 0.0f, 0.0f, 1.0f));

	// set the Bspline Matrix
	BsplineMatrix.setRow(0, (1.0f / 6.0f) * Vec4f(1.0f, -3.0f, 3.0f, -1.0f));
	BsplineMatrix.setRow(1, (1.0f / 6.0f) * Vec4f(4.0f, 0.0f, -6.0f, 3.0f));
	BsplineMatrix.setRow(2, (1.0f / 6.0f) * Vec4f(1.0f, 3.0f, 3.0f, -3.0f));
	BsplineMatrix.setRow(3, (1.0f / 6.0f) * Vec4f(0.0f, 0.0f, 0.0f, 1.0f));

	//inver the bezier matrix
	Binvert = B.inverted();

	for (unsigned i = 0; i < P.size() - 3; i++) {
		//set the geometry matrix
		points.setRow(0, Vec4f(P[i].x, P[i + 1].x, P[i + 2].x, P[i + 3].x));
		points.setRow(1, Vec4f(P[i].y, P[i + 1].y, P[i + 2].y, P[i + 3].y));
		points.setRow(2, Vec4f(P[i].z, P[i + 1].z, P[i + 2].z, P[i + 3].z));
		points.setRow(3, Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		//convert the original point
		points = points * BsplineMatrix * Binvert;
		if (i + 4 == P.size()) {
			Pnew.push_back(Vec3f(points.m00, points.m10, points.m20));
			Pnew.push_back(Vec3f(points.m01, points.m11, points.m21));
			Pnew.push_back(Vec3f(points.m02, points.m12, points.m22));
			Pnew.push_back(Vec3f(points.m03, points.m13, points.m23));
		}
		else {
			Pnew.push_back(Vec3f(points.m00, points.m10, points.m20));
		}
	}
	return Pnew;
}
// the P argument holds the control points and steps gives the amount of uniform tessellation.
// the rest of the arguments are for the adaptive tessellation extra.
Curve evalBspline(const vector<Vec3f>& P, unsigned steps, bool adaptive, float errorbound, float minstep) {
    // Check
    if (P.size() < 4) {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit(0);
    }

    // YOUR CODE HERE (R2):
    // We suggest you implement this function via a change of basis from
	// B-spline to Bezier.  That way, you can just call your evalBezier function.

	// matrix for convert bezier to bspline
	Curve Bspline(0);
	Curve tmp(steps);
	vector<Vec3f> Pnew(0);

	//define the bezier matrix and bspline matrix
	Pnew = BsplinetoBezier(P);
	//BsplineMatrix.inverted();
    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;
    cerr << "\t>>> Control points (type vector< Vec3f >): "<< endl;
	
	//call the evalBezier function
    for (unsigned i = 0; i < P.size()-3; i++) {
		vector<Vec3f> tmpp;
		//tmpp.clear();
		//tmpp.push_back(Pnew[i]);
		//tmpp.push_back(Pnew[i+1]);
		//tmpp.push_back(Pnew[i+2]);
		//tmpp.push_back(Pnew[i+3]);

		tmp = coreBezier(P[i], P[i + 1], P[i + 2], P[i + 3], (0, 0, 0), steps, false);

		Bspline.insert(Bspline.end(), tmp.begin(), tmp.end());
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }
    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

    // Return an empty curve right now.
    return Bspline;
}

Curve evalCircle(float radius, unsigned steps) {
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector<CurvePoint>).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps+1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i) {
        // step from 0 to 2pi
        float t = 2.0f * (float)M_PI * float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vec3f(FW::cos(t), FW::sin(t), 0);
        
        // Tangent vector is first derivative
        R[i].T = Vec3f(-FW::sin(t), FW::cos(t), 0);
        
        // Normal vector is second derivative
        R[i].N = Vec3f(-FW::cos(t), -FW::sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vec3f(0, 0, 1);
    }

    return R;
}

void drawCurve(const Curve& curve, float framesize) {
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Setup for line drawing
    glDisable(GL_LIGHTING); 
    glColor4f(1, 1, 1, 1);
    glLineWidth(1);
    
	if (framesize >= 0) {
		// Draw curve
		glBegin(GL_LINE_STRIP);
		for (unsigned i = 0; i < curve.size(); ++i) {
			glVertex(curve[i].V);
		}
		glEnd();
	}

    glLineWidth(1);

    // Draw coordinate frames if framesize nonzero
    if (framesize != 0.0f) {
		framesize = FW::abs(framesize);
        Mat4f M;

        for (unsigned i = 0; i < curve.size(); ++i) {
            M.setCol( 0, Vec4f( curve[i].N, 0 ) );
            M.setCol( 1, Vec4f( curve[i].B, 0 ) );
            M.setCol( 2, Vec4f( curve[i].T, 0 ) );
            M.setCol( 3, Vec4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf(M.getPtr());
            glScaled(framesize, framesize, framesize);
            glBegin(GL_LINES);
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

