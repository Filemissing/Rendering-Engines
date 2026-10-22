//
// Created by micha on 24/11/2025.
//

#ifndef RAWENGINE_ASSETLIBRARY_H
#define RAWENGINE_ASSETLIBRARY_H
#include <random>
#include <unordered_map>

namespace core {
    class Asset;

    class AssetLibrary {
    public:
        static void Initialize();
        static Asset* GetAssetByUUID(int uuid);
        static void Register(Asset* asset);
        static std::uint64_t GenerateUUID();
    };
} // core

#endif //RAWENGINE_ASSETLIBRARY_H