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

#include "Model/Issue.h"

#include "CollectionUtils.h"
#include "Model/Entity.h"
#include "Model/Node.h"

#include <cassert>

namespace TrenchBroom {
    namespace Model {
        Issue::~Issue() {}

        size_t Issue::seqId() const {
            return m_seqId;
        }

        size_t Issue::lineNumber() const {
            return m_node->lineNumber();
        }
        
        const String Issue::description() const {
            return doGetDescription();
        }

        IssueType Issue::type() const {
            return doGetType();
        }

        Node* Issue::node() const {
            return m_node;
        }

        void Issue::addSelectableNodes(Model::NodeList& nodes) const {
            doAddSelectableNodes(nodes);
        }

        bool Issue::hidden() const {
            return m_node->issueHidden(type());
        }
        
        void Issue::setHidden(const bool hidden) {
            m_node->setIssueHidden(type(), hidden);
        }

        Issue::Issue(Node* node) :
        m_seqId(nextSeqId()),
        m_node(node) {
            assert(m_node != NULL);
        }

        size_t Issue::nextSeqId() {
            static size_t seqId = 0;
            return seqId++;
        }

        IssueType Issue::freeType() {
            static IssueType type = 1;
            const IssueType result = type;
            type = (type << 1);
            return result;
        }

        void Issue::doAddSelectableNodes(Model::NodeList& nodes) const {
            nodes.push_back(m_node);
        }

        EntityIssue::EntityIssue(Node* node) :
        Issue(node) {}

        Entity* EntityIssue::entity() const {
            return static_cast<Entity*>(m_node);
        }

        void EntityIssue::doAddSelectableNodes(Model::NodeList& nodes) const {
            if (m_node->hasChildren())
                VectorUtils::append(nodes, node()->children());
            else
                nodes.push_back(m_node);
        }
    }
}
