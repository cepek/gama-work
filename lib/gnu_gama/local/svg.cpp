/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2014  Maxime Le Moual <maxime.le-moual@ensg.eu>
                 2012, 2018, 2019, 2023  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file svg.cpp
 * \brief #GNU_gama::local::GamaLocalSVG class implementation
 *
 * \author Ales Cepek 2012, 2023
 * \author Maxime Le Moual 2014
 */

#include <cmath>
#include <algorithm>
#include <sstream>
#include <utility>
#include <set>
#include <iomanip>
#include <gnu_gama/local/svg.h>
#include <gnu_gama/radian.h>

using namespace GNU_gama::local;
using std::abs;   // floating point std::abs() from <cmath>

GamaLocalSVG::GamaLocalSVG(LocalNetwork* is)
  : IS(*is), PD(is->PD), OD(is->OD),
    y_sign(is->y_sign())
{
  restoreDefaults();
}

void GamaLocalSVG::restoreDefaults()
{
  tst_implicit_size = true;
  tst_draw_axes = true;
  tst_draw_point_symbols = true;
  tst_draw_point_ids = true;
  tst_draw_ellipses = true;
  tst_draw_xy_shifts = true;
  tst_draw_z_shifts = true;
  tst_draw_observations = true;

  fixedsymbol = "triangle";         fixedfill = "blue";
  constrainedsymbol = "circle";     constrainedfill = "green";
  freesymbol = "circle";            freefill = "yellow";

  xyshiftcolor = "black";
  zshiftcolor = "black";   // not implemented yet
}

