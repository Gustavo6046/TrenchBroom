/*
 Copyright (C) 2010-2012 Kristian Duske

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

#pragma once
#include "Renderer/FontManager.h"

namespace TrenchBroom {
	namespace Renderer {
		namespace StringFactoryCallback {
            void gluTessBeginData(GLenum type, StringData* data);
            void gluTessVertexData(StringData::Point* vertex, StringData* data);
            void gluTessCombineData(GLdouble coords[3], void *vertexData[4], GLfloat weight[4], void **outData, StringData* data);
            void gluTessEndData(StringData* data);
			void gluTessError(GLenum errorCode);
        }

		class WinStringFactory :public StringFactory {
        private:
            GLUtesselator* m_gluTess;
			HDC m_dc;
		public:
			WinStringFactory(HDC mainDC);
			~WinStringFactory();
			StringData* createStringData(const FontDescriptor& descriptor, const string& str);
		};
	}
}