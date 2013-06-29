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
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Texture.h"

namespace TrenchBroom {
    namespace Model {
        TexturePtr Texture::newTexture(const GLuint textureId, const String& name, const size_t width, const size_t height) {
            return TexturePtr(new Texture(textureId, name, width, height));
        }

        Texture::Texture(const GLuint textureId, const String& name, const size_t width, const size_t height) :
        m_textureId(textureId),
        m_name(name),
        m_width(width),
        m_height(height),
        m_usageCount(0),
        m_overridden(false) {}
    }
}