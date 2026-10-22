//
// Created by micha on 24/11/2025.
//

#include "Asset.h"

namespace core {
    Asset::Asset() {
        AssetLibrary::Register(this);
    }
} // core