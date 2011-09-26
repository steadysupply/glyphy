/*
 * Copyright © 2011  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#include <math.h>

#ifndef GEOMETRY_HH
#define GEOMETRY_HH

typedef double Coord;
typedef double Scalar;

template <typename Type> struct Pair;
template <typename Coord> struct Vector;
template <typename Coord> struct Point;
template <typename Coord> struct Line;
template <typename Coord, typename Scalar> struct Circle;
template <typename Coord, typename Scalar> struct Arc;
template <typename Coord> struct Bezier;


template <typename Type>
struct Pair {
  inline Pair (const Type &first_, const Type &second_) : first (first_), second (second_) {}

  Type first, second;
};

template <typename Coord>
struct Vector {
  inline Vector (Coord dx_, Coord dy_) : dx (dx_), dy (dy_) {};
  inline explicit Vector (Point<Coord> p) : dx (p.x), dy (p.y) {};

  inline operator bool (void) const;
  inline bool operator == (const Vector<Coord> &v) const;
  inline bool operator != (const Vector<Coord> &v) const;
  inline const Vector<Coord> operator- (void) const;
  inline Vector<Coord>& operator+= (const Vector<Coord> &v);
  inline Vector<Coord>& operator-= (const Vector<Coord> &v);
  template <typename Scalar>
  inline Vector<Coord>& operator*= (const Scalar &s);
  template <typename Scalar>
  inline Vector<Coord>& operator/= (const Scalar &s);
  inline const Vector<Coord> operator+ (const Vector<Coord> &v) const;
  inline const Vector<Coord> operator- (const Vector<Coord> &v) const;
  template <typename Scalar>
  inline const Vector<Coord> operator* (const Scalar &s) const;
  template <typename Scalar>
  inline const Vector<Coord> operator/ (const Scalar &s) const;
  inline const Point<Coord> operator+ (const Point<Coord> &p) const;

  inline Scalar len (void) const;
  inline const Vector<Coord> normalized (void) const;
  inline const Vector<Coord> perpendicular (void) const;
  inline const Vector<Coord> normal (void) const; /* perpendicular().normalized() */
  inline Scalar angle (void) const;

  inline const Vector<Coord> rebase (const Vector<Coord> &bx, const Vector<Coord> &by) const;
  inline const Vector<Coord> rebase (const Vector<Coord> &bx) const;

  Coord dx, dy;
};

template <typename Coord>
struct Point {
  inline Point (Coord x_, Coord y_) : x (x_), y (y_) {};
  inline explicit Point (Vector<Coord> v) : x (v.dx), y (v.dy) {};

  inline operator bool (void) const;
  inline bool operator == (const Point<Coord> &p) const;
  inline bool operator != (const Point<Coord> &p) const;
  inline Point<Coord>& operator+= (const Vector<Coord> &v);
  inline Point<Coord>& operator-= (const Vector<Coord> &v);
  inline const Point<Coord> operator+ (const Vector<Coord> &v) const;
  inline const Point<Coord> operator- (const Vector<Coord> &v) const;
  inline const Vector<Coord> operator- (const Point<Coord> &p) const;
  inline const Point<Coord> operator+ (const Point<Coord> &v) const; /* mid-point! */
  inline const Scalar operator- (const Line<Coord> &l) const; /* distance to line! */
  inline const Line<Coord> operator| (const Point<Coord> &p) const; /* segment axis line! */

  inline const Point<Coord> lerp (Scalar a, const Point<Coord> &p) const;

  Coord x, y;
};

template <typename Coord>
struct Line {
  inline Line (Coord a_, Coord b_, Coord c_) : a (a_), b (b_), c (c_) {};
  inline Line (const Point<Coord> &p0, const Point<Coord> &p1);

  inline operator bool (void) const;
  inline const Point<Coord> operator+ (const Line<Coord> &l) const; /* line intersection! */

  inline const Line<Coord> normalized (void) const;
  inline const Vector<Coord> normal (void) const;

  Coord a, b, c; /* a*x + b*y = c */
};

template <typename Coord, typename Scalar>
struct Circle {
  inline Circle (const Point<Coord> &c_, Scalar r_) : c (c_), r (r_) {};
  inline Circle (const Point<Coord> &p0, const Point<Coord> &p1, const Point<Coord> &p2) :
		 c ((p0|p1) + (p2|p1)), r ((c - p0).len ()) {}

