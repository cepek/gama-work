/*  
    GNU Gama -- program 'gnu_gama_dep' for genering project makefiles
    Copyright (C) 2003  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU Gama library.
    
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
 *  $Id: gnu_gama_dep.cpp,v 1.4 2003/12/24 11:34:11 uid66336 Exp $
 */

#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <set>

const char* version = "1.0";

/*************************************************************************
 * 
 * 1.0  - added comments into the 'gnu_gama_files' (lines starting with #)
 * 0.9  - various changes needed for GNU Gama to be compiled with
 *        expat version 1.95.2 (or any later). Old version 1.1 of expat 
 *        parser is still available as an alternative
 * 0.8  - accepts include directives with spaces between '#' and `include'
 *      - avoids possible recursive dependences
 * 0.7  gamalib_dep renamed to gnu_gama_dep; all bash  scripts used
 *      for generating makefiles are removed
 * 0.6  added directory <gnu_gama/ ... > for processing
 * 0.5  added include <iostream> for g++ 3.0.4 
 * 0.4  .o changed to .$(OBJ) for
 * 0.3  `name' not written to output
 * 0.2  added SRC make macro (2000-11-11)
 *  
 *************************************************************************/


enum Projects  { t_lib, 
                 t_expat,         
                 t_gama_local 
};  
enum Platforms { t_gnu, 
                 t_gnu_expat_1_1,
                 t_win32_borland, 
                 t_win32_msvc 
};  


const char* platform_pars[] = {

  "#CC       = gcc\n"
  "CFLAGS   += -pipe\n"
  "#CXX      = g++\n"
  "CXXFLAGS += -pipe -I../../..\n"
  "OBJ       = o\n"
  "LIBR      = @ar -r libgama.a\n"
  "RANLIB    = ranlib libgama.a\n"
  "LINK      = $(CXX) #g++\n"
  "LFLAGS   += -o\n"
  "LIBS     += -lexpat\n"
  "MAKE      = make\n"
  "LIBGAMA   = libgama.a\n"
  "P_GAMA_L  = gama-local\n"
  ,

  "#CC       = gcc\n"
  "CFLAGS   += -pipe\n"
  "#CXX      = g++\n"
  "CXXFLAGS += -pipe -DGNU_gama_expat_1_1 -I../../..\n"
  "OBJ       = o\n"
  "LIBR      = @ar -r libgama.a\n"
  "RANLIB    = ranlib libgama.a\n"
  "LINK      = $(CXX) #g++\n"
  "LFLAGS   += -o\n"
  "LIBS     += \n"
  "MAKE      = make\n"
  "LIBGAMA   = libgama.a\n"
  "P_GAMA_L  = gama-local\n"
  ,

  "CC        = bcc32\n"
  "CFLAGS    = -A -Od -w-8008 -w-8065 -w-8066 -w-8057\n"
  "CXX       = bcc32\n"
  "CXXFLAGS  = -DGNU_gama_expat_1_1 -I../../.. -w-8026 -w-8027 -w-8004 -tWR\n"
  "OBJ       = obj\n"
  "LIBR      = tlib /P4096 libgama.lib +\n"
  "RANLIB    = rem\n"
  "LINK      = bcc32\n"
  "LFLAGS    = -E\n"
  "LIBS      = ../lib/libgama.lib\n"
  "MAKE      = make\n"
  "LIBGAMA   = libgama.lib\n"
  "P_GAMA_L  = gama-local.exe\n"
  ,

  "CC        = cl\n"
  "CFLAGS    =\n"
  "CXX       = cl\n"
  "CXXFLAGS  = -DGNU_gama_expat_1_1 -I../../.. /nologo /W1 /GX /O2 /D \"WIN32\" \\\n"
  "            /D \"NDEBUG\" /D \"_MBCS\" /D \"_LIB\" /Zp1 /MT /GR\n"
  "OBJ       = obj\n"
  "LIBR      = rem link -lib libgama.lib\n"
  "RANLIB    = link -lib /out:libgama.lib *.obj\n"
  "LINK      = link\n"
  "LFLAGS    = /out:\n"
  "LIBS      = kernel32.lib user32.lib gdi32.lib winspool.lib  \\\n"
  "            comdlg32.lib advapi32.lib shell32.lib ole32.lib \\\n"
  "            oleaut32.lib uuid.lib odbc32.lib odbccp32.lib   \\\n"
  "            /nodefaultlib:libc /nologo /subsystem:console   \\\n"
  "            /incremental:no /machine:I386\n"
  "MAKE      = nmake\n"
  "LIBGAMA   = libgama.lib\n"
  "P_GAMA_L  = gama-local.exe\n"

};


