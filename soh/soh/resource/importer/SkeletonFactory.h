#pragma once

#include "Resource.h"
#include "ResourceFactory.h"

namespace Ship {
class SkeletonFactory : public ResourceFactory
{
  public:
    std::shared_ptr<Resource> ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader);
};

class SkeletonFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
