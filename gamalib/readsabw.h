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
 *  $Id: readsabw.h,v 1.1 2001/12/07 12:22:42 cepek Exp $
 */

#ifndef GaMaLib_read_sparse_Ab_h
#define GaMaLib_read_sparse_Ab_h

#include <gmatvec/gmatvec.h>
#include <iostream>

namespace GaMaLib {


/*
   Reads A, b and w, sparse matrix of project equations with diagonal weights.

        Ax = b,   w=diag(w1, w2, ..., w_m),   x = inv(At*w*A)*(At*w*b)
*/
   
template <class Float, class Exc>
void Read_Sparse_Abw(std::istream& inp, 
                     gMatVec::Mat<Float, Exc>& A,
                     gMatVec::Vec<Float, Exc>& b, 
                     gMatVec::Vec<Float, Exc>& w)
{
   int M, N;
   inp >> N >> M;                   // number of unknowns and observations
   A.reset(M, N);
   b.reset(M);
   w.reset(M);

   A.set_zero();

   int* ind = new int[N];
   for (int n, r=1; r<=M; r++)      // all project equations 1, 2, ..., M
      {
         inp >> n;                  // number of nonzero coefficients
         for (int i=0; i<n; i++)
            inp >> ind[i];          // list of indexes of nonzero coefficients
         inp >> w(r) >> b(r);       // weight and rhs
         for (int j=0; j<n; j++)
            inp >> A(r,ind[j]);     // list of nonzero coefficients
      }
      delete[] ind;
}

}      // namespace GaMaLib

#endif






