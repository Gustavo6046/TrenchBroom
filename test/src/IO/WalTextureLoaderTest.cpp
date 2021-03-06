/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "Assets/AssetTypes.h"
#include "Assets/Texture.h"
#include "Assets/TextureCollection.h"
#include "Assets/TextureCollectionSpec.h"
#include "Assets/Palette.h"
#include "IO/DiskFileSystem.h"
#include "IO/Path.h"
#include "IO/WalTextureLoader.h"

namespace TrenchBroom {
    namespace IO {
        inline void assertTexture(const String& name, const size_t width, const size_t height, Assets::Texture* texture) {
            ASSERT_EQ(name, texture->name());
            ASSERT_EQ(width, texture->width());
            ASSERT_EQ(height, texture->height());
        }
        
        TEST(WalTextureLoaderTest, testLoadWalDir) {
            DiskFileSystem fs(IO::Disk::getCurrentWorkingDir());
            
            const Assets::Palette palette(Path("data/colormap.pcx"));
            WalTextureLoader loader(fs, palette);
            
            const Assets::TextureCollectionSpec spec("rtz", IO::Path("data/IO/Wal/rtz"));
            Assets::TextureCollection* collection = loader.loadTextureCollection(spec);
            ASSERT_TRUE(collection->loaded());
            
            const Assets::TextureList& textures = collection->textures();
            ASSERT_EQ(7u, textures.size());
            assertTexture("rtz/b_pv_v1a1",  128, 256, textures[0]);
            assertTexture("rtz/b_pv_v1a2",  128, 256, textures[1]);
            assertTexture("rtz/b_pv_v1a3",  128, 128, textures[2]);
            assertTexture("rtz/b_rc_v16",   128, 128, textures[3]);
            assertTexture("rtz/b_rc_v16w",  128, 128, textures[4]);
            assertTexture("rtz/b_rc_v28",   128, 64,  textures[5]);
            assertTexture("rtz/b_rc_v4",    128, 128, textures[6]);
            delete collection;
        }
    }
}
