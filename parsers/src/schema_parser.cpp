// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "schema_tokens.h"

// User implementation prologue.

#line 51 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:412
// Unqualified %code blocks.
#line 44 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:413

#include "schema_driver.h"

#undef yylex
#define yylex scanner.yylex

void debug_r(const char *s, ...);

#line 62 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:479
namespace com { namespace wookler { namespace reactfs { namespace core { namespace parsers {
#line 148 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:479

  /// Build a parser object.
  schema_parser::schema_parser (schema_scanner  &scanner_yyarg, schema_driver  &driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

  schema_parser::~schema_parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  schema_parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  schema_parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  schema_parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  schema_parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  schema_parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  schema_parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  schema_parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  schema_parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  schema_parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  schema_parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  schema_parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  schema_parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  schema_parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  schema_parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  schema_parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  schema_parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  schema_parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  schema_parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  schema_parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  schema_parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  schema_parser::symbol_number_type
  schema_parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  schema_parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  schema_parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  schema_parser::stack_symbol_type&
  schema_parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  schema_parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  schema_parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  schema_parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  schema_parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  schema_parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  schema_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  schema_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  schema_parser::debug_level_type
  schema_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  schema_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline schema_parser::state_type
  schema_parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  schema_parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  schema_parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  schema_parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 4:
#line 127 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
											
										}
#line 593 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 5:
#line 131 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new type [%s]", ss.c_str());
							driver.add_type(ss); 
						}
#line 603 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 8:
#line 143 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yylhs.value.str), (yystack_[2].value.str), (yystack_[1].value.str));
							std::string t((yystack_[2].value.str));
							std::string n((yystack_[1].value.str));
							driver.add_declaration(n, t); 
							FREE_PTR((yystack_[2].value.str));
							FREE_PTR((yystack_[1].value.str));
						}
#line 616 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 9:
#line 151 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yystack_[2].value.str), (yystack_[1].value.str));
							std::string t((yystack_[2].value.str));
							std::string n((yystack_[1].value.str));
							driver.add_declaration(n, t, true); 
						}
#line 627 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 10:
#line 157 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", (yystack_[4].value.str), (yystack_[2].value.str), (yystack_[1].value.lval));
									std::string t((yystack_[4].value.str));
									std::string n((yystack_[2].value.str));
									int s = (yystack_[1].value.lval);
									driver.add_array_decl(n, s, t, false);
									FREE_PTR((yystack_[4].value.str));
									FREE_PTR((yystack_[2].value.str));
								}
#line 641 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 11:
#line 166 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", (yystack_[4].value.str), (yystack_[2].value.str), (yystack_[1].value.lval));
									std::string t((yystack_[4].value.str));
									std::string n((yystack_[2].value.str));
									int s = (yystack_[1].value.lval);
									driver.add_array_decl(n, s, t, true);
									FREE_PTR((yystack_[4].value.str));
									FREE_PTR((yystack_[2].value.str));
	
								}
#line 656 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 176 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[3].value.str), (yystack_[1].value.str));
									std::string t((yystack_[3].value.str));
									std::string n((yystack_[1].value.str));
									driver.add_list_decl(n, t, false);
									FREE_PTR((yystack_[3].value.str));
									FREE_PTR((yystack_[1].value.str));

								}
#line 670 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 185 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[3].value.str), (yystack_[1].value.str));
									std::string t((yystack_[3].value.str));
									std::string n((yystack_[1].value.str));
									driver.add_list_decl(n, t, true);
									FREE_PTR((yystack_[3].value.str));
									FREE_PTR((yystack_[1].value.str));	
									}
#line 683 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 14:
#line 193 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str), (yystack_[1].value.str));
									std::string kt((yystack_[5].value.str));
									std::string vt((yystack_[3].value.str));
									std::string n((yystack_[1].value.str));
									driver.add_map_decl(n, kt, vt, false);
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));
									FREE_PTR((yystack_[1].value.str));
											}
#line 698 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 15:
#line 203 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", (yystack_[6].value.str), (yystack_[3].value.str), (yystack_[1].value.str));
									std::string kt((yystack_[6].value.str));
									std::string vt((yystack_[3].value.str));
									std::string n((yystack_[1].value.str));
									driver.add_map_decl(n, kt, vt, true);
									FREE_PTR((yystack_[6].value.str));
									FREE_PTR((yystack_[3].value.str));
									FREE_PTR((yystack_[1].value.str));
											}
