#include <iostream>
#include <map>
#include <vector>

#include "gama-local-defo.h"

using namespace GNU_gama::local;

bool GamaLocalDeformation::check_arguments(std::ostream& out, int argc, char *argv[])
{
  auto help = R"(
https://www.gnu.org/software/gama/

Usage: gama-local-deformation epoch1.xml epoch2.xml [--text file] [--svg file]
       gama-local-deformation --version
       gama-local-deformation --help

Options:

epoch1 and epoch2 are adjustment results in XML format of the surveying network.
           The program computes the shift vectors of common adjusted points
           and their corresponding covariance matrix.

--text     deformation analyses in textual format. If missing, standard
           output device is used (i.e. screen).
--svg      if defined, the program writes SVG image of the second epoch
           adjustment with standard deviation ellipses and points' shits.
           The network schema is available only in 2D (xy coordinates only).
--version
--help


Report bugs to: <bug-gama@gnu.org>
GNU gama home page: <https://www.gnu.org/software/gama/>
General help using GNU software: <https://www.gnu.org/gethelp/>
)";

  auto print_help = [help](std::ostream& out) { out << help; return false; };

  static const std::map<std::string, std::string> equivalents
      {
          {"-help", "-h"}, {"--help", "-h"},
          {"-version", "-v"}, {"--version", "-v"},
          {"--svg", "-svg"},
          {"--text", "-text"}
      };

  std::vector<std::string> epoch;

  for (int i=1; i<argc; i++)
  {
    if (argv[i][0] == '-')
    {
      std::string argvi = argv[i];
      auto ptr_argvi = equivalents.find(argvi);
      if (ptr_argvi != equivalents.end()) argvi = ptr_argvi->second;

      if (argvi == "-h") {
        return print_help(out);
      }

      if (argvi == "-v") {
        out << version() << "\n";
        return true;
      }

      if (argvi == "-svg") {
        if (i+1 <= argc){
          argv_svg_file = argv[++i];
        }
        else {
          return print_help(out);
        }
      }

      if (argvi == "-text") {
        if (i+1 <= argc) {
          argv_txt_file = argv[++i];
        }
        else {
          return print_help(out);
        }
      }
    }
    else {
      epoch.push_back(argv[i]);
    }
  }

  if (epoch.size() != 2) return print_help(out);
  argv_epoch1 = epoch[0];
  argv_epoch2 = epoch[1];

  return true;
}