using namespace std;

string path = "./";

void add_dep(string file, set<string>& dep)
{
  ifstream inp(file.c_str());
  if (!inp) 
    {
      file = path + file;
      inp.clear();
      inp.open(file.c_str());
      if (!inp)
        {
          cerr << "******  gnu_gama_dep : cannot open file " << file << endl;
          return;
        }
    }
  string line;
  while (getline(inp, line))
    {
      string::const_iterator b=line.begin(), e=line.end();
      
      while(b != e && isspace(*b)) ++b;
      
      if (b == e || *b != '#') continue;   ++b;
      
      while(b != e && isspace(*b)) ++b;
      
      if (b == e || *b != 'i') continue;   ++b;
      if (b == e || *b != 'n') continue;   ++b;
      if (b == e || *b != 'c') continue;   ++b;
      if (b == e || *b != 'l') continue;   ++b;
      if (b == e || *b != 'u') continue;   ++b;
      if (b == e || *b != 'd') continue;   ++b;
      if (b == e || *b != 'e') continue;   ++b;
      
      while(b != e && isspace(*b)) ++b;
      
      if (b == e || *b != '<') continue;   ++b;
      
      string name;
      while(b != e && *b != '>')
        {
          name.push_back(*b);
          ++b;
        }
      
      if (name.size() < 9) continue;
      if ((name[0] != 'g'  ||
           name[1] != 'a'  ||
           name[2] != 'm'  ||
           name[3] != 'a'  ||
           name[4] != 'l'  ||
           name[5] != 'i'  ||
           name[6] != 'b'  ||
           name[7] != '/') && 
          (name[0] != 'g'  ||
           name[1] != 'n'  ||
           name[2] != 'u'  ||
           name[3] != '_'  ||
           name[4] != 'g'  ||
           name[5] != 'a'  ||
           name[6] != 'm'  ||
           name[7] != 'a'  ||
           name[8] != '/') ) continue;

      if (dep.find(name) == dep.end())
        {
          dep.insert(name);
          add_dep(name, dep);
        }
    } 
}