#line 713 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 224 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("varaible=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 719 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 228 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 725 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 20:
#line 229 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 731 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 21:
#line 230 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 737 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 231 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 743 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 232 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 749 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 233 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 755 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 234 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 761 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 235 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 767 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 236 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 773 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 237 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 779 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 238 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 785 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 30:
#line 239 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 791 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 31:
#line 243 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("size=%d", (yystack_[1].value.lval)); (yylhs.value.lval)=(yystack_[1].value.lval); }
#line 797 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 32:
#line 246 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.finish_def(); }
#line 803 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 252 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 814 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 258 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 825 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 36:
#line 264 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 836 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 37:
#line 270 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 847 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 38:
#line 276 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 858 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 39:
#line 282 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 869 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 40:
#line 288 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 880 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 41:
#line 294 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 891 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 42:
#line 300 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 902 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 43:
#line 306 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 913 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 315 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
						int size = strlen((yystack_[0].value.str));
						char *ptr = (char *)malloc(sizeof(char) * size);
						CHECK_ALLOC(ptr, TYPE_NAME(char));
						memset(ptr, 0, size);
						sprintf(ptr, "%s", (yystack_[0].value.str));
						FREE_PTR((yystack_[0].value.str));
						(yylhs.value.str) = ptr;
						debug_r("VALUES {%s}", (yylhs.value.str)); 
 					}
#line 928 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 325 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
						int size = strlen((yystack_[2].value.str)) + strlen((yystack_[0].value.str)) + 4;
						char *ptr = (char *)malloc(sizeof(char) * size);
						CHECK_ALLOC(ptr, TYPE_NAME(char));
						memset(ptr, 0, size);
						sprintf(ptr, "%s,%s", (yystack_[2].value.str), (yystack_[0].value.str));
						FREE_PTR((yystack_[2].value.str));
						FREE_PTR((yystack_[0].value.str));
						(yylhs.value.str) = ptr;
						debug_r("VALUES {%s}", (yylhs.value.str)); 
 					}
#line 944 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 340 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%ld", (yystack_[0].value.lval));
					(yylhs.value.str) = ptr; 
				}
#line 956 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 347 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%f", (yystack_[0].value.dval));
					(yylhs.value.str) = ptr; 
				}
#line 968 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 354 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("String value [%s]", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 974 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;


