/*  
   GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2003  Ales Cepek <cepek@fsv.cvut.cz>

   This file is part of the GNU Gama C++ library.
   
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

/* $Id: g3_parameter.cpp,v 1.1 2003/03/25 12:38:33 cepek Exp $  */


#include <gnu_gama/g3/g3_parameter.h>
#include <gnu_gama/list.h>


using namespace GNU_gama::g3;


void ParameterTree::add_parlist(const ParameterList& parlist)
{
  Parameter** b = parlist.begin();
  Parameter** e = parlist.end  ();
  Parameter*  p;
  
  while (b != e)
    {
      if ((p = *b))
        {
          tree.push_back(p);
          add_parlist(p->parlist);
        }
      ++b;
    }
}
