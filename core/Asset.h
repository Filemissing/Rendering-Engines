//
// Created by micha on 24/11/2025.
//

#ifndef RAWENGINE_ASSET_H
#define RAWENGINE_ASSET_H

#include <cstdint>
#include "AssetLibrary.h"

namespace core {
    class Asset {
    public:
        std::uint64_t uuid = AssetLibrary::GenerateUUID();

        Asset();
    };
} // core

#endif //RAWENGINE_ASSET_H