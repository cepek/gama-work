/* Ellipsoids_xml is a simple program to transform ellipsoids.xml
 * input file into ellipsoids.[h|cpp|html] output.
 * ==========================================================================
 * 
 * $Id: ellipsoids_xml.cpp,v 1.6 2002/06/28 09:01:40 cepek Exp $
 *
 * ------------------------------------------------------------------------ */

const char* version = "0.02";

/* ---------------------------------------------------------------------------
 *
 * 0.02  2002-06-28
 *
 *       - missing `#include <cstring>' in generated code (Jan Pytel)
 *         
 * 0.01  2002-06-09  
 *       
 *       - initial draft
 *
 * ---------------------------------------------------------------------------
 *   
 *  GNU GaMa -- Adjustment of geodetic networks
 *  Copyright (C) 2002  Ales Cepek <cepek@fsv.cvut.cz>
 *
 *  This file is part of the GNU GaMa.
 *  
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ---------------------------------------------------------------------------
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <list>
#include <expat/xmlparse/xmlparse.h>
#include <gamalib/xml/encoding.h>

using namespace std;
using namespace GaMaLib;

// ---------------------------------------------------------------------------

enum parser_state { START, REFERENCES, ITEM, ELLIPSOIDS, ELLIPSOID, END };
int  result = 0;

class Parser {

public:

  struct Entry 
  {
    string id, caption, a, b, f, f1, ref;
  };
  parser_state state;
  string       revision;

  Parser(const char* filename);
  ~Parser();

  void error   (const char*);
  void add     (Entry e)        { elist.push_back(e); }
  void xml2h   (ostream&);
  void xml2cpp (ostream&);
  void xml2html(ostream&);
  void addlabel(string s)       { label.push_back(s); }
  void additem (string s)       { item .push_back(s); }

  string       item_data;
  
private:

  list<Entry>  elist;
  list<string> label;
  list<string> item;

  XML_Parser expat_parser;
  string     filename;
  string     errString;
  int        errCode;
  int        errLine;
  string     text;
  
};



void startElement(void *userData, const char *cname, const char **atts)
{
  Parser* parser = static_cast<Parser*>(userData);
  const string name(cname);

  if (parser->state == START && name == "ellipsoids") 
    {
      parser->state = ELLIPSOIDS;

      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);
          
          if (nam == "revision")  parser->revision = val;
          else
            {
              parser->error("unknown attribute");
            }
        }
    }
  else if (parser->state == ELLIPSOIDS && name == "references") 
    {
      parser->state = REFERENCES;

      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);
          
          {
            parser->error("unknown attribute");
          }
        }
    }
  else if (parser->state == REFERENCES && name == "item") 
    {
      parser->state = ITEM;
      parser->item_data.erase();

      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);
          
          if (nam == "label")
            {
              parser->addlabel(val);
            }
          else
            {
              parser->error("unknown attribute");
            }
        }
    }
  else if (parser->state == ELLIPSOIDS && name == "ellipsoid") 
    {
      parser->state = ELLIPSOID;
      
      string id;
      Parser::Entry  entry;
      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);
          
          if      (nam == "id")      entry.id = val;
          else if (nam == "caption") entry.caption = val;
          else if (nam == "a")       entry.a = val;
          else if (nam == "b")       entry.b = val;
          else if (nam == "f")       entry.f = val;
          else if (nam == "f1")      entry.f1 = val;
          else if (nam == "ref")     entry.ref = val;
          else
            {
              parser->error("unknown attribute");
            }
        }
      parser->add(entry);
    }
  else
    parser->error("unknown tag");
}

void endElement(void *userData, const char * /*cname*/)
{
  Parser* parser = static_cast<Parser*>(userData);

  switch (parser->state)
    {
    case ELLIPSOIDS: parser->state = END;        break;
    case REFERENCES: parser->state = ELLIPSOIDS; break;
    case ITEM      : parser->state = REFERENCES; 
                     parser->additem(parser->item_data);
                                                 break;
    case ELLIPSOID : parser->state = ELLIPSOIDS; break;
    }
}