  inline operator bool (void) const;
  inline bool operator == (const Circle<Coord, Scalar> &c) const;
  inline bool operator != (const Circle<Coord, Scalar> &c) const;

  Point<Coord> c;
  Scalar r;
};

template <typename Coord, typename Scalar>
struct Arc {
  inline Arc (const Circle<Coord, Scalar> &c_, Scalar a0_, Scalar a1_) :
	      c (c_), a0 (a0_), a1 (a1_) {};
  inline Arc (const Circle<Coord, Scalar> &c_, const Point<Coord> &p0_, const Point<Coord> &p1_) :
	      c (c_), a0 ((p0_ - c_.c).angle ()), a1 ((p1_ - c_.c).angle ()) {};

  inline operator bool (void) const;
  inline bool operator == (const Arc<Coord, Scalar> &a) const;
  inline bool operator != (const Arc<Coord, Scalar> &a) const;

  Circle<Coord, Scalar> c;
  Scalar a0, a1;
};

template <typename Coord>
struct Bezier {
  inline Bezier (const Point<Coord> &p0_, const Point<Coord> &p1_,
		 const Point<Coord> &p2_, const Point<Coord> &p3_) :
		 p0 (p0_), p1 (p1_), p2 (p2_), p3 (p3_) {}

  template <typename Scalar>
  inline const Point<Coord> point (Scalar t) const;

  template <typename Scalar>
  inline const Vector<Coord> tangent (Scalar t) const;

  template <typename Scalar>
  inline const Vector<Coord> normal (Scalar t) const;

  template <typename Scalar>
  inline const Pair<Bezier<Coord> > split (Scalar t) const;

  inline const Pair<Bezier<Coord> > halve (void) const;

  Point<Coord> p0, p1, p2, p3;
};


/* Implementations */

/* Vector */

