#include "Obj.h"
#include "pystring.h"
#include <string>
#include <cmath>
namespace ps=pystring;

Obj::Obj(const std::string& _fname  , CalcBB _calcBB)  noexcept :AbstractMesh()
{
  load(_fname,_calcBB);
}

void Obj::addVertex(const ngl::Vec3 &_v)
{
  m_verts.push_back(_v);
}
void Obj::addNormal(const ngl::Vec3 &_v)
{
  m_norm.push_back(_v);
}
void Obj::addUV(const ngl::Vec2 &_v)
{
  ngl::Vec3 v(_v.m_x,_v.m_y,0.0f);
  m_uv.push_back(v);
}
void Obj::addUV(const ngl::Vec3 &_v)
{
  m_uv.push_back(_v);
}

void Obj::addFace(const ngl::Face &_f)
{
  m_face.push_back(_f);
}


bool Obj::save(const std::string_view &_fname)
{
  std::ofstream out(_fname.data());
  if (out.is_open() != true)
  {
    std::cout<<"ERROR could not open file for writing "<<_fname.data()<<"\n";
    return false;
  }

  // write out some comments
  out<<"# This file was created by ngl Obj exporter "<<_fname.data()<<'\n';
  // was c++ 11  for(Vec3 v : m_norm) for all of these
  // write out the verts
  for(auto v : m_verts)
  {
    out<<"v "<<v.m_x<<" "<<v.m_y<<" "<<v.m_z<<'\n';
  }

  // write out the tex cords
  for(auto v : m_uv)
  {
    out<<"vt "<<v.m_x<<" "<<v.m_y<<'\n';
  }
  // write out the normals

  for(auto v : m_norm)
  {
    out<<"vn "<<v.m_x<<" "<<v.m_y<<" "<<v.m_z<<'\n';
  }

  // finally the faces
  for(auto f : m_face)
  {
  out<<"f ";
  // we now have V/T/N for each to write out
  for(unsigned int i=0; i<f.m_vert.size(); ++i)
  {
    // don't forget that obj indices start from 1 not 0 (i did originally !)
    out<<f.m_vert[i]+1;
    if(m_uv.size() !=0)
    {
    out<<'/';
    out<<f.m_uv[i]+1;
    }
    if(m_norm.size() !=0)
    {
    out<<'/';
      if(m_uv.size()==0)
        out<<'/';
    out<<f.m_norm[i]+1;
    out<<" ";
    }
    out<<' ';
  }
  out<<'\n';
  }
  return true;
}




bool Obj::load(const std::string_view & _fname, CalcBB _calcBB ) noexcept
{
  std::ifstream in(_fname.data());
  if (in.is_open() != true)
  {
    std::cout<<"ERROR .obj file not found  "<<_fname.data()<<"\n";
    return false;
  }

  std::string str;
  // Read the next line from File untill it reaches the end.
  while (std::getline(in, str))
  {
    bool status=true;
  // Line contains string of length > 0 then parse it
    if(str.size() > 0)
    {
      std::vector<std::string> tokens;
      ps::split(str,tokens);
      if( tokens[0] == "v" )
      {
        status=parseVertex(tokens) ;
      }
      else if (tokens[0] == "vn" )
      {
        status=parseNormal(tokens);
      }

      else if(tokens[0] == "vt" )
      {
        status=parseUV(tokens);
      }
      else if(tokens[0]== "f" )
      {
        status=parseFace(tokens);
      }
    } // str.size()
    // early out sanity checks!
    if(status == false)
      return false;
  } // while

  in.close();
  // Calculate the center of the object.
  if(_calcBB == CalcBB::True)
  {
    this->calcDimensions();
  }
  m_isLoaded=true;
  return true;
}


bool Obj::parseVertex(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  try
  {
    float x=std::stof(_tokens[1]);
    float y=std::stof(_tokens[2]);
    float z=std::stof(_tokens[3]);
    m_verts.push_back({x,y,z});
    ++m_currentVertexOffset;
  }
  catch (std::invalid_argument)
  {
    std::cerr<<"error converting Obj file vertex\n";
    parsedOK=false;
  }
  return parsedOK;
}

bool Obj::parseNormal(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  try
  {
    float x=std::stof(_tokens[1]);
    float y=std::stof(_tokens[2]);
    float z=std::stof(_tokens[3]);
    m_norm.push_back({x,y,z});
    ++m_currentNormalOffset;
  }
  catch (std::invalid_argument)
  {
    std::cerr<<"error converting Obj file normals\n";
    parsedOK=false;
  }
  return parsedOK;
}


