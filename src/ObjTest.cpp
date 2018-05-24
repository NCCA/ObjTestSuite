#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include "Obj.h"

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

std::array<const char *,5> validfiles={
  {
   "files/Triangle1.obj",
   "files/TriangleNoNormal.obj",
   "files/TriangleVertsOnly.obj",
   "files/TriangleVertsUV.obj",
   "files/Triangle3UV.obj"
  }
};


std::array<const char *,1> invalidfiles={
  {
    "files/BrokenFloats.obj"
  }
};

TEST(ObjFilesPresent,openForReading)
{
  for(auto s : validfiles)
  {
    std::ifstream f(s);
    EXPECT_TRUE(f.good());
  }
  for(auto s : invalidfiles)
  {
    std::ifstream f(s);
    EXPECT_TRUE(f.good());
  }

}

TEST(ObjAbstractMesh,defaultCtor)
{
  Obj a;
  EXPECT_EQ(a.getNumFaces() , 0);
  EXPECT_EQ(a.getNumNormals() , 0);
  EXPECT_EQ(a.getNumVerts() , 0);
  EXPECT_EQ(a.getNumTexCords(),0);
}

TEST(Obj,loadvalid)
{
  Obj a;
  for(auto f : validfiles)
  {
    EXPECT_TRUE(a.load(f));
    EXPECT_TRUE(a.isLoaded());
  }
}
TEST(Obj,loadinvalid)
{
  Obj a;
  for(auto f : invalidfiles)
  {
    EXPECT_FALSE(a.load(f));
    EXPECT_FALSE(a.isLoaded());
  }

}

TEST(Obj,loadFail)
{
  Obj a;
  EXPECT_FALSE(a.load("nothing"));
  EXPECT_FALSE(a.isLoaded());
}

TEST(ObjInternal,parseVertex)
{
  Obj a;
  EXPECT_TRUE(a.load("files/Triangle1.obj"));
  EXPECT_EQ(a.getNumVerts(),3);
}

TEST(ObjInternal,parseNormal)
{
  Obj a;
  EXPECT_TRUE(a.load("files/Triangle1.obj"));
  EXPECT_EQ(a.getNumVerts(),3);
}

TEST(ObjInternal,parseUV)
{
  Obj a;
  EXPECT_TRUE(a.load("files/Triangle1.obj"));
  EXPECT_EQ(a.getNumTexCords(),3);
}

TEST(ObjInternal,parseUV3)
{
  Obj a;
  EXPECT_TRUE(a.load("files/Triangle3UV.obj"));
  EXPECT_EQ(a.getNumTexCords(),3);
}


