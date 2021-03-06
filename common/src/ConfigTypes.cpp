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

#include "ConfigTypes.h"

#include "CollectionUtils.h"

#include <cassert>

namespace TrenchBroom {
    ConfigEntry::~ConfigEntry() {}
    
    ConfigEntry::Type ConfigEntry::type() const {
        return m_type;
    }

    ConfigEntry::operator const String&() const {
        const ConfigValue& asValue = *this;
        return static_cast<const String&>(asValue);
    }
    
    ConfigEntry::operator const ConfigValue&() const {
        assert(m_type == Type_Value);
        return static_cast<const ConfigValue&>(*this);
    }
    
    ConfigEntry::operator const ConfigList&() const {
        assert(m_type == Type_List);
        return static_cast<const ConfigList&>(*this);
    }
    
    ConfigEntry::operator const ConfigTable&() const {
        assert(m_type == Type_Table);
        return static_cast<const ConfigTable&>(*this);
    }

    void ConfigEntry::appendToStream(std::ostream& stream) const {
        doAppendToStream(stream);
    }

    String ConfigEntry::asString() const {
        StringStream stream;
        stream << this;
        return stream.str();
    }

    ConfigEntry::ConfigEntry(const Type type) :
    m_type(type) {}

    std::ostream& operator<<(std::ostream& stream, const ConfigEntry::Ptr& entry) {
        entry->appendToStream(stream);
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const ConfigEntry* entry) {
        entry->appendToStream(stream);
        return stream;
    }
    
    ConfigValue::ConfigValue(const String& value) :
    ConfigEntry(Type_Value),
    m_value(value) {}
    
    ConfigValue::operator const String&() const {
        return m_value;
    }

    void ConfigValue::doAppendToStream(std::ostream& stream) const {
        stream << "\"" << m_value << "\"";
    }

    ConfigList::ConfigList() :
    ConfigEntry(Type_List) {}

    const ConfigEntry& ConfigList::operator[](const size_t index) const {
        assert(index < count());
        return *m_entries[index];
    }
    
    size_t ConfigList::count() const {
        return m_entries.size();
    }
    
    void ConfigList::addEntry(ConfigEntry::Ptr entry) {
        m_entries.push_back(entry);
    }

    void ConfigList::doAppendToStream(std::ostream& stream) const {
        stream << "{";
        if (!m_entries.empty()) {
            for (size_t i = 0; i < m_entries.size() - 1; ++i)
                stream << m_entries[i] << ",";
            stream << m_entries.back();
        }
        stream << "}";
    }

    ConfigTable::ConfigTable() :
    ConfigEntry(Type_Table) {}

    const StringSet& ConfigTable::keys() const {
        return m_keys;
    }
    
    const ConfigEntry& ConfigTable::operator[](const String& key) const {
        EntryMap::const_iterator it = m_entries.find(key);
        assert(it != m_entries.end());
        return *it->second;
    }
    
    size_t ConfigTable::count() const {
        return m_entries.size();
    }
    
    bool ConfigTable::contains(const String& key) const {
        return m_keys.count(key) > 0;
    }

    void ConfigTable::addEntry(const String& key, ConfigEntry::Ptr entry) {
        if (MapUtils::insertOrReplace(m_entries, key, entry))
            m_keys.insert(key);
    }

    void ConfigTable::doAppendToStream(std::ostream& stream) const {
        stream << "{";
        EntryMap::const_iterator it = m_entries.begin();
        EntryMap::const_iterator end = m_entries.end();
        while (it != end) {
            const String& key = it->first;
            const ConfigEntry::Ptr entry = it->second;
            stream << key << "=" << entry;
            ++it;
            if (it != end)
                stream << ",";
        }
        stream << "}";
    }
}