int main(int argc, char* argv[])
{
  bool HELP = false;
  if (argc != 3) HELP = true;

  int project=0;
  if (argc == 3)
    {
      string arg1 = argv[1];
      if      (arg1 == "lib"       ) project = t_lib;
      else if (arg1 == "expat"     ) project = t_expat;
      else if (arg1 == "gama-local") project = t_gama_local;
      else
        HELP = true;
    }

  int platform=0;
  if (argc == 3)
    {
      string arg2 = argv[2];
      if      (arg2 == "gnu"          ) platform = t_gnu;
      else if (arg2 == "gnu-expat-1.1") platform = t_gnu_expat_1_1;
      else if (arg2 == "win32-borland") platform = t_win32_borland;
      else if (arg2 == "win32-msvc"   ) platform = t_win32_msvc;
      else
        HELP = true;
    }

  ifstream cin("scripts/gnu_gama_files");
  if (!cin)
    {
      cerr << "\n\ncannot open file 'scripts/gnu_gama_files'";
      HELP = true;
    }
  
  if (HELP)
    {
      cerr << "\n\nusage: " << argv[0] << " project platform  " << "\n\n"

           << "project     lib | expat | gama-local \n"
           << "platform    gnu | win32-borland | win32-msvc \n\n";
      return 1;
    }


  // ----------------------------------------------------------------------


  cout
    << "# This makefile was generated by gnu_gama_dep version " << version
    << "\n# for GNU Gama subproject '" << argv[1] 
    << "' for the " << argv[2] << " platform\n"
    << "#\n"
    << "# ------------------------------------------------------------------\n"
    << "#\n"
    << platform_pars[platform]
    << "#\n"
    << "# ------------------------------------------------------------------\n"
    <<  "#\n\n"
    << "SRC=../../../\n"
    << "OBJDIR=\n\n";


  // ----------------------------------------------------------------------


  if (project == t_gama_local)
    {
      cout <<
        "$(P_GAMA_L) : gama-local.$(OBJ) ../lib/$(LIBGAMA)\n"
        "\t$(LINK) $(LFLAGS)$(P_GAMA_L) gama-local.$(OBJ) "
        "$(LIBS) ../lib/$(LIBGAMA)\n\n"
        ;

      path = "gamaprog/linux/gama-local/";
      string file = "gama-local-main.h";

      set<string> dep;
      string name;
      for (string::const_iterator i=file.begin(); i!=file.end(); ++i)
        if     (*i == '/') 
          name.erase();
        else
          name += *i;

      add_dep(file, dep);

      cout << "gama-local.$(OBJ) : ../../linux/gama-local/" << name;
      for (set<string>::const_iterator i=dep.begin(); i!=dep.end(); ++i)
        cout << " $(SRC)" << *i;  
      cout << endl;
      
      cout << 
        "\t$(CXX) $(CXXFLAGS) -I. "
        "-c ../../linux/gama-local/gama-local.cpp\n\n";

      return 0;
    }


  // ----------------------------------------------------------------------


  if (project == t_expat)
    {
      string co  = "";

      if (platform == t_gnu) co = "### ";  // don't use expat 1.1

      cout << 
        "ALL : " << co << "$(LIBGAMA)\n\n"
        
        "$(OBJDIR)xmltok.$(OBJ) : $(SRC)expat/xmltok/xmltok.c\n"
	"\t$(CC) $(CFLAGS) -O2 -I../../../expat/xmltok "
        "-I../../../expat/xmlparse -DXML_NS -c $(SRC)expat/xmltok/xmltok.c\n"
	"\t$(LIBR) xmltok.$(OBJ)\n\n"
        
        "$(OBJDIR)xmlrole.$(OBJ) : $(SRC)expat/xmltok/xmlrole.c\n"
	"\t$(CC) $(CFLAGS) -O2 -I../../../expat/xmltok "
        "-I../../../expat/xmlparse -DXML_NS -c $(SRC)expat/xmltok/xmlrole.c\n"
	"\t$(LIBR) xmlrole.$(OBJ)\n\n"
        
        "$(OBJDIR)codepage.$(OBJ) : $(SRC)expat/xmlwf/codepage.c\n"
	"\t$(CC) $(CFLAGS) -O2 -I../../../expat/xmltok "
        "-I../../../expat/xmlparse -DXML_NS -c $(SRC)expat/xmlwf/codepage.c\n"
	"\t$(LIBR) codepage.$(OBJ)\n\n"
        
        "$(OBJDIR)xmlparse.$(OBJ) : $(SRC)expat/xmlparse/xmlparse.c\n"
        "\t$(CC) $(CFLAGS) -O2 -I../../../expat/xmltok "
        "-I../../../expat/xmlparse -DXML_NS -c "
        "$(SRC)expat/xmlparse/xmlparse.c\n"
	"\t$(LIBR) xmlparse.$(OBJ)\n\n"
        
        "$(OBJDIR)hashtable.$(OBJ) : $(SRC)expat/xmlparse/hashtable.c\n"
	"\t$(CC) $(CFLAGS) -O2 -I../../../expat/xmltok "
        "-I../../../expat/xmlparse -DXML_NS -c "
        "$(SRC)expat/xmlparse/hashtable.c\n"
	"\t$(LIBR) hashtable.$(OBJ)\n\n"
        
        "$(LIBGAMA) : xmltok.$(OBJ) xmlrole.$(OBJ) codepage.$(OBJ) "
        "xmlparse.$(OBJ) hashtable.$(OBJ)\n"
	"\t$(RANLIB)\n\n"
        ;
      
      return 0;
    }


  // ----------------------------------------------------------------------


  cout << "ALL : $(LIBGAMA)\n\n";

  string all_objects;

  path = "./";

  string file, line;
  while(getline(cin, file))
    {
      if (file[0] == '#') continue;

      set<string> dep;
      string name;
      for (string::const_iterator i=file.begin(); i!=file.end(); ++i)
        if     (*i == '/') 
          name.erase();
        else
          name += *i;

      add_dep(file, dep);
 
      string object_file;
      for (string::const_iterator i=name.begin(), e=name.end(); i!=e; ++i)
        {
          if (*i == '.') break;

          object_file += *i;
        }
      object_file += ".$(OBJ)";
      all_objects += object_file;
      all_objects += ' ';

      cout << object_file << " : " << "$(SRC)" << file;
      for (set<string>::const_iterator i=dep.begin(); i!=dep.end(); ++i)
        cout << " $(SRC)" << *i;
      cout << "\n";

      cout << "\t$(CXX) $(CXXFLAGS) -c $(SRC)" << file << "\n";
      cout << "\t@$(LIBR) " << object_file << "\n\n";
    }

  cout << "$(LIBGAMA) : " << all_objects << "\n"
       << "\t$(RANLIB)\n\n";
    
}











