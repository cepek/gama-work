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
 *  $Id: adjusted_unknowns.h,v 1.1 2001/12/07 12:54:43 cepek Exp $
 */

#ifndef GaMa_GaMaProg_Vyrovnane_Nezname_h_
#define GaMa_GaMaProg_Vyrovnane_Nezname_h_

#include <gamalib/local/gamadata.h>
#include <gamalib/local/network.h>
#include <gamalib/cluster.h>

namespace GaMaLib {

template <class OutStream>
void AdjustedUnknowns(GaMaLib::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GaMaLib;

  const int y_sign = Consistent(IS->PD, IS->OD) ? +1 : -1;
  
  const Vec& x = IS->solve();
  Double kki = IS->conf_int_coef();
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
  if (sour)
    {
      Double mp, mp_max = -1, mp_prum = 0;
      PointID mp_max_cb, prev_id;
      int pocbod = 0;
      
      out << T_GaMa_adjunk_Review_of_unknowns_coordidantes << "\n"
          << underline(T_GaMa_adjunk_Review_of_unknowns_coordidantes, '*') 
          << "\n\n";
      out.width(IS->maxw_unk());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_point;
      out << T_GaMa_adjunk_header1;
      for (int i=0; i<IS->maxw_unk()+IS->maxw_id()+1; i++) out << '=';
      out << T_GaMa_adjunk_header2;
      out.setf(ios::fixed, ios::floatfield);

      for (PointData::const_iterator ii=IS->PD.begin(); ii!=IS->PD.end(); ii++)
        {
          const PointID point_id = (*ii).first;
          const Point&  b        = (*ii).second;

          if (b.free_xy() && b.index_x())
            {
              int i = b.index_x();
              
              out.width(IS->maxw_unk());
              out << " " << " ";
              out.width(IS->maxw_id());
              if (prev_id != point_id)
                out << point_id.c_str();
              else
                out << " ";
              prev_id = point_id;
              Double mx = IS->unknown_stdev(i);
              Double my = IS->unknown_stdev(i+1);
              mp = sqrt(my*my+mx*mx);
              out << endl;
              
              out.width(IS->maxw_unk());
              out << i << " ";
              out.width(IS->maxw_id());
              if (b.constrained_xy())
                out << "X" << " * ";
              else
                out << "x" << "   ";
              out.precision(5);
              out.width(13);
              out << b.x() << " ";
              out.width(9);
              out << (x(i)/1000) << " ";
              out.width(13);
              out << (b.x()+x(i)/1000) << " ";
              out.precision(1);
              out.width(7);
              out << mx << " ";
              out.width(7);
              out << mx*kki;
              out << "\n";
              
              out.flush();
              out.width(IS->maxw_unk());
              out << (i+1) << " ";
              out.width(IS->maxw_id());
              if (b.constrained_xy())
                out << "Y" << " * ";
              else
                out << "y" << "   ";
              out.precision(5);
              out.width(13);
              out << y_sign*b.y() << " ";
              out.width(9);
              out << y_sign*(x(i+1)/1000) << " ";
              out.width(13);
              out << y_sign*(b.y()+x(i+1)/1000) << " ";
              out.precision(1);
              out.width(7);
              out << my << " ";
              out.width(7);
              out << my*kki;
              out << "\n";              
            }
          if (b.free_z() && b.index_z())
            {
              int i = b.index_z();

              if (!b.free_xy())
                {
                  out.width(IS->maxw_unk());
                  out << " " << " ";
                  out.width(IS->maxw_id());
                  if (prev_id != point_id)
                    out << point_id.c_str();
                  else
                    out << " ";
                  out << endl;
                }
              prev_id = point_id;
              
              out.width(IS->maxw_unk());
              out << i << " ";
              out.width(IS->maxw_id());
              if (b.constrained_z())
                out << "Z" << " * ";
              else
                out << "z" << "   ";
              out.precision(5);
              out.width(13);
              out << b.z() << " ";
              out.width(9);
              out << (x(i)/1000) << " ";
              out.width(13);
              out << (b.z()+x(i)/1000) << " ";
              double mv = IS->unknown_stdev(i);
              out.precision(1);
              out.width(7);
              out << mv << " ";
              out.width(7);
              out << mv*kki;
              out << "\n";
            }

          if ((b.free_xy() && b.index_x()) ||
              (b.free_z()  && b.index_z()) ) out << endl;
        }
      
      if (pocbod >= 5) 
        {
          out.precision(1);
          out << T_GaMa_adjunk_mean_position_error_maximal << mp_max
              << T_GaMa_adjunk_mean_position_error_on_point 
              << mp_max_cb << endl
              << T_GaMa_adjunk_mean_position_error_average << mp_prum/pocbod
              << " mm\n\n";
        }

      out << endl;
    }

  bool orp = false;
  for (int i=1; i<=pocnez; i++)
    if (IS->unknown_type(i) == 'R')
      {
        orp = true;
        break;
      }
  if (orp)
    {
      out << T_GaMa_adjunk_Review_of_unknowns_bearings << "\n"
          << underline(T_GaMa_adjunk_Review_of_unknowns_bearings, '*') 
          << "\n\n";
      out.width(IS->maxw_unk());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_standpoint;
      out << T_GaMa_adjunk_header3;
      for (int i=0; i<IS->maxw_unk()+IS->maxw_id()+1; i++) out << '=';
      out << T_GaMa_adjunk_header4;
      out.flush();    // flush() sends read data to output
      
      {   // for ...
        for (int i=1; i<=pocnez; i++)
          if (IS->unknown_type(i) == 'R')
            {
              out.width(IS->maxw_unk());
              out << i << " " ;
              out.width(IS->maxw_id());
              const PointID cb = IS->unknown_pointid(i);
              out << cb.c_str() << "  ";
              StandPoint* k = IS->unknown_standpoint(i);
              Double z = y_sign*( k->orientation() )*R2G;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              out.setf(ios::fixed, ios::floatfield);
              out.precision(6);
              out.width(12);
              out << z << " ";
              out.width(10);
              out << y_sign*x(i)/10000 << " ";
              z += y_sign*x(i)/10000;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              out.width(11);
              out << z << " ";
              out.precision(1);
              out.width(8);
              Double mz = IS->unknown_stdev(i);
              out << mz << " ";
              out.width(7);
              out << mz*kki;
              out << endl;
              out.flush();
            }
      }   // for ...
      
      out << endl << endl;
    }
  

  bool vysky = false;
  {
    for (int i=1; i<=pocnez; i++)
      if (IS->unknown_type(i) == 'Z')
        {
          vysky = true;
          break;
       }
  }
  if (vysky && !sour)
    {
      out << T_GaMa_adjunk_Review_of_unknowns_heights << "\n"
          << underline(T_GaMa_adjunk_Review_of_unknowns_heights, '*') 
          << "\n\n";
      out.width(IS->maxw_unk());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_point;
      out << T_GaMa_adjunk_header5;
      { for (int i=0; i<IS->maxw_unk()+IS->maxw_id()+1; i++) out << '='; }
      out << T_GaMa_adjunk_header6;
      out.setf(ios::fixed, ios::floatfield);

      for (int i=1; i<=pocnez; i++)
        if (IS->unknown_type(i) == 'Z')
          {
            const PointID cb = IS->unknown_pointid(i);
            const Point&  b = IS->PD[cb];

            out.width(IS->maxw_unk());
            out << i << " ";
            out.width(IS->maxw_id());
            out << cb.c_str();
            if (b.constrained_z())
              out << " * ";
            else
              out << "   ";    
            out.precision(5);
            out.width(13);
            out << b.z() << " ";
            out.width(9);
            out << (x(i)/1000) << " ";
            out.width(13);
            out << (b.z()+x(i)/1000) << " ";
            double mv = IS->unknown_stdev(i);
            out.precision(1);
            out.width(7);
            out << mv << " ";
            out.width(7);
            out << mv*kki;
            out << endl;
          }

      out << "\n\n";
    }
}

}

#endif