template <typename Coord>
inline Vector<Coord>::operator bool (void) const {
  return dx || dy;
}
template <typename Coord>
inline bool Vector<Coord>::operator == (const Vector<Coord> &v) const {
  return dx == v.dx && dy == v.dy;
}
template <typename Coord>
inline bool Vector<Coord>::operator != (const Vector<Coord> &v) const {
  return !(*this == v);
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::operator- (void) const {
  return Vector<Coord> (-dx, -dy);
}
template <typename Coord>
inline Vector<Coord>& Vector<Coord>::operator+= (const Vector<Coord> &v) {
  dx += v.dx;
  dy += v.dy;
  return *this;
}
template <typename Coord>
inline Vector<Coord>& Vector<Coord>::operator-= (const Vector<Coord> &v) {
  dx -= v.dx;
  dy -= v.dy;
  return *this;
}
template <typename Coord> template <typename Scalar>
inline Vector<Coord>& Vector<Coord>::operator*= (const Scalar &s) {
  dx *= s;
  dy *= s;
  return *this;
}
template <typename Coord> template <typename Scalar>
inline Vector<Coord>& Vector<Coord>::operator/= (const Scalar &s) {
  dx /= s;
  dy /= s;
  return *this;
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::operator+ (const Vector<Coord> &v) const {
  return Vector<Coord> (*this) += v;
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::operator- (const Vector<Coord> &v) const {
  return Vector<Coord> (*this) -= v;
}
template <typename Coord> template <typename Scalar>
inline const Vector<Coord> Vector<Coord>::operator* (const Scalar &s) const {
  return Vector<Coord> (*this) *= s;
}
template <typename Coord> template <typename Scalar>
inline const Vector<Coord> Vector<Coord>::operator/ (const Scalar &s) const {
  return Vector<Coord> (*this) /= s;
}
template <typename Coord>
inline const Point<Coord> Vector<Coord>::operator+ (const Point<Coord> &p) const {
  return p + *this;
}

template <typename Coord>
inline Scalar Vector<Coord>::len (void) const {
  return hypot (dx, dy);
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::normalized (void) const {
  Scalar d = len ();
  return d ? *this / d : *this;
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::perpendicular (void) const {
  return Vector<Coord> (-dy, dx);
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::normal (void) const {
  return perpendicular ().normalized ();
}
template <typename Coord>
inline Scalar Vector<Coord>::angle (void) const {
  return atan2 (dy, dx);
}

template <typename Coord>
inline const Vector<Coord> Vector<Coord>::rebase (const Vector<Coord> &bx,
						  const Vector<Coord> &by) const {
  return Vector<Coord> (dx * bx.dx + dy * bx.dy,
		        dx * by.dx + dy * by.dy /* XXX times */);
}
template <typename Coord>
inline const Vector<Coord> Vector<Coord>::rebase (const Vector<Coord> &bx) const {
  return rebase (bx, bx.perpendicular ());
}


/* Point */

template <typename Coord>
inline Point<Coord>::operator bool (void) const {
  return isnormal (x) && isnormal (y);
}
template <typename Coord>
inline bool Point<Coord>::operator == (const Point<Coord> &p) const {
  return x == p.x && y == p.y;
}
template <typename Coord>
inline bool Point<Coord>::operator != (const Point<Coord> &p) const {
  return !(*this == p);
}
template <typename Coord>
inline Point<Coord>& Point<Coord>::operator+= (const Vector<Coord> &v) {
  x += v.dx;
  y += v.dy;
  return *this;
}
template <typename Coord>
inline Point<Coord>& Point<Coord>::operator-= (const Vector<Coord> &v) {
  x -= v.dx;
  y -= v.dy;
  return *this;
}
template <typename Coord>
inline const Point<Coord> Point<Coord>::operator+ (const Vector<Coord> &v) const {
  return Point<Coord> (*this) += v;
}
template <typename Coord>
inline const Point<Coord> Point<Coord>::operator- (const Vector<Coord> &v) const {
  return Point<Coord> (*this) -= v;
}
template <typename Coord>
inline const Vector<Coord> Point<Coord>::operator- (const Point<Coord> &p) const {
  return Vector<Coord> (x - p.x, y - p.y);
}

template <typename Coord>
inline const Point<Coord> Point<Coord>::operator+ (const Point<Coord> &p) const { /* mid-point! */
  return *this + (p - *this) / 2;
}
template <typename Coord>
inline const Scalar Point<Coord>::operator- (const Line<Coord> &l) const { /* distance to line! */
  return (l.a * x + l.b * y - l.c) / hypot (l.a, l.b) /*XXX times? */;
}
template <typename Coord>
inline const Line<Coord> Point<Coord>::operator| (const Point<Coord> &p) const { /* segment axis line! */
  Vector<Coord> d = p - *this;
  return Line<Coord> (d.dx * 2, d.dy * 2,
		      (d.dx * p.x + d.dy * p.y /* XXX times */) +
		      (d.dx *   x + d.dy *   y /* XXX times */));
}

template <typename Coord>
inline const Point<Coord> Point<Coord>::lerp (Scalar a, const Point<Coord> &p) const {
  return Point<Coord> ((1-a) * x + a * p.x, (1-a) * y + a * p.y);
}


/* Line */
template <typename Coord>
Line<Coord>::Line (const Point<Coord> &p0, const Point<Coord> &p1)
{
  Vector<Coord> n = (p1 - p0).perpendicular ();
  a = n.dx;
  b = n.dy;
  c = n.dx * p0.x + n.dy * p0.y /* XXX times */;
}

template <typename Coord>
inline Line<Coord>::operator bool (void) const {
  return a || b;
}
template <typename Coord>
inline const Point<Coord> Line<Coord>::operator+ (const Line<Coord> &l) const {
  Scalar det = a * l.b - b * l.a;
  if (!det)
    return Point<Coord> (/* XXX */ INFINITY, INFINITY);
  return Point<Coord> ((c * l.b - b * l.c) / det,
		       (a * l.c - c * l.a) / det);
}

template <typename Coord>
inline const Line<Coord> Line<Coord>::normalized (void) const {
  Scalar d = normal ().len ();
  return d ? Line<Coord> (a / d, b / d, c / d) : *this;
}
template <typename Coord>
inline const Vector<Coord> Line<Coord>::normal (void) const {
  return Vector<Coord> (a, b);
}


/* Circle */

template <typename Coord, typename Scalar>
inline Circle<Coord, Scalar>::operator bool (void) const {
  return r;
}
template <typename Coord, typename Scalar>
inline bool Circle<Coord, Scalar>::operator == (const Circle<Coord, Scalar> &c_) const {
  return c == c_.c && r == c_.r;
}
template <typename Coord,  typename Scalar>
inline bool Circle<Coord, Scalar>::operator != (const Circle<Coord, Scalar> &c) const {
  return !(*this == c);
}


/* Arc */

template <typename Coord, typename Scalar>
inline Arc<Coord, Scalar>::operator bool (void) const {
  return c && a0 != a1;
}
template <typename Coord, typename Scalar>
inline bool Arc<Coord, Scalar>::operator == (const Arc<Coord, Scalar> &a) const {
  return c == a.c && a0 == a.a0 && a1 == a.a1;
}
template <typename Coord,  typename Scalar>
inline bool Arc<Coord, Scalar>::operator != (const Arc<Coord, Scalar> &a) const {
  return !(*this == a);
}


/* Bezier */

template <typename Coord> template <typename Scalar>
inline const Point<Coord> Bezier<Coord>::point (Scalar t) const {
  Point<Coord> p01 = p0.lerp (t, p1);
  Point<Coord> p12 = p1.lerp (t, p2);
  Point<Coord> p23 = p2.lerp (t, p3);
  Point<Coord> p012 = p01.lerp (t, p12);
  Point<Coord> p123 = p12.lerp (t, p23);
  Point<Coord> p0123 = p012.lerp (t, p123);
  return p0123;
}

template <typename Coord> template <typename Scalar>
inline const Vector<Coord> Bezier<Coord>::tangent (Scalar t) const
{
  double t_2_0 = t * t;
  double t_0_2 = (1 - t) * (1 - t);

  double _1__4t_1_0_3t_2_0 = 1 - 4 * t + 3 * t_2_0;
  double _2t_1_0_3t_2_0    =     2 * t - 3 * t_2_0;

  return Vector<Coord> (-3 * p0.x * t_0_2
			+3 * p1.x * _1__4t_1_0_3t_2_0
			+3 * p2.x * _2t_1_0_3t_2_0
			+3 * p3.x * t_2_0,
			-3 * p0.y * t_0_2
			+3 * p1.y * _1__4t_1_0_3t_2_0
			+3 * p2.y * _2t_1_0_3t_2_0
			+3 * p3.y * t_2_0);
}

template <typename Coord> template <typename Scalar>
inline const Vector<Coord> Bezier<Coord>::normal (Scalar t) const {
  return Vector<Coord> (6 * ((-p0.x + 3*p1.x - 3*p2.x + p3.x) * t + (p0.x - 2*p1.x + p2.x)),
			6 * ((-p0.y + 3*p1.y - 3*p2.y + p3.y) * t + (p0.y - 2*p1.y + p2.y)));
}

template <typename Coord> template <typename Scalar>
inline const Pair<Bezier<Coord> > Bezier<Coord>::split (Scalar t) const {
  Point<Coord> p01 = p0.lerp (t, p1);
  Point<Coord> p12 = p1.lerp (t, p2);
  Point<Coord> p23 = p2.lerp (t, p3);
  Point<Coord> p012 = p01.lerp (t, p12);
  Point<Coord> p123 = p12.lerp (t, p23);
  Point<Coord> p0123 = p012.lerp (t, p123);
  return Pair<Bezier<Coord> > (Bezier<Coord> (p0, p01, p012, p0123),
			       Bezier<Coord> (p0123, p123, p23, p3));
}

template <typename Coord>
inline const Pair<Bezier<Coord> > Bezier<Coord>::halve (void) const
{
  Point<Coord> p01 = p0 + p1;
  Point<Coord> p12 = p1 + p2;
  Point<Coord> p23 = p2 + p3;
  Point<Coord> p012 = p01 + p12;
  Point<Coord> p123 = p12 + p23;
  Point<Coord> p0123 = p012 + p123;
  return Pair<Bezier<Coord> > (Bezier<Coord> (p0, p01, p012, p0123),
			       Bezier<Coord> (p0123, p123, p23, p3));
}


#endif