void GamaLocalSVG::svg_init() const
{
  /* Original coordinate unit vectors x and y (EN, NW, ...) expressed
     in SVG coordinate system (x right, y down).

     For example in NE coordinate system axis vector x (1, 0) {points
     north} is expressed in SVG as (0, -1) {x points up}; similarly
     canonical coordinate vector y (0, 1) {pointing east} in SVG
     corresponds to (1, 0) {points right}; and parameters are
     sett(0,-1,1,0).
  */
  switch (PD.local_coordinate_system)
    {
      // right-handed
    case LocalCoordinateSystem::CS::EN: sett( 1, 0, 0,-1); break;
    case LocalCoordinateSystem::CS::NW: sett( 0,-1,-1, 0); break;
    case LocalCoordinateSystem::CS::SE: sett( 0, 1, 1, 0); break;
    case LocalCoordinateSystem::CS::WS: sett(-1, 0, 0, 1); break;
      // left-handed
    case LocalCoordinateSystem::CS::NE: sett( 0,-1, 1, 0); break;
    case LocalCoordinateSystem::CS::SW: sett( 0, 1,-1, 0); break;
    case LocalCoordinateSystem::CS::ES: sett( 1, 0, 0, 1); break;
    case LocalCoordinateSystem::CS::WN: sett(-1, 0, 0,-1); break;
    default:
      sett(0,0,0,0);
    }

  // median of error ellipsoid semiaxes x, y and main bounding box
  std::vector<double> abmed, dsmed;
  for (PointData::const_iterator i=PD.begin(), e=PD.end(); i!=e; ++i)
    {
      PointID    pid   = i->first;
      LocalPoint point = i->second;

      // skip points that are not part of the adjustment or do not have xy
      if (!point.active_xy() || !point.test_xy()) continue;

      if (tst_draw_ellipses) {
        if ((IS.is_adjusted() || IS.has_stashed_ellipses())
            && !point.fixed_xy())
          {
            double a, b, alpha;
            IS.std_error_ellipse(pid, a, b, alpha);
            abmed.push_back(a);
            abmed.push_back(b);
          }
      }

      if (tst_draw_xy_shifts) {
        auto shift = shifts.find(pid);
        if (shift != shifts.end()) {
          // dx, dy in millimeters
          double dx = std::abs(1000 * std::get<1>(shift->second));
          double dy = std::abs(1000 * std::get<2>(shift->second));

          if (dx > 1e-1 || dy > 1e-1) { // ignore small shifts
            dsmed.push_back(dx);
            dsmed.push_back(dy);
          }
        }
      }

    }

  ab_median = 0;
  if (unsigned n = abmed.size()) {
    std::sort(abmed.begin(), abmed.end());
    if (n % 2) ab_median =  abmed[n/2];
    else
      ab_median = (abmed[n/2] + abmed[n/2 - 1])/2;
  }
  if (ab_median <= 0) ab_median = 1;   // handle unrealistic data

  
  ds_median = 0;
  if (unsigned n = dsmed.size()) {
    std::sort(dsmed.begin(), dsmed.end());
    if (n % 2)
      ds_median = dsmed[n/2];
    else
      ds_median = (dsmed[n/2] + dsmed[n/2 - 1])/2;
  }
  if (ds_median <= 0)  ds_median = 1; // handle unrealistic data

  // main bounding box
  if (tst_implicit_size)
    ellipsescale = 1.0;

  { // define minx, maxx, miny, maxy and offset
    double x, y, dx, dy, tminx, tmaxx, tminy, tmaxy;
    tminx = tminy = 1e20;
    tmaxx = tmaxy = -1e20;

    T11 = TX.x;    T12 = TY.x * y_sign;
    T21 = TX.y;    T22 = TY.y * y_sign;
    // Tx  = 2*offset - minx; ... will be redefined later when
    // Ty  = 2*offset - miny;     offset, minx and miny are known
    Tx = Ty = 0;

    for (PointData::const_iterator i = PD.begin(), e = PD.end(); i != e; ++i) {
      PointID pid = i->first;
      LocalPoint point = i->second;

      // skip points that are not part of the adjustment or do not have xy
      if (!point.active_xy() || !point.test_xy())
        continue;

      svg_xy(point, x, y);

      tminx = std::min(tminx, x);
      tmaxx = std::max(tmaxx, x);
      tminy = std::min(tminy, y);
      tmaxy = std::max(tmaxy, y);
    }

    minx = tminx;
    miny = tminy;
    maxx = std::abs(tmaxx - tminx);
    maxy = std::abs(tmaxy - tminy);
    offset = (maxx + maxy) / 2 * 0.05;
  }

  T11 = TX.x;
  T12 = TY.x * y_sign;
  T21 = TX.y;
  T22 = TY.y * y_sign;
  Tx = 2 * offset - minx;
  Ty = 2 * offset - miny;

  // update parameters to fit the ellipses and x,y shifts
  // into the canvas
  {
    // the space around the canvas is wide 2*offset on all sides
    extension = 0.3;   // the outer boundary extension in % of the offset
    auto tol_right  = [this](){ return maxx + (3 + extension)*offset; };
    auto tol_left   = [this](){ return (1 - extension)*offset; };
    auto tol_top    = [this](){ return (1 - extension)*offset; };
    auto tol_bottom = [this](){ return maxy + (3 + extension)*offset; };

    for (PointData::const_iterator i = PD.begin(), e = PD.end(); i != e; ++i) {
      PointID pid = i->first;
      LocalPoint point = i->second;

      double dx {0}, dy {0};

      // skip points that are not part of the adjustment or do not have xy
      if (tst_draw_ellipses && point.free_xy() &&
          (IS.is_adjusted() || IS.has_stashed_ellipses()))
      {
        double x, y;
        svg_xy(point, x, y);
#if 0
        std::cerr << pid << "\n"
                  << "      x, y : " << x << " " << y << "      offset : " << offset << "\n";

	std::cerr << " tol_right : " << tol_right() << "\n";
	std::cerr << "  tol_left : " << tol_left()  << "\n";
	std::cerr << "   tol_top : " << tol_top()  << "\n";
	std::cerr << "  tol_bottom : " << tol_bottom()  << "\n";

#endif
        double a, b, alpha;
        svg_ellipse(pid, a, b, alpha);
        svg_ellipse_bounding_box(alpha, a, b, dx, dy);

#if 0
	auto sint = [this](double k) {
	  std::stringstream s; s << std::setw(4) << int(k);
	  return s.str();
	};
	auto spid = pid.str();
	while (spid.length() < 4) spid.append(" ");
	std::cerr << "XXX " << spid
		  << "  x/y  "   << sint(x)  << " " << sint(y) << "  "
		  << "  dx/dy  " << sint(dx) << " " << sint(dy)
		  << "   top/bottom/left/right" << " "
		  << sint(tol_top())  << " " << sint(tol_bottom()) << "   "
		  << sint(tol_left()) << " " << sint(tol_right()) << "\n";
#endif
	double skip = extension*offset, tdx = dx, tdy = dy;
	while (x+tdx > tol_right())  maxx += skip;
	while (y+tdy > tol_bottom()) maxy += skip;
	while (x-tdx < tol_left())   Tx += skip, maxx += skip, tdx -= skip;
	while (y-tdy < tol_top())    Ty += skip, maxy += skip, tdy -= skip;
      }
    }
  }



  // font and symbol sizes must be initialized only once
  // in the constructer, otherwise they could not be setup
  // by the interface

  if (tst_implicit_size)
    {
      setMinimalSize();
      ellipsescale = 1.0;
      shiftscale = 1.0;

      fontsize = offset*0.4;
      if (fontsize < minimalsize) fontsize = minimalsize;
      symbolsize  = fontsize;
      strokewidth = offset*0.01;
      if (strokewidth < minimalsize) strokewidth = minimalsize;
    }

#if 0
  std::cerr << "### initial implicit SVG units\n";
  std::cerr << "### minx        = " << minx << "\n";
  std::cerr << "### maxx        = " << maxx << "\n";
  std::cerr << "### miny        = " << miny << "\n";
  std::cerr << "### maxy        = " << maxy << "\n";
  std::cerr << "### offset      = " << offset << "\n";
  std::cerr << "### fontsize    = " << fontsize << "\n";
  std::cerr << "### symbolsize  = " << symbolsize << "\n";
  std::cerr << "### strokewidth = " << strokewidth << "\n";
#endif
}

