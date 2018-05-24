#ifndef OBJ_H_
#define OBJ_H_
#include <vector>
#include <string>

#include <ngl/AbstractMesh.h>

class Obj : public ngl::AbstractMesh
{
  public :
    Obj()  noexcept: ngl::AbstractMesh(){}
    Obj(const std::string& _fname  , CalcBB _clacBB=CalcBB::True)  noexcept ;

    virtual bool load(const std::string_view &_fname, CalcBB _calcBB=CalcBB::True ) noexcept;
    bool isLoaded() const {return m_isLoaded;}
  private :
    bool parseVertex();
    bool parseNormal();
    bool parseUV();
    std::vector<std::string> m_fileContents;
    bool m_isLoaded=false;
    // as faces can use negative index values keep track of index
    size_t m_currentVertexOffset=0;
    size_t m_currentNormalOffset=0;
    size_t m_currentUVOffset=0;
};

#endif
