/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *  $Id: error_ellipses.h,v 1.1 2001/12/07 12:54:43 cepek Exp $
 */

#ifndef GaMa_GaMaProg_Prehled_Elipsy_Chyb_h_
#define GaMa_GaMaProg_Prehled_Elipsy_Chyb_h_

#include <gamalib/local/network.h>
#include <gamalib/statan.h>
#include <cmath>

namespace GaMaLib {

template <class OutStream>
void ErrorEllipses(GaMaLib::LocalNetwork* IS, OutStream& out)
{
   using namespace std;
   using namespace GaMaLib;
   using GaMaLib::Double;

  const int y_sign = Consistent(IS->PD, IS->OD) ? +1 : -1;
  
   const Vec& x = IS->solve();
   Double elp_k = 0;
   {
     Double alfa = (1 - IS->conf_pr());
     if (IS->m_0_apriori())
       {
	 elp_k = sqrt(GaMaLib::Chi_square(float(alfa), 2));
       }
     else
       {
	 int n = IS->degrees_of_freedom();
	 if (n > 0)
	   elp_k = sqrt( n*(pow(alfa, -2.0/n) - 1));
	 else
	   elp_k = 0;
       }
   }
   const int pocnez = IS->sum_unknowns();

   bool sour = false;
   {   // for ...
   for (int i=1; i<=pocnez; i++)
       if (IS->unknown_type(i) == 'X')
       {
         sour = true;
         break;
       }
   }   // for ...

   Double mp_max = -1, mp_prum = 0;
   PointID mp_max_cb;
   int pocbod = 0;
   
   if (sour) 
   {
     out.precision(1);
 
     out 
       << T_GaMa_errell_review_of_mean_errors_and_error_ellipses << "\n" 
       << underline(T_GaMa_errell_review_of_mean_errors_and_error_ellipses,'*')
       << "\n\n";
     out.width(IS->maxw_id());
     out << T_GaMa_point << ' ';
     out << T_GaMa_errell_header1;
     for (int i=0; i<IS->maxw_id()+1; i++) out << '=';
     out << T_GaMa_errell_header2
	 << "\n\n";
     {   // for ...
       // 1.3.13 for (int i=1; i<=pocnez; i++)
       // 1.3.13  if (IS->unknown_type(i) == 'X')
       for (PointData::const_iterator 
              point=IS->PD.begin(); point!=IS->PD.end(); ++point)
         if ((*point).second.free_xy())
           if (int i = (*point).second.index_x())
             {
               const PointID point_id  = (*point).first;	     
               out.width(IS->maxw_id());
               out << point_id.c_str() << ' ';
               
               Double my = IS->unknown_stdev(i);
               Double mx = IS->unknown_stdev(i+1);
               
               Double mp = sqrt(my*my+mx*mx);
               if (mp < 1000)     
                 out.setf(ios::fixed, ios::floatfield);
               else
                 out.setf(ios::scientific, ios::floatfield);
               out.width(7);
               out << mp << ' ';
               
               mp_prum += mp;
               if (mp > mp_max) {
                 mp_max = mp;
                 mp_max_cb = point_id;
               }
               pocbod++;
               
               Double myx = mp/sqrt(2.0);
               out.width(7);
               if (myx < 1000)     
                 out.setf(ios::fixed, ios::floatfield);
               else
                 out.setf(ios::scientific, ios::floatfield);
               out << myx << ' ' ;
               
               Double a, b, alfa;
               IS->std_error_ellipse(point_id, a, b, alfa);
               if (y_sign == -1)
                 {
                   alfa = 2*M_PI - alfa;
                 }
               out.width(7);
               if (a < 1000)     
                 out.setf(ios::fixed, ios::floatfield);
               else
                 out.setf(ios::scientific, ios::floatfield);
               out << a << ' ';
               out.width(7);
               if (b < 1000)     
                 out.setf(ios::fixed, ios::floatfield);
               else
                 out.setf(ios::scientific, ios::floatfield);
               out << b << ' ';
               out.width(7);
               out.setf(ios::fixed, ios::floatfield);
               out << alfa*R2G << ' ';
               
               if (mp < 1000 && mp > 1e-3)
                 {           // ********* testing noise (coordinates are OK)
                   Double ak = a*elp_k;
                   Double bk = b*elp_k;
                   out.width(7);
                   if (ak < 1000)     
                     out.setf(ios::fixed, ios::floatfield);
                   else
                     out.setf(ios::scientific, ios::floatfield);
                   out << ak << ' ';
                   out.width(7);
                   if (bk < 1000)     
                     out.setf(ios::fixed, ios::floatfield);
                   else
                     out.setf(ios::scientific, ios::floatfield);
                   out << bk << ' ';
                   
                   Double g  = 0;
                   Double dx = x( i );
                   Double dy = y_sign*x(i+1);
                   Double p1 = (dx*cos(alfa) + dy*sin(alfa));
                   Double p2 = (dy*cos(alfa) - dx*sin(alfa));
                   if (ak > 0 && bk > 0 && bk > ak*1e-4) 
                     {           // ***** testing noise (bk is practically 0)
                       p1 /= ak;
                       p2 /= bk;
                       g = sqrt(p1*p1 + p2*p2);
                     }
                   if (g < 1000)     
                     out.setf(ios::fixed, ios::floatfield);
                   else
                     out.setf(ios::scientific, ios::floatfield);
                   out.width(7);
                   out << g;		 
                 }
               
               out << "\n";
               out.flush();
             }
     }   // for ...
     
     if (pocbod >= 5) 
       {
         out.precision(1);
         out << endl
             << T_GaMa_adjunk_mean_position_error_maximal << mp_max
             << T_GaMa_adjunk_mean_position_error_on_point 
             << mp_max_cb << endl
             << T_GaMa_adjunk_mean_position_error_average << mp_prum/pocbod
             << " mm\n";
       }
     
     out << "\n\n";     
     
   }
}

}

#endif


