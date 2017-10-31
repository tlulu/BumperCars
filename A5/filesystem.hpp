#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <cstdlib>

class FileSystem
{
private:
  typedef std::string (*Builder) (const std::string& path);

public:
  static std::string getPath(const std::string& path)
  {
    static std::string(*pathBuilder)(std::string const &) = getPathBuilder();
    return (*pathBuilder)(path);
  }

private:
  //static std::string(*foo (std::string const &)) getPathBuilder()
  static Builder getPathBuilder()
  {
    return &FileSystem::getPathRelativeRoot;
  }

  static std::string getPathRelativeRoot(const std::string& path)
  {
    return std::string("../") + path;
  }

  static std::string getPathRelativeBinary(const std::string& path)
  {
    return "../../../" + path;
  }


};

// FILESYSTEM_H
#endif