void GamaLocalSVG::svg_xy(const LocalPoint& point, double& x, double& y) const
{
  x = T11*point.x() + T12*point.y() + Tx;
  y = T21*point.x() + T22*point.y() + Ty;
}

std::string GamaLocalSVG::string() const
{
  std::ostringstream stream;
  draw(stream);
  return stream.str();
}

void GamaLocalSVG::draw(std::ostream& output_stream) const
{
  svg = &output_stream;

  svg_init();

  const double wmaxx = 1.0*(maxx + 4*offset);
  const double wmaxy = 1.0*(maxy + 4*offset);

  *svg << "<?xml version='1.0' encoding='utf-8' standalone='no'?>\n";
  *svg <<
    "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n"
    "  'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
    "<svg version='1.1' "
    " width='"  << wmaxx << "'"
    " height='" << wmaxy << "'"
    " xmlns='http://www.w3.org/2000/svg'"
    " xmlns:xlink='http://www.w3.org/1999/xlink' >\n"
    "\n"
    "<defs>\n"
    "  <marker id='arrowhead' markerWidth='10' markerHeight='7'\n"
    "  refX='0' refY='3.5' orient='auto' style='fill:" << xyshiftcolor << "'>\n"
    "  <polygon points='0 0, 10 3.5, 0 7' />\n"
    "  </marker>\n"
    "</defs>\n";

#if 0
  *svg << "<rect x='0' y='0' "
       << "width ='" << wmaxx << "' "
       << "height='" << wmaxy << "' "
       << "style='fill:none;stroke:blue;stroke-width:"
       << strokewidth << ";' />\n";

  *svg << "<rect x='" << 2*offset << "' y='" << 2*offset << "' "
       << "width ='" << maxx << "' " << "height='" << maxy << "' "
       << "style='fill:grey;stroke:black;stroke-width:"
       << strokewidth << ";opacity:0.1' />\n";

  *svg << "<polyline points='"
       << " " << offset        << "," << offset << " "
       << " " << wmaxx-offset  << "," << offset << " "
       << " " << wmaxx-offset  << "," << wmaxy-offset << " "
       << " " << offset        << "," << wmaxy-offset << " "
       << " " << offset        << "," << offset << " "
       << "' style='fill:none;stroke:aqua;stroke-width:" << strokewidth << "' />\n";
#endif

  svg_axes_xy();
  svg_observations();
  svg_points();

  *svg << "</svg>\n";
}

