/*
 Copyright (C) 2010-2013 Kristian Duske
 
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

#include "WadTextureLoader.h"

#include "ByteBuffer.h"
#include "Color.h"
#include "Exceptions.h"
#include "Renderer/GL.h"
#include "IO/Wad.h"
#include "Assets/Palette.h"
#include "Assets/Texture.h"
#include "Assets/TextureCollection.h"

namespace TrenchBroom {
    namespace IO {
        WadTextureLoader::WadTextureLoader(const Assets::Palette& palette) :
        m_palette(palette) {}

        Assets::TextureCollection* WadTextureLoader::doLoadTextureCollection(const Path& path) {
            Wad wad(path);
            const WadEntryList mipEntries = wad.entriesWithType(WadEntryType::WEMip);
            const size_t textureCount = mipEntries.size();

            Assets::TextureList textures;
            textures.reserve(textureCount);

            Color tempColor, averageColor;

            for (size_t i = 0; i < textureCount; ++i) {
                const WadEntry& entry = mipEntries[i];
                const MipSize mipSize = wad.mipSize(entry);

                Assets::TextureBuffer::List buffers(4);
                Assets::setMipBufferSize(buffers, mipSize.width, mipSize.height);

                for (size_t j = 0; j < 4; ++j) {
                    const MipData mipData = wad.mipData(entry, j);
                    m_palette.indexedToRgb(mipData.begin, mipData.end - mipData.begin, buffers[j], tempColor);
                    if (j == 0)
                        averageColor = tempColor;
                }

                textures.push_back(new Assets::Texture(entry.name(), mipSize.width, mipSize.height, averageColor, buffers));
            }
            
            return new Assets::TextureCollection(path.lastComponent().asString(), textures);
        }
    }
}