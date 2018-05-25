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
    void addVertex(const ngl::Vec3 &_v);
    void addNormal(const ngl::Vec3 &_v);
    void addUV(const ngl::Vec2 &_v);
    void addUV(const ngl::Vec3 &_v);
    void addFace(const ngl::Face &_f);


    virtual bool load(const std::string_view &_fname, CalcBB _calcBB=CalcBB::True ) noexcept;
    bool save(const std::string_view &_fname);
    bool isLoaded() const {return m_isLoaded;}
  private :
    bool parseVertex(std::vector<std::string> &_tokens);
    bool parseNormal(std::vector<std::string> &_tokens);
    bool parseUV(std::vector<std::string> &_tokens);
    // face parsing is complex we have different layouts.
    // don't forget we can also have negative indices
    bool parseFace(std::vector<std::string> &_tokens);
    // f v v v v
    bool parseFaceVertex(std::vector<std::string> &_tokens);
    // f v//vn v//vn v//vn v//vn
    bool parseFaceVertexNormal(std::vector<std::string> &_tokens);
    // f v/vt v/vt v/vt v/vt
    bool parseFaceVertexUV(std::vector<std::string> &_tokens);
    // f v/vt/vn v/vt/vn v/vt/vn v/vt/vn
    bool parseFaceVertexNormalUV(std::vector<std::string> &_tokens);

    std::vector<std::string> m_fileContents;
    bool m_isLoaded=false;
    // as faces can use negative index values keep track of index
    size_t m_currentVertexOffset=0;
    size_t m_currentNormalOffset=0;
    size_t m_currentUVOffset=0;
};

#endif