void GamaLocalSVG::svg_point_shape (double x, double y,
                                    std::string /*type*/,  // Fixed Constrained Free
                                    std::string shape,     // triangle, circle
                                    std::string fillColor
                                    ) const
{
   if (shape == "triangle")
   {
      Point P(x, y);

      *svg << "<polyline points='"
           << P + Point(-0.5, 0.28868) * (1.2*symbolsize)
           << P + Point( 0.5, 0.28868) * (1.2*symbolsize)
           << P + Point( 0.0,-0.57735) * (1.2*symbolsize)
           << P + Point(-0.5, 0.28868) * (1.2*symbolsize) << "' ";
   }
   else if (shape == "circle")
   {
      *svg <<  "<circle cx='" << x << "' cy='" << y << "' "
           << "r='" << 0.5*symbolsize << "' ";
   }

   *svg << " style='" << "stroke:black;fill:" << fillColor
        << ";stroke-width:" << strokewidth<< ";'/>\n";
}

void GamaLocalSVG::svg_axes_xy() const
{
  if (!tst_draw_axes) return;

  Point  P, X, Y, CX, X1, X2, CY, Y1, Y2;
  std::string alignx, aligny;
  const double arrowlength = 4*offset;
  const double caption = 0.4*offset;
  const double arrowheadlong  = 1.5*0.3*offset;
  const double arrowheadshort = 0.3*0.3*offset;

  const double left   = offset;
  const double right  = maxx + 3*offset;
  const double top    = offset;
  const double bottom = maxy + 3*offset;

  switch (PD.local_coordinate_system)
    {
      // right handed
    case LocalCoordinateSystem::CS::EN: P = Point(left,  bottom);  break;
    case LocalCoordinateSystem::CS::NW: P = Point(right, bottom);  break;
    case LocalCoordinateSystem::CS::SE: P = Point(left,  top);     break;
    case LocalCoordinateSystem::CS::WS: P = Point(right, top);     break;

      // left handed
    case LocalCoordinateSystem::CS::NE: P = Point(left,  bottom);  break;
    case LocalCoordinateSystem::CS::SW: P = Point(right, top);     break;
    case LocalCoordinateSystem::CS::ES: P = Point(left,  top);     break;
    case LocalCoordinateSystem::CS::WN: P = Point(right, bottom);  break;

    default:
      return;
    }

  X  = P + TX*arrowlength;
  Y  = P + TY*arrowlength;
  CX = X + TX*caption;
  CY = Y + TY*caption;
  X1 = X - TX*arrowheadlong + TY*arrowheadshort;
  X2 = X - TX*arrowheadlong - TY*arrowheadshort;
  Y1 = Y - TY*arrowheadlong + TX*arrowheadshort;
  Y2 = Y - TY*arrowheadlong - TX*arrowheadshort;
  if (TX.x)
    {
      alignx = "dominant-baseline: central;";
      aligny = "text-anchor: middle;";
    }
  else
    {
      aligny = "dominant-baseline: central;";
      alignx = "text-anchor: middle;";
    }

  *svg << "<g style='stroke:black;stroke-width:" << strokewidth << ";'>\n";

  // axes
  *svg << "<polyline points='"
       << X << P << Y
       << "' style='fill:none;' />\n";

  // arrows
  *svg << "<polyline points='"
       << X1 << X << X2
       << "' style='fill:none;' />\n";
  *svg << "<polyline points='"
       << Y1 << Y << Y2
       << "' style='fill:none;' />\n";

  *svg << "</g>\n";

  // captions
  *svg << "<text font-family='sans-serif' "
       << "font-size='" << fontsize <<  "' "
       << "x='" << CX.x << "' y='" << CX.y << "' "
       << "style='" << alignx << "'>X</text>\n";
  *svg << "<text font-family='sans-serif' "
       << "x='" << CY.x << "' y='" << CY.y << "' "
       << "font-size='" << fontsize <<  "' "
       << "style='" << aligny << "'> Y</text>\n";
}

