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

#ifndef __TrenchBroom__ContainerBar__
#define __TrenchBroom__ContainerBar__

#include <wx/panel.h>

namespace TrenchBroom {
    namespace View {
        class ContainerBar : public wxPanel {
        public:
            ContainerBar(wxWindow* parent);
            
            void OnPaint(wxPaintEvent& event);
        private:
            void paintMac(wxPaintEvent& event);
            void paintOther(wxPaintEvent& event);
        };
    }
}

#endif /* defined(__TrenchBroom__ContainerBar__) */