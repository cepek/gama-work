/*  
    C++ Matrix/Vector templates (GNU Gama / gMatVec 0.9.22)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the gMatVec C++ Matrix/Vector template library.
    
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
 *  $Id: sortvec.h,v 1.10 2003/08/14 16:20:25 cepek Exp $
 *  http://www.gnu.org/software/gama/
 */

#ifndef gMatVec_Sort_Vec__h_
#define gMatVec_Sort_Vec__h_

#include <algorithm>
#include <gmatvec/vecbase.h>


namespace gMatVec {

template <class Float, class Exc>
inline void sort(Vec<Float, Exc>& v)
  {
    typename Vec<Float, Exc>::iterator b = v.begin();
    typename Vec<Float, Exc>::iterator e = v.end();
    std::sort(b, e);
  }


}   // namespace gMatVec

#endif



