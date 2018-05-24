#include "Obj.h"
#include "pystring.h"

namespace ps=pystring;

bool Obj::load(const std::string_view & _fname, CalcBB _calcBB ) noexcept
{
  std::ifstream in(_fname.data());
  if (in.is_open() != true)
  {
    std::cout<<"FILE NOT FOUND !!!! "<<_fname.data()<<"\n";
    return false;
  }

  std::string str;
  // Read the next line from File untill it reaches the end.
  while (std::getline(in, str))
  {
  // Line contains string of length > 0 then save it in vector
    if(str.size() > 0)
    {
      m_fileContents.push_back(str);
    }
  }
  in.close();
  if (parseVertex() ==false)
  {
    return false;
  }

  if (parseNormal() ==false)
  {
    return false;
  }

  if(parseUV()==false)
  {
    return false;
  }

  m_isLoaded=true;
  return true;
}


bool Obj::parseVertex()
{
  bool parsedOK=true;
  for(auto line : m_fileContents)
  {
    line=ps::lstrip(line);
    std::vector<std::string> tokens;
    ps::split(line,tokens," ");
    if(tokens.size() !=0)
    {
      if( tokens[0] == "v" )
      {
        try
        {
          float x=std::stof(tokens[1]);
          float y=std::stof(tokens[2]);
          float z=std::stof(tokens[3]);
          m_verts.push_back({x,y,z});
          ++m_currentVertexOffset;

        } catch (std::invalid_argument)
        {
          std::cerr<<"error converting Obj file parameter\n" <<
                     line<<'\n';
          parsedOK=false;
        }
      }
    }

  }
  m_nVerts=m_verts.size();

  return parsedOK;
}

bool Obj::parseNormal()
{
  bool parsedOK=true;
  for(auto line : m_fileContents)
  {
    line=ps::lstrip(line);
    std::vector<std::string> tokens;
    ps::split(line,tokens," ");
    if(tokens.size() !=0)
    {
      if( tokens[0] == "vn" )
      {
        try
        {
          float x=std::stof(tokens[1]);
          float y=std::stof(tokens[2]);
          float z=std::stof(tokens[3]);
          m_norm.push_back({x,y,z});
          ++m_currentNormalOffset;

        } catch (std::invalid_argument)
        {
          std::cerr<<"error converting Obj file parameter\n" <<
                     line<<'\n';
          parsedOK=false;
        }
      }
    }

  }
  m_nNorm=m_norm.size();

  return parsedOK;

}

bool Obj::parseUV()
{
  bool parsedOK=true;
  for(auto line : m_fileContents)
  {
    line=ps::lstrip(line);
    std::vector<std::string> tokens;
    ps::split(line,tokens," ");
    if(tokens.size() !=0)
    {
      if( tokens[0] == "vt" )
      {
        try
        {
          float x=std::stof(tokens[1]);
          float y=std::stof(tokens[2]);
          float z=0;
          if(tokens.size()==4)
            z=std::stof(tokens[3]);

          m_tex.push_back({x,y,z});
          ++m_currentUVOffset;

        } catch (std::invalid_argument)
        {
          std::cerr<<"error converting Obj file parameter\n" <<
                     line<<'\n';
          parsedOK=false;
        }
      }
    }

  }
  m_nTex=m_tex.size();

  return parsedOK;

}