void GamaLocalSVG::svg_points() const
{
  for (PointData::const_iterator i=PD.begin(), e=PD.end(); i!=e; ++i)
    {
      PointID    pid   = i->first;
      LocalPoint point = i->second;

      // skip points that are not part of the adjustment or do not have xy
      if (!point.active_xy() || !point.test_xy()) continue;

      svg_draw_point(pid, point);
   }
}

void GamaLocalSVG::svg_observations() const
{
  if (!tst_draw_observations) return;

  typedef std::pair<PointID, PointID> PairID;
  typedef std::set<std::pair<PointID, PointID> > Pairs;

  Pairs pairs;

  typedef std::list<GNU_gama::Cluster<Observation>*> ClusterList;
  const ClusterList& clusters = OD.clusters;
  for (ClusterList::const_iterator
         c=clusters.begin(), ce=clusters.end(); c!=ce; ++c)
    {
      for (ObservationList::const_iterator
             i=(*c)->observation_list.begin(),
             e=(*c)->observation_list.end(); i!=e; ++i)
        {
          if (const Observation* b = dynamic_cast<const Observation*>(*i))
            {
              if (b->active())
                {
                  PointID from = b->from();
                  PointID to   = b->to();
                  if (from < to) pairs.insert(PairID(from, to));
                  else           pairs.insert(PairID(to, from));
                }

              if (const Angle* b = dynamic_cast<const Angle*>(*i))
                {
                  PointID from = b->from();
                  PointID to   = b->fs();
                  if (from < to) pairs.insert(PairID(from, to));
                  else           pairs.insert(PairID(to, from));
                }
            }
        }
    }

  for (Pairs::const_iterator i=pairs.begin(), e=pairs.end(); i!=e; ++i)
    {
      // operator[] cannot be used for a const map
      PointData::const_iterator a = PD.find(i->first);
      PointData::const_iterator b = PD.find(i->second);
      if (a == PD.end()) continue;
      if (b == PD.end()) continue;

      const LocalPoint& A = a->second;
      const LocalPoint& B = b->second;
      if (!A.test_xy() || !B.test_xy()) continue;

      double x1, y1, x2, y2;
      svg_xy(A, x1, y1);
      svg_xy(B, x2, y2);

      *svg << "<line x1='"<<x1<<"' y1='"<<y1<<"' x2='"<<x2<<"' y2='"<<y2<<"' "
           << "style='stroke:black;stroke-width:" << strokewidth << ";' />\n";
    }
}

