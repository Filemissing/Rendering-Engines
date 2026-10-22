//
// Created by micha on 24/11/2025.
//

#include "AssetLibrary.h"

#include <filesystem>
#include <fstream>

#include "Asset.h"

namespace core {
    static std::unordered_map<std::uint64_t, Asset*>& AssetMap() {
        static std::unordered_map<std::uint64_t, Asset*> instance;
        return instance;
    }

    void AssetLibrary::Initialize() {
        for (const auto& file : std::filesystem::recursive_directory_iterator("Assets")) {

        }
    }

    Asset* AssetLibrary::GetAssetByUUID(int uuid) {
        return AssetMap().find(uuid)->second;
    }

    void AssetLibrary::Register(Asset* asset) {
        AssetMap().insert_or_assign(asset->uuid, asset);
    }

    uint64_t AssetLibrary::GenerateUUID() {
        static std::mt19937_64 rng(std::random_device{}());
        static std::uniform_int_distribution<uint64_t> dist;
        return dist(rng);
    }
} // core