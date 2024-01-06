/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2013  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#ifndef gama_local_deformation
#define gama_local_deformation

#include <map>
#include <string>
#include <vector>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

#ifdef DEBUG_GAMA_LOCAL_DEFORMATION
#include <iomanip>
#endif

namespace GNU_gama { namespace local {

class GamaLocalDeformation {

  using Results = GNU_gama::LocalNetworkAdjustmentResults;
  std::string argv_svg_file, argv_txt_file, argv_epoch1, argv_epoch2;

  std::vector<std::string> vec_errors;

  Results* ptr_epoch_1 {nullptr};
  Results* ptr_epoch_2 {nullptr};

  struct RecDiff {
    std::string id;
    int indx {0}; double dx {0};
    int indy {0}; double dy {0};
    int indz {0}; double dz {0};

    //int dim {0};
  };

  struct Rec12 {
    std::string id;
    int indx1 {0}; double x1 {0};
    int indy1 {0}; double y1 {0};
    int indz1 {0}; double z1 {0};
    int indx2 {0}; double x2 {0};
    int indy2 {0}; double y2 {0};
    int indz2 {0}; double z2 {0};

    bool empty() const { return dim() == 0; }

    int dim() const {
      int d = 0;
      if (indz1*indz2) d += 1;
      if (indx1*indx2) d += 2;
      return d;
    }

#ifdef DEBUG_GAMA_LOCAL_DEFORMATION
    friend std::ostream& operator<<(std::ostream& os, const Rec2& rec);
#endif
  };

#ifdef DEBUG_GAMA_LOCAL_DEFORMATION
  friend std::ostream& operator<<(std::ostream& os, const Rec2& rec)
  {
    using std::setw;
    os << "  " << rec.id << "  "
       << setw(3) << rec.indx1 << " "
       << setw(3) << rec.indy1 << " "
       << setw(3) << rec.indz1 << setw(3) << " "
       << rec.x1    << " " << rec.y1    << " " << rec.z1    << " \t"
       << setw(3) << rec.indx2 << " "
       << setw(3) << rec.indy2 << " "
       << setw(3) << rec.indz2 << "   "
       << rec.x2    << " " << rec.y2    << " " << rec.z2 << "\n"
        ;
    return os;
  }
#endif

  void init();

  std::map<std::string, RecDiff> adjdiff;
  std::map<std::string, Rec12>   adjrec12;
  bool is_ready;

  std::vector<int> t1 {0}, t2 {0};  // 1 based index transformation
  int cov_index {0};

public:

  GamaLocalDeformation() : is_ready(false) {}

  ~GamaLocalDeformation()
  {
    delete ptr_epoch_1;
    delete ptr_epoch_2;
  }

  std::string version() const { return "0.3"; }

  bool good() const { return  vec_errors.empty(); }
  bool bad()  const { return !vec_errors.empty(); }

  const std::vector<std::string>& errors() const { return vec_errors; }

  int  check_arguments(std::ostream& out,int argc, char *argv[]);
  void write_txt();
  void write_svg();

  std::string str_epoch1() const { return argv_epoch1;   }
  std::string str_epoch2() const { return argv_epoch2;   }
  Results*    ptr_epoch1() const { return ptr_epoch_1;   }
  Results*    ptr_epoch2() const { return ptr_epoch_2;   }
  std::string txt_file()   const { return argv_txt_file; }
  std::string svg_file()   const { return argv_svg_file; }
  };

}}

#endif
