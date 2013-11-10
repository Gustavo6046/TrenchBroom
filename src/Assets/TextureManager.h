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

#ifndef __TrenchBroom__TextureManager__
#define __TrenchBroom__TextureManager__

#include "Assets/AssetTypes.h"
#include "Model/ModelTypes.h"
#include "IO/Path.h"

#include <map>

namespace TrenchBroom {
    namespace Assets {
        class TextureManager {
        public:
            enum SortOrder {
                Name  = 0,
                Usage = 1
            };
            typedef std::pair<TextureCollection*, TextureList> Group;
            typedef std::vector<Group> GroupList;
        private:
            typedef std::map<IO::Path, TextureCollection*> TextureCollectionMap;
            typedef std::pair<IO::Path, TextureCollection*> TextureCollectionMapEntry;
            typedef std::map<String, Texture*> TextureMap;
            
            Model::GamePtr m_game;
            
            TextureCollectionList m_collections;
            TextureCollectionMap m_collectionsByPath;
            TextureCollectionMap m_toRemove;
            TextureList m_sortedTextures[2];
            GroupList m_sortedGroups[2];
            
            TextureMap m_texturesByName;
        public:
            ~TextureManager();

            void addTextureCollection(const IO::Path& path);
            void addTextureCollections(const IO::Path::List& paths);
            void removeTextureCollection(const IO::Path& path);
            
            void reset(Model::GamePtr game);
            void commitChanges();
            
            Texture* texture(const String& name) const;
            const TextureList& textures(const SortOrder sortOrder) const;
            const GroupList& groups(const SortOrder sortOrder) const;
            const TextureCollectionList& collections() const;
        private:
            void doAddTextureCollection(const IO::Path& path, TextureCollectionList& collections, TextureCollectionMap& collectionsByPath, TextureCollectionMap& toRemove);
            void doAddTextureCollection(const IO::Path& path, TextureCollection* collection, TextureCollectionList& collections, TextureCollectionMap& collectionsByPath);
            void updateTextures();
            TextureList textureList() const;
        };
    }
}

#endif /* defined(__TrenchBroom__TextureManager__) */