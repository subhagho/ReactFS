//
// Created by Subhabrata Ghosh on 06/12/16.
//


/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef REACTFS_SCHEMA_SCANNER_H
#define REACTFS_SCHEMA_SCANNER_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#if !defined(yyFlexLexerOnce)

#include "FlexLexer.h"

#endif

#include "core/includes/types/type_defs.h"

#include "schema_tokens.h"
#include "parsers/src/location.hh"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class schema_scanner : public yyFlexLexer {
                    private:
                        /* yyval ptr */
                        com::wookler::reactfs::core::parsers::schema_parser::semantic_type *yylval = nullptr;
                        /* location ptr */
                        com::wookler::reactfs::core::parsers::schema_parser::location_type *loc = nullptr;

                    public:
                        schema_scanner(std::istream *in) : yyFlexLexer(in) {
                            loc = new com::wookler::reactfs::core::parsers::schema_parser::location_type();
                        }

                        //get rid of override virtual function warning
                        using FlexLexer::yylex;

                        virtual ~schema_scanner() {
                            delete (loc);
                        }

                        virtual
                        int yylex(com::wookler::reactfs::core::parsers::schema_parser::semantic_type *const lval,
                                  com::wookler::reactfs::core::parsers::schema_parser::location_type *loc);
                        // YY_DECL defined in mc_lexer.l
                        // Method body created by flex in mc_lexer.yy.cc
                    };
                }
REACTFS_NS_CORE_END

#endif //REACTFS_SCHEMA_SCANNER_H