void characterDataHandler(void *userData, const char* s, int len)
{
  Parser* parser = static_cast<Parser*>(userData);

  if (parser->state == ITEM)
    {
      parser->item_data += string(s, len);
    }
  else
    for (int b=0; b <len; b++)
      if (!isspace(s[b]))
        {
          parser->error("ignored junk");
          return;
        }
}



Parser::Parser(const char* fn)
{
  expat_parser = XML_ParserCreate(0); 
  
  XML_SetUserData(expat_parser, this);
  XML_SetElementHandler(expat_parser, startElement, endElement);
  XML_SetCharacterDataHandler(expat_parser, characterDataHandler);
  XML_SetUnknownEncodingHandler(expat_parser, UnknownEncodingHandler, 0);
  
  filename = string(fn);
  state = START;
  
  ifstream inp(fn);
  char   c;
  while (getline(inp, text))
    {
      text += "\n";
      int err = XML_Parse(expat_parser, text.c_str(), text.length(), false);
      if (err == 0)
        {
          errString = string(XML_ErrorString(XML_GetErrorCode(expat_parser)));
          errCode   = XML_GetErrorCode(expat_parser);
          errLine   = XML_GetCurrentLineNumber(expat_parser);
          
          cerr << filename << ":" << errLine << ": "
               << errString << " : " << text << endl;
          return;
        }
    }
  XML_Parse(expat_parser, "", 0, true);
}

Parser::~Parser()
{
  XML_ParserFree(expat_parser); 
}

void Parser::error(const char* message)
{
  int erl = XML_GetCurrentLineNumber(expat_parser);
  cerr << filename << ":" << erl << ": " << message << ": " << text << endl; 
  result = 1;
}

void Parser::xml2h(ostream& out)
{
  out << "#ifndef GaMaLib___gama_ellipsoids__header_file_h\n"
      << "#define GaMaLib___gama_ellipsoids__header_file_h\n\n"
      << "#include <gamalib/ellipsoid.h>\n\n"
      << "// This file was generated by GNU GaMa (program ellipsoids_xml" 
      << /* version << */ ") from\n"
      << "// http://www.gnu.org/software/gama/xml/ellipsoids.xml"
      << " revision " << revision << "\n\n"
      << "namespace GaMaLib {\n\n"
      << "enum gama_ellipsoid {\n\n"
      << "ellipsoid_unknown,\n";
  
  for (list<Entry>::iterator i=elist.begin(); i!=elist.end();)
    {
      Entry e = *i;
      out << "ellipsoid_" << e.id;
      if (++i != elist.end()) out << ",";
      out << "\t\t// " << e.a << "   " << e.b+e.f+e.f1 << "\n";
    }
  
  out << "\n};\n\n"
      << "extern const char * const gama_ellipsoid_caption[];\n\n"
      << "gama_ellipsoid ellipsoid(const char*);\n"
      << "int            set(Ellipsoid*, gama_ellipsoid);\n"
      << "\n}\n\n"
      << "#endif\n";
}

