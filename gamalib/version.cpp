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
 *  $Id: version.cpp,v 1.6 2002/03/19 17:04:31 cepek Exp $
 */


#include <gamalib/version.h>

namespace GaMaLib {

const char* GaMaLib_version  = "1.3.32";

const char* GaMaLib_compiler =
              #ifdef __GNUC__
              "g++"      // g++ 2.95.2
              #elif defined __BORLANDC__
              "bc"       // 5.5
              #elif defined _MSC_VER
              "msc"      // 6.0
              #else
              #error GaMaLib - has not been tested with your compiler
              #endif
              ;
}


/* GaMaLib uses James Clark's parser Expat (v.1.1) for XML data processing
 *
 *    Expat is subject to the Mozilla Public License Version 1.1. 
 *    Alternatively you may use expat under the GNU General Public
 *    License instead.
 *
 *              ftp://ftp.jclark.com/pub/xml/expat.zip
 *
 * Scripts for compiling GaMaLib and linking the program GaMa expect
 * Expat library to be in the same directory as GaMaLib

=============================================================================

1.3.32 2002-03-19

   - gmatvec 0.9.17 (a bug in BandMat::cholDec(); Jan Pytel: Cholesky
     decomposition failed if the first element of the matrix was zero)
   
   - gmatvec 0.9.16 (added iterators to Mat<> / Vec<> classes)


1.3.31 2001-12-21

   - in scripts/make-linux2borland.sed added parameters needed by
     Rosinante and a minor change in scripts/Build_GaMa (Jan Pytel)

   - gmatvec 0.9.15: a bug in SymMat::invert() reported by Leos Mervart;
     in the case of dimension==1 inversion was computed twice
     and thus nothing happened (missing return statement)

   - a bug in computation of approximate coordinates reported by Jiri
     Vesely; if there were two neighbouring directions with identical
     targets, gama reported exception and stopped (the exception was
     thrown by the Angle constructor).

     From 1.3.31 we allow for angles left and right targets to be
     identical, surely this is not an realistic case but it's useful
     in g2d_point.h:67 and should not cause a trouble elsewhere


1.3.30  2001-12-07

   - a bug in ObservationData::deepCopy found by Jan Pytel (covariance
     matrices were not copied)

       diff -r1.1 gamadata.cpp
       59a60
       >       current->covariance_matrix = (*ci)->covariance_matrix;


1.3.29  2001-12-02

   - GNU GaMa sources installed at Savannah CVS repository

   - old change logs 

               from    version 1.3.28  2001-11-30
               back to         0.1.00  1998-01-19 

     are recorded in the file ChangeLog.1

*/