void GamaLocalSVG::svg_draw_point(const PointID& pid,
                                  const LocalPoint& point) const
{
      const double psize = 0.4*fontsize;

      double x, y;
      svg_xy(point, x, y);

      if (tst_draw_xy_shifts && point.free_xy()) {
      auto shift = shifts.find(pid);
      if (shift != shifts.end()) {
            double dx = 1000 * std::get<1>(shift->second); // dx in millimeters
            double dy = 1000 * std::get<2>(shift->second);

	    dx *= offset / ds_median * ellipsescale;
	    dy *= offset / ds_median * ellipsescale;

	    if (std::abs(dx) > 1e-1 || std::abs(dy) > 1e-1)
		*svg << "<line x1='" << x << "' y1='" << y << "' x2='" << x + dx << "' y2='"
		     << y + dy << "' style='stroke:" << xyshiftcolor << ";stroke-width:1"
		     << "' marker-end='url(#arrowhead)' />\n";
      }
      }


      if (tst_draw_point_symbols)
        {
          if (point.fixed_xy())
            svg_point_shape(x, y, "Fixed", fixedsymbol, fixedfill);
          else if (point.constrained_xy())
            svg_point_shape(x, y, "Constrained",
                            constrainedsymbol, constrainedfill);
          else
            svg_point_shape(x, y, "Free",  freesymbol,  freefill);
      }

      if (tst_draw_point_ids) {
          *svg << "<text font-family='sans-serif' "
               << "transform='translate(" << 2 * psize << " " << 2 * psize << ")' "
               << "font-size='" << fontsize << "' "
               << "x='" << x << "' y='" << y << "' "
               << ">" << pid.str() << "</text>\n";
      }

      if (tst_draw_ellipses && !point.fixed_xy()
          && (IS.is_adjusted() || IS.has_stashed_ellipses())) {
          double a, b, alpha;
          svg_ellipse(pid, a, b, alpha);
#if 0
	  double dx, dy;
	  svg_ellipse_bounding_box(alpha, a, b, dx, dy);

	  *svg << "<polygon points='" << x - dx << "," << y - dy << " " // left  top
	       << x + dx << "," << y - dy << " "                        // right top
	       << x + dx << "," << y + dy << " "                        // right bottom
	       << x - dx << "," << y + dy << " "                        // left  bottom
	       << "' style='fill-opacity:0;stroke:lime;stroke-width:" << 0.5 << "' />";
#endif
	  alpha *= RAD_TO_DEG; // see gnu_gama/radian.h

	  *svg << "<ellipse  " //cx='" << x << "' cy='" << y << "' "
	       << "rx='" << a << "' ry='" << b << "' "
	       << "transform='translate(" << x << " " << y << ") "
	       << "rotate(" << alpha << ")' "
	       << "style='stroke:grey;stroke-width:" << strokewidth << ";fill:none;' />\n";
      }
}

void GamaLocalSVG::svg_ellipse(const PointID& pid, double &a, double &b, double &alpha) const
{
    IS.std_error_ellipse(pid, a, b, alpha);

    if (PD.right_handed_angles()) alpha = -alpha;

    a *= offset/ab_median*ellipsescale;
    b *= offset / ab_median * ellipsescale;

    switch (PD.local_coordinate_system)
      {
        // right handed
      case LocalCoordinateSystem::CS::EN: ; break;
      case LocalCoordinateSystem::CS::NW: alpha += M_PI/2; break;
      case LocalCoordinateSystem::CS::SE: alpha += M_PI/2; break;
      case LocalCoordinateSystem::CS::WS: ; break;
        // left handed
      case LocalCoordinateSystem::CS::NE: alpha += M_PI/2; break;
      case LocalCoordinateSystem::CS::SW: alpha += M_PI/2; break;
      case LocalCoordinateSystem::CS::ES: break;
      case LocalCoordinateSystem::CS::WN: break;
      default:
        ;
      }

     while (alpha < 0)      alpha += 2*M_PI;
     while (alpha > 2*M_PI) alpha -= 2*M_PI;
}

void GamaLocalSVG::svg_ellipse_bounding_box(
    double alpha, double a, double b, double& dx, double& dy) const
{
     double t = atan2(-b * sin(alpha), a * cos(alpha));
     double u = atan2(b * cos(alpha), a * sin(alpha));
     dx = abs(a * cos(t) * cos(alpha) - b * sin(t) * sin(alpha));
     dy = abs(b * sin(u) * cos(alpha) + a * cos(u) * sin(alpha));
}