bool Obj::parseUV(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  try
  {
    float x=std::stof(_tokens[1]);
    float y=std::stof(_tokens[2]);
    float z=std::stof(_tokens[3]);
    m_uv.push_back({x,y,z});
    ++m_currentUVOffset;
  }
  catch (std::invalid_argument)
  {
    std::cerr<<"error converting Obj file UV's\n";
    parsedOK=false;
  }
  return parsedOK;
}


bool Obj::parseFace(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  // first let's find what sort of face we are dealing with
  // I assume most likely case is all
  if( ps::count(_tokens[1],"/") == 2 && ps::find(_tokens[1],"//") ==-1)
  {
    parsedOK=parseFaceVertexNormalUV(_tokens);
  }

  else if(ps::find(_tokens[1],"/") == -1)
  {
    parsedOK=parseFaceVertex(_tokens);
  }
  // look for VertNormal
  else if( ps::find(_tokens[1],"//")!= -1)
  {
    parsedOK=parseFaceVertexNormal(_tokens);
  }
  // if we have 1 / it is a VertUV format
  else if( ps::count(_tokens[1],"/") == 1)
  {
    parsedOK=parseFaceVertexUV(_tokens);
  }

  return parsedOK;

}
// f v v v v
bool Obj::parseFaceVertex(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  ngl::Face f;
  // token still starts with f so skip
  for( size_t i=1; i<_tokens.size(); ++i)
  {
    try
    {
      // note we need to subtract one from the list
      int idx=std::stoi(_tokens[i])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentVertexOffset+(idx+1);
      }
      f.m_vert.push_back(static_cast<uint32_t>(idx));
    }
    catch (std::invalid_argument)
    {
      std::cerr<<"error converting Obj file face\n";
      parsedOK=false;
    }
  }
  m_face.push_back(f);
  return parsedOK;

}
// f v//vn v//vn v//vn v//vn
bool Obj::parseFaceVertexNormal(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  ngl::Face f;
  // token still starts with f so skip
  for( size_t i=1; i<_tokens.size(); ++i)
  {
    std::vector <std::string> vn;
    ps::split(_tokens[i],vn,"//");
    try
    {
      // note we need to subtract one from the list
      int idx=std::stoi(vn[0])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentVertexOffset+(idx+1);
      }
      f.m_vert.push_back(static_cast<uint32_t>(idx));
      idx=std::stoi(vn[1])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentNormalOffset+(idx+1);
      }
      f.m_norm.push_back(static_cast<uint32_t>(idx));

    }
    catch (std::invalid_argument)
    {
      std::cerr<<"error converting Obj file face\n";
      parsedOK=false;
    }
  }
  m_face.push_back(f);
  return parsedOK;

}
// f v/vt v/vt v/vt v/vt
bool Obj::parseFaceVertexUV(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  ngl::Face f;
  // token still starts with f so skip
  for( size_t i=1; i<_tokens.size(); ++i)
  {
    std::vector <std::string> vn;
    ps::split(_tokens[i],vn,"/");
    try
    {
      // note we need to subtract one from the list
      int idx=std::stoi(vn[0])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentVertexOffset+(idx+1);
      }
      f.m_vert.push_back(static_cast<uint32_t>(idx));
      idx=std::stoi(vn[1])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentUVOffset+(idx+1);
      }
      f.m_uv.push_back(static_cast<uint32_t>(idx));

    }
    catch (std::invalid_argument)
    {
      std::cerr<<"error converting Obj file face\n";
      parsedOK=false;
    }
  }
  m_face.push_back(f);
  return parsedOK;
}
// f v/vt/vn v/vt/vn v/vt/vn v/vt/vn
bool Obj::parseFaceVertexNormalUV(std::vector<std::string> &_tokens)
{
  bool parsedOK=true;
  ngl::Face f;
  // token still starts with f so skip
  for( size_t i=1; i<_tokens.size(); ++i)
  {
    std::vector <std::string> vn;
    ps::split(_tokens[i],vn,"/");
    try
    {
      // note we need to subtract one from the list
      int idx=std::stoi(vn[0])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentVertexOffset+(idx+1);
      }
      f.m_vert.push_back(static_cast<uint32_t>(idx));

      idx=std::stoi(vn[1])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentUVOffset+(idx+1);
      }
      f.m_uv.push_back(static_cast<uint32_t>(idx));


      idx=std::stoi(vn[2])-1;
      // check if we are a negative index
      if(std::signbit(idx))
      {
        // note we index from 0 not 1 like obj so adjust
        idx=m_currentNormalOffset+(idx+1);
      }
      f.m_norm.push_back(static_cast<uint32_t>(idx));
    }
    catch (std::invalid_argument)
    {
      std::cerr<<"error converting Obj file face\n";
      parsedOK=false;
    }
  }
  m_face.push_back(f);
  return parsedOK;
}

