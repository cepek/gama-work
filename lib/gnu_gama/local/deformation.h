#ifndef gama_local_deformation
#define gama_local_deformation

#include <string>

namespace GNU_gama { namespace local {

class GamaLocalDeformation {

  std::string argv_svg_file, argv_txt_file, argv_epoch1, argv_epoch2;

public:

  std::string version() const { return "0.21"; }
  bool check_arguments(std::ostream& out,int argc, char *argv[]);

  std::string epoch1()   const { return argv_epoch1;   }
  std::string epoch2()   const { return argv_epoch2;   }
  std::string txt_file() const { return argv_txt_file; }
  std::string svg_file() const { return argv_svg_file; }
};

}}

#endif
