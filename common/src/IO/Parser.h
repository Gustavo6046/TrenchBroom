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

#ifndef TrenchBroom_Parser
#define TrenchBroom_Parser

#include "Exceptions.h"
#include "StringUtils.h"
#include "IO/ParserStatus.h"
#include "IO/Token.h"

#include <map>

namespace TrenchBroom {
    namespace IO {
        template <typename TokenType>
        class Parser {
        protected:
            typedef std::map<TokenType, String> TokenNameMap;
        private:
            typedef TokenTemplate<TokenType> Token;
            mutable TokenNameMap m_tokenNames;
        public:
            virtual ~Parser() {}
        protected:
            bool check(const TokenType typeMask, const Token& token) const {
                return (token.type() & typeMask) != 0;
            }
            
            void expect(const TokenType typeMask, const Token& token) const {
                if (!check(typeMask, token))
                    throw ParserException(token.line(), token.column()) << expectString(tokenName(typeMask), token);
            }
            
            void expect(ParserStatus& status, const TokenType typeMask, const Token& token) const {
                if (!check(typeMask, token))
                    expect(status, tokenName(typeMask), token);
            }
            
            void expect(ParserStatus& status, const String& typeName, const Token& token) const {
                const String msg = expectString(typeName, token);
                status.error(token.line(), token.column(), msg);
                throw ParserException(token.line(), token.column(), msg);
            }
        private:
            String expectString(const String& expected, const Token& token) const {
                const String data(token.begin(), token.end());
                StringStream msg;
                msg << " Expected " << expected << ", but got '" << data << "'";
                return msg.str();
            }
        protected:

            String tokenName(const TokenType typeMask) const {
                if (m_tokenNames.empty())
                    m_tokenNames = tokenNames();
                
                StringList names;
                typename TokenNameMap::const_iterator it, end;
                for (it = m_tokenNames.begin(), end = m_tokenNames.end(); it != end; ++it) {
                    const TokenType type = it->first;
                    const String& name = it->second;
                    if ((typeMask & type) != 0)
                        names.push_back(name);
                }
                
                if (names.empty())
                    return "unknown token type";
                if (names.size() == 1)
                    return names[0];
                return StringUtils::join(names, ", ", ", or ", " or ");
            }
        private:
            virtual TokenNameMap tokenNames() const = 0;
        };
    }
}

#endif /* defined(TrenchBroom_Parser) */
