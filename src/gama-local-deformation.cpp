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

#include <gnu_gama/local/deformation.h>
#include <gnu_gama/local/svg.h>

int main(int argc, char *argv[])
{
    GNU_gama::local::GamaLocalDeformation gamlocdef;

    // to display the exit code in bash run echo $?
    auto status = gamlocdef.check_arguments(std::cerr, argc, argv);

    if (status < 0 ) return 0;  // --help or --version
    if (status == 0)
    {
      gamlocdef.write_txt();
      gamlocdef.write_svg();
    }

    if (gamlocdef.bad())
    {
      std::cerr << "\n";
      for (auto& error : gamlocdef.errors()) std::cerr << "####  " << error << "\n";
      std::cerr << "\n";
    }

    if (status > 0) return status;
    return gamlocdef.errors().size();
}