#line 978 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  schema_parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  schema_parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char schema_parser::yypact_ninf_ = -76;

  const signed char schema_parser::yytable_ninf_ = -1;

  const short int
  schema_parser::yypact_[] =
  {
      -5,     4,     5,   -76,   -18,   -76,   -76,   -76,    65,     9,
     -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,     2,    21,    33,    -2,   -76,     9,   -76,     9,
      81,    94,   119,   -10,    65,     8,     8,     9,    34,     9,
      35,    28,   -76,   -76,   -76,    41,   -76,   -76,   -76,    37,
       9,    38,     9,   107,     0,     9,    42,     9,     8,     9,
      40,    95,   112,   114,    12,   115,   116,   118,    42,   142,
       8,     8,   -76,   117,     9,   128,    16,    16,   103,   122,
     123,   124,   125,    16,    16,   -76,     8,   126,   -76,   -76,
       9,     8,   -76,   -76,   -76,   -76,   -25,   -76,    17,   135,
      16,    16,    16,    16,   127,   129,   -76,   -76,     8,   -76,
     -76,    16,   -76,   -76,    29,    30,   130,   131,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76
  };

  const unsigned char
  schema_parser::yydefact_[] =
  {
       0,     0,     0,     2,     0,     5,     1,     3,     0,     0,
      20,    21,    22,    23,    24,    25,    26,    19,    27,    28,
      30,    29,     0,     0,     0,     0,     6,     0,    18,     0,
       0,     0,     0,     0,     0,    16,    16,     0,     0,     0,
       0,     0,    32,     4,     7,     0,     8,    17,     9,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    16,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      16,    16,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    16,     0,    10,    13,
       0,    16,    34,    48,    46,    47,     0,    44,     0,     0,
       0,     0,     0,     0,     0,     0,    11,    31,    16,    14,
      36,     0,    38,    35,     0,     0,     0,     0,    40,    42,
      15,    45,    37,    39,    41,    43
  };

  const short int
  schema_parser::yypgoto_[] =
  {
     -76,   -76,   155,   -76,   -76,   132,   -36,   -26,   -14,    93,
     -76,   -76,   -76,   -63,   -75
  };

  const signed char
  schema_parser::yydefgoto_[] =
  {
      -1,     2,     3,     4,    25,    26,    46,    29,    27,    70,
      43,    47,    67,    96,    97
  };

  const unsigned char
  schema_parser::yytable_[] =
  {
      48,    35,     1,    36,   110,     6,     8,   111,   104,   105,
       5,    49,     1,    51,    98,    28,    38,    40,    41,    93,
      94,    95,    72,    33,    56,    42,    58,   116,   117,    68,
      34,    71,    30,    73,    88,    89,   121,   114,   115,    60,
      61,    62,    63,    64,    65,    66,   112,    45,    91,   111,
     106,    31,    78,    79,    80,   109,    81,    82,   122,   123,
      53,   111,   111,    32,   108,    50,    52,    54,    55,    57,
      69,    74,   120,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    37,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    39,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    59,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      76,    75,    77,    83,    84,    85,    87,    92,    90,    99,
     100,   101,   102,   103,   113,   107,   118,     7,   119,   124,
     125,    86,     0,     0,     0,     0,    44
  };

  const signed char
  schema_parser::yycheck_[] =
  {
      36,    27,     7,    29,    29,     0,    24,    32,    83,    84,
       6,    37,     7,    39,    77,     6,    30,    31,    32,     3,
       4,     5,    58,    25,    50,    35,    52,   102,   103,    55,
      32,    57,    30,    59,    70,    71,   111,   100,   101,    53,
      40,    41,    42,    43,    44,    45,    29,    39,    74,    32,
      86,    30,    40,    41,    42,    91,    44,    45,    29,    29,
      32,    32,    32,    30,    90,    31,    31,    26,    31,    31,
      28,    31,   108,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      28,    46,    28,    28,    28,    27,     4,    19,    31,    46,
      28,    28,    28,    28,    19,    29,    29,     2,    29,    29,
      29,    68,    -1,    -1,    -1,    -1,    34
  };

  const unsigned char
  schema_parser::yystos_[] =
  {
       0,     7,    49,    50,    51,     6,     0,    50,    24,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    52,    53,    56,     6,    55,
      30,    30,    30,    25,    32,    55,    55,     8,    56,     8,
      56,    56,    35,    58,    53,    39,    54,    59,    54,    55,
      31,    55,    31,    32,    26,    31,    55,    31,    55,     8,
      56,    40,    41,    42,    43,    44,    45,    60,    55,    28,
      57,    55,    54,    55,    31,    46,    28,    28,    40,    41,
      42,    44,    45,    28,    28,    27,    57,     4,    54,    54,
      31,    55,    19,     3,     4,     5,    61,    62,    61,    46,
      28,    28,    28,    28,    62,    62,    54,    29,    55,    54,
      29,    32,    29,    19,    61,    61,    62,    62,    29,    29,
      54,    62,    29,    29,    29,    29
  };

  const unsigned char
  schema_parser::yyr1_[] =
  {
       0,    48,    49,    49,    50,    51,    52,    52,    53,    53,
      53,    53,    53,    53,    53,    53,    54,    54,    55,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    57,    58,    59,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    61,    61,    62,    62,    62
  };

  const unsigned char
  schema_parser::yyr2_[] =
  {
       0,     2,     1,     2,     5,     2,     1,     3,     3,     4,
       7,     8,     6,     7,     8,     9,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     3,     4,     4,     5,     4,     5,
       4,     5,     4,     5,     1,     3,     1,     1,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const schema_parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "SVALUE", "IVALUE", "DVALUE",
  "VARNAME", "TYPE", "DATATYPE", "BYTE", "CHAR", "BOOL", "SHORT",
  "INTEGER", "LONG", "FLOAT", "DOUBLE", "TIMESTAMP", "DATETIME", "STRING",
  "TEXT", "ARRAY", "LIST", "MAP", "LTYPEBRACE", "RTYPEBRACE", "LINBRACE",
  "RINBRACE", "LSZBRACE", "RSZBRACE", "LINTYPBRACE", "RINTYPBRACE",
  "COMMA", "DEFAULT", "REF", "TYPE_END", "PRIMARY_KEY", "INDEX", "SCHEMA",
  "CONSTRAINT", "REGEX", "IN", "BETWEEN", "NOT", "LT", "GT", "COLON",
  "NEWLINE", "$accept", "types", "type", "type_declare", "declarations",
  "declare", "opt_constraint", "variable", "datatype", "size_def",
  "declare_finish", "constraint", "constraint_type", "values", "value", YY_NULLPTR
  };


  const unsigned short int
  schema_parser::yyrline_[] =
  {
       0,   123,   123,   124,   127,   131,   137,   138,   143,   151,
     157,   166,   176,   185,   193,   203,   217,   219,   224,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   243,   246,   249,   252,   258,   264,   270,   276,   282,
     288,   294,   300,   306,   315,   325,   340,   347,   354
  };

  // Print the state stack on the debug stream.
  void
  schema_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  schema_parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  schema_parser::token_number_type
  schema_parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47
    };
    const unsigned int user_token_number_max_ = 302;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 5 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1167
} } } } } // com::wookler::reactfs::core::parsers
#line 1393 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:1167
#line 358 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1168


void com::wookler::reactfs::core::parsers::schema_parser::error( const location_type &l, const std::string &err_message ) {
	throw TYPE_PARSER_ERROR("Parser error occurred. [error=%s][location=%s]", err_message.c_str(), l);
}

void debug_r(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    printf("debug: ");
    vfprintf(stdout, s, ap);
    printf("\n");
}
