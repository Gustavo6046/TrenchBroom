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

#include "CollectTouchingNodesVisitor.h"

#include "Model/Object.h"

#include <cassert>

namespace TrenchBroom {
    namespace Model {
        MatchTouchingNodes::MatchTouchingNodes(const Object* object) :
        m_object(object) {
            assert(m_object != NULL);
        }

        bool MatchTouchingNodes::operator()(const Node* node) const {
            return m_object->intersects(node);
        }

        CollectTouchingNodesVisitor::CollectTouchingNodesVisitor(const Object* object) :
        CollectMatchingNodesVisitor(NodePredicates::And<NodePredicates::Not<NodePredicates::EqualsObject>,
                                    MatchTouchingNodes>
                                    (NodePredicates::Not<NodePredicates::EqualsObject>
                                     (NodePredicates::EqualsObject(object)), MatchTouchingNodes(object))) {}
    }
}