void Parser::xml2cpp(ostream& out)
{
  out << "#include <gamalib/ellipsoids.h>\n"
      << "#include <cstring>\n\n"
      << "// This file was generated by GNU GaMa (program ellipsoids_xml" 
      << /* version << */ ") from\n"
      << "// http://www.gnu.org/software/gama/xml/ellipsoids.xml"
      << " revision " << revision << "\n\n"
      << "namespace GaMaLib {\n\n";
  out << "const char * const gama_ellipsoid_caption[] = { \"\",\n";
  {
    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); )
      {
        Entry e = *i;
        out << "   \"" << e.caption << "\"";
        if (++i != elist.end()) out << ",";
        out << "\n";
      }
  }
  out << "};\n\n";
  
  out << "int set(Ellipsoid* E, gama_ellipsoid T)\n"
      << "{\n"
      << "   switch(T) {\n";
  {
    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
      {
        Entry e = *i;
        out << "   case ellipsoid_" << e.id  << " :\n";
        out << "      E->set_a";
        if (!e.b.empty()) 
          out << "b( " << e.a << ", " << e.b  << " );\n";
        else if (!e.f.empty()) 
          out << "f( " << e.a << ", " << e.f  << " );\n";
        else if (!e.f1.empty()) 
          out << "f1( " << e.a << ", " << e.f1 << " );\n";
        out << "      break;\n";
      }
  }    
  out << "   default :\n      return 1;\n   }\n\n   return 0;\n}\n\n";
  
  out << "gama_ellipsoid ellipsoid(const char* s)\n"
      << "{\n"
      << "   using namespace std;\n\n"
      << "   gama_ellipsoid T = ellipsoid_unknown;\n\n";
  bool first = true;
  for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
    {
      Entry e = *i;
      out << "   ";
      if (!first) out << "else ";
      out << "if ";
      if (first)
        {
          out << "     ";
          first = false;
        }
      out << "(!strcmp(\""<< e.id << "\", s))"
          << "  T = ellipsoid_" << e.id << ";\n";
    }
  out << "\n   return T;\n}\n\n}      // namespace GaMaLib\n";
}

void Parser::xml2html(ostream& out)
{
    out <<
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      "<!DOCTYPE html\n"
      "     PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
      "     \"DTD/xhtml1-strict.dtd\">\n"
      "<html xmlns=\"http://www.w3.org/1999/xhtml\""
      " xml:lang=\"en\" lang=\"en\">\n"
      "  <head>\n"
      "    <title>GNU GaMa ellipsoids</title>\n"
      "  </head>\n"
      "<body>\n\n"
      "<h1><a href=\"http://www.gnu.org/software/gama/xml/ellipsoids.xml\">"
      "GNU GaMa ellipsoids</a></h1>\n\n"
      "<h2>revision " << revision << "</h2>\n\n"
      "<table border=\"1\">\n\n";

    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
      {
        Entry e = *i;

        out << "<tr>\n";
        out << "<td>" << e.id         << "</td>\n";
        out << "<td>" << e.a          << "</td>\n";
        out << "<td>" << e.b+e.f+e.f1 << "</td>\n";
        out << "<td>" << e.caption    << "</td>\n";
        out << "<td>" << e.ref        << "</td>\n";
        out << "</tr>\n";  
      }

    out << "\n</table>\n\n";

    out << "<h2>References</h2>\n\n";

    out << "<dl>\n";
    list<string>::iterator li=label.begin(), ti=item.begin();
    while (li != label.end() && ti != item.end())
      {
        out << "<dt><strong>" << *li << "</strong></dt>";
        out << "<dd>" << *ti << "</dd>\n\n";
        ++li, ++ti;
      }
    out << "</dl>\n\n";

    out << "</body>\n</html>\n";
}

int main(int argc, char* argv[])
{
  if (argc != 3 ||
      argc == 3 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help") ||
                    !strcmp(argv[2],"-h") || !strcmp(argv[2],"--help")) )
    {
      cout << "\nusage: ellipsoids_xml  input.xml  output_directory\n\n";
      return 1;
    }

  Parser parser(argv[1]);

  string path(argv[2]);
  if (*path.rbegin() != '/') path = path + '/';

  {
    string file = path + "ellipsoids.h";
    ofstream out(file.c_str());

    parser.xml2h(out);
  }
  {
    string file = path + "ellipsoids.cpp";
    ofstream out(file.c_str());

    parser.xml2cpp(out);
  }
  {
    string file = path + "ellipsoids.html";
    ofstream out(file.c_str());

    parser.xml2html(out);
  }

  return result;
}