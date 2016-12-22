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
#line 140 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new schema [%s]", ss.c_str());
							driver.create_schema(ss);
							FREE_PTR((yystack_[0].value.str));
						}
#line 596 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 152 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[1].value.str));
							debug_r("namespace [%s]", ss.c_str());
							driver.set_namespace(ss);
							FREE_PTR((yystack_[1].value.str));

						}
#line 608 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 171 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {

						}
#line 616 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 177 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_index_fields(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 627 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 190 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 633 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 191 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 639 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 192 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 645 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 196 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string n((yystack_[2].value.str));
							std::string s((yystack_[0].value.str));
							debug_r("Creating index [%s] on schema [%s]", n.c_str(), s.c_str());
							driver.create_index(n, s);
							FREE_PTR((yystack_[2].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 658 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 212 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_primary_key(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 669 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 221 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 677 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 224 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							int s = strlen((yystack_[2].value.str)) + strlen((yystack_[0].value.str)) + 3;
							char *p = (char *)malloc(sizeof(char) * s);
							CHECK_ALLOC(p, TYPE_NAME(char));
							memset(p, 0, s);
							sprintf(p, "%s,%s", (yystack_[2].value.str), (yystack_[0].value.str));	
							(yylhs.value.str) = p;
							FREE_PTR((yystack_[2].value.str));
							FREE_PTR((yystack_[0].value.str));
							debug_r("COLUMNS [%s]", (yylhs.value.str));
							
						}
#line 694 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 238 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							debug_r("COLUMN [%s] SORT[%s]", (yystack_[1].value.str), (yystack_[0].value.str));
							int s = strlen((yystack_[1].value.str) + 1);
							if (NOT_NULL((yystack_[0].value.str))) {	
								s += strlen((yystack_[0].value.str));
								char *p = (char *)malloc(s * sizeof(char));
								CHECK_ALLOC(p, TYPE_NAME(char));
								memset(p, 0, s);
								sprintf(p, "%s %s", (yystack_[1].value.str), (yystack_[0].value.str));
								FREE_PTR((yystack_[0].value.str));
								(yylhs.value.str) = p;
							} else {
								(yylhs.value.str) = strdup((yystack_[1].value.str));
							}
							FREE_PTR((yystack_[1].value.str));
						}
#line 715 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 257 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = nullptr;
						}
#line 723 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 260 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 732 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 267 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 740 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 270 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 748 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 285 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
											
										}
#line 756 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 289 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new type [%s]", ss.c_str());
							driver.add_type(ss); 
							FREE_PTR((yystack_[0].value.str));
						}
#line 767 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 311 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s nullable=%d", (yystack_[4].value.str), (yystack_[3].value.str), (yystack_[0].value.lval));
							std::string t((yystack_[4].value.str));
							std::string n((yystack_[3].value.str));
							driver.add_declaration(n, t, false, (yystack_[0].value.lval)); 
							FREE_PTR((yystack_[4].value.str));
							FREE_PTR((yystack_[3].value.str));
						}
#line 780 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 322 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yystack_[4].value.str), (yystack_[3].value.str));
							debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
							std::string t((yystack_[4].value.str));
							std::string n((yystack_[3].value.str));
							driver.add_declaration(n, t, true, (yystack_[0].value.lval)); 
						}
#line 792 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 332 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_list_decl(n, t, false, (yystack_[0].value.lval));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));

								}
#line 807 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 345 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_list_decl(n, t, true, (yystack_[0].value.lval));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));	
								}
#line 821 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 356 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", (yystack_[7].value.str), (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string kt((yystack_[7].value.str));
									std::string vt((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_map_decl(n, kt, vt, false, (yystack_[0].value.lval));
									FREE_PTR((yystack_[7].value.str));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));
								}
#line 837 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 370 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", (yystack_[8].value.str), (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string kt((yystack_[8].value.str));
									std::string vt((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_map_decl(n, kt, vt, true, (yystack_[0].value.lval));
									FREE_PTR((yystack_[8].value.str));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));
								}
#line 853 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 384 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	(yylhs.value.lval) = 0; }
#line 859 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 385 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	(yylhs.value.lval) = 1; driver.set_nullable(); }
#line 865 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 53:
#line 390 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							debug_r("Defualt : %s", (yystack_[0].value.str));
							std::string v((yystack_[0].value.str));
							driver.set_default_value(v);
							FREE_PTR((yystack_[0].value.str));
						}
#line 876 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 56:
#line 402 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 885 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 406 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							int s = strlen((yystack_[2].value.str)) + strlen((yystack_[0].value.str)) + 2;
							char *p = (char *)malloc(s * sizeof(char));
							CHECK_ALLOC(p, TYPE_NAME(char));
							memset(p, 0, s);
							sprintf(p, "%s.%s", (yystack_[2].value.str), (yystack_[0].value.str));
							FREE_PTR((yystack_[2].value.str));
							FREE_PTR((yystack_[0].value.str));
							(yylhs.value.str) = p;
						}
#line 900 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 58:
#line 420 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("varaible=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 906 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 59:
#line 424 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 912 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 425 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 918 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 61:
#line 426 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 924 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 62:
#line 427 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 930 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 63:
#line 428 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 936 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 429 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 942 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 430 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 948 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 431 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 954 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 432 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 960 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 433 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 966 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 69:
#line 434 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 972 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 70:
#line 435 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 978 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 71:
#line 439 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.finish_def(); }
#line 984 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 445 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 995 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 74:
#line 451 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1006 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 75:
#line 457 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1017 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 76:
#line 463 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1028 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 77:
#line 469 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1039 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 78:
#line 475 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1050 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 79:
#line 481 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1061 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 80:
#line 487 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1072 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 81:
#line 493 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1083 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 82:
#line 499 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1094 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 83:
#line 506 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1105 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 84:
#line 512 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1116 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 85:
#line 518 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1127 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 86:
#line 524 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1138 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 87:
#line 533 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1153 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 88:
#line 543 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1169 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 89:
#line 558 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%ld", (yystack_[0].value.lval));
					(yylhs.value.str) = ptr; 
				}
#line 1181 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 90:
#line 565 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%f", (yystack_[0].value.dval));
					(yylhs.value.str) = ptr; 
				}
#line 1193 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 91:
#line 572 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("String value [%s]", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1199 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;


#line 1203 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
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


  const signed char schema_parser::yypact_ninf_ = -128;

  const signed char schema_parser::yytable_ninf_ = -1;

  const short int
  schema_parser::yypact_[] =
  {
     -27,    10,    27,    21,  -128,  -128,   -21,  -128,  -128,    10,
      -2,    21,  -128,    16,  -128,    10,  -128,    10,    34,    46,
    -128,    93,  -128,  -128,    10,  -128,    34,  -128,    45,    93,
      10,  -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,
    -128,  -128,  -128,    31,    39,   -12,  -128,  -128,  -128,  -128,
    -128,  -128,  -128,    10,    42,  -128,    10,    -1,   -11,    10,
     115,   146,    56,    93,    54,    10,     9,  -128,   -28,  -128,
    -128,  -128,    56,  -128,    59,    54,    10,    49,    65,  -128,
    -128,  -128,    70,    66,  -128,  -128,  -128,    10,  -128,  -128,
    -128,  -128,  -128,    71,    56,  -128,    66,    55,    10,   134,
      41,    57,    76,  -128,    10,  -128,    76,    10,    54,    10,
      73,    69,    92,    94,   130,    57,    57,    57,    57,   110,
    -128,  -128,  -128,  -128,  -128,  -128,    11,  -128,    54,    66,
      95,    10,   101,    57,    57,    88,   111,   113,    57,    57,
      57,    57,  -128,  -128,  -128,  -128,  -128,  -128,    66,    76,
      10,    54,  -128,    13,  -128,    29,   151,    57,    57,  -128,
    -128,  -128,  -128,    76,  -128,    54,    66,  -128,    57,  -128,
    -128,    35,    37,  -128,    66,    76,  -128,  -128,  -128,    76,
    -128,  -128
  };

  const unsigned char
  schema_parser::yydefact_[] =
  {
       5,     0,     0,    30,     6,    58,     0,    56,     1,     0,
       0,    31,    32,     0,     7,     0,    35,     0,     8,     0,
      33,     0,    57,     4,     0,     2,     9,    10,     0,     0,
       0,    60,    61,    62,    63,    64,    65,    66,    59,    67,
      68,    70,    69,     0,     0,     0,    36,    38,    39,    40,
      41,    42,    43,     0,     0,    11,     0,    14,     0,     0,
       0,     0,     0,     0,    54,     0,     0,    23,    26,    16,
      17,    18,     0,    15,    20,    54,     0,     0,     0,    71,
      34,    37,     0,    52,    55,    19,    13,     0,    28,    29,
      25,    27,    12,     0,     0,    21,    52,     0,     0,     0,
       0,     0,    50,    24,     0,     3,    50,     0,    54,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      91,    89,    90,    53,    51,    44,     0,    45,    54,    52,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    79,    83,    81,    85,    72,    22,    52,    50,
       0,    54,    73,     0,    87,     0,     0,     0,     0,    80,
      84,    82,    86,    50,    46,    54,    52,    75,     0,    77,
      74,     0,     0,    47,    52,    50,    88,    76,    78,    50,
      48,    49
  };

  const short int
  schema_parser::yypgoto_[] =
  {
    -128,  -128,  -128,  -128,  -128,  -128,  -128,  -128,   145,  -128,
    -128,  -128,  -128,  -128,  -128,    68,    90,  -128,  -128,  -128,
    -128,   167,  -128,   150,   117,  -128,  -128,  -128,  -128,  -128,
    -128,  -104,   -93,   -74,   180,    -9,   -56,   -62,  -128,  -128,
    -127,   -92
  };

  const short int
  schema_parser::yydefgoto_[] =
  {
      -1,     2,    18,    19,     3,     4,    25,    26,    27,    57,
      72,    73,    28,    94,    95,    66,    67,    90,    91,    10,
      11,    12,    13,    45,    46,    47,    48,    49,    50,    51,
      52,   125,   102,    83,    68,     7,    53,    80,    84,   119,
     153,   154
  };

  const unsigned char
  schema_parser::yytable_[] =
  {
      16,    96,   127,   106,    77,    78,    22,   155,    23,   123,
      92,    14,    62,    74,     1,    54,     5,    63,    63,    88,
      89,    59,    15,   142,   143,   144,   145,     8,     9,    15,
     171,   172,   105,    17,   129,    86,   149,   147,    87,    21,
      87,   167,   168,   110,    64,   164,   159,   160,   161,   162,
      75,    69,    70,    71,   148,   163,    85,   169,   168,   173,
     120,   121,   122,   177,   168,   178,   168,    97,    24,    29,
      56,   180,    65,   175,    60,   181,   176,   166,   111,   112,
     113,   179,    61,   114,   115,   116,   117,   118,    79,   108,
      82,   174,    93,    98,    99,   100,   104,   101,   128,   107,
     130,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,   124,   131,   132,   133,
     152,   134,   151,    76,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,   146,   156,   157,   150,
     158,   165,   109,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,   135,   136,   137,
     170,    55,   126,   138,   139,   140,   141,   103,    20,    58,
      81,     6
  };

  const unsigned char
  schema_parser::yycheck_[] =
  {
       9,    75,   106,    96,    60,    61,    15,   134,    17,   101,
      72,    32,    24,    24,    41,    24,     6,    29,    29,    47,
      48,    30,    50,   115,   116,   117,   118,     0,     7,    50,
     157,   158,    94,    35,   108,    26,   129,    26,    29,    23,
      29,    28,    29,    99,    53,   149,   138,   139,   140,   141,
      59,    52,    53,    54,   128,   148,    65,    28,    29,   163,
       3,     4,     5,    28,    29,    28,    29,    76,    34,    23,
      25,   175,    30,   166,    43,   179,   168,   151,    37,    38,
      39,   174,    43,    42,    43,    44,    45,    46,    32,    98,
      36,   165,    33,    44,    29,    25,    25,    31,   107,    44,
     109,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    40,    44,    49,    27,
      19,    27,   131,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    26,    49,    27,    44,
      27,   150,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    37,    38,    39,
      19,    26,   104,    43,    44,    45,    46,    87,    11,    29,
      63,     1
  };

  const unsigned char
  schema_parser::yystos_[] =
  {
       0,    41,    56,    59,    60,     6,    89,    90,     0,     7,
      74,    75,    76,    77,    32,    50,    90,    35,    57,    58,
      76,    23,    90,    90,    34,    61,    62,    63,    67,    23,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    78,    79,    80,    81,    82,
      83,    84,    85,    91,    90,    63,    25,    64,    78,    90,
      43,    43,    24,    29,    90,    30,    70,    71,    89,    52,
      53,    54,    65,    66,    24,    90,     8,    91,    91,    32,
      92,    79,    36,    88,    93,    90,    26,    29,    47,    48,
      72,    73,    92,    33,    68,    69,    88,    90,    44,    29,
      25,    31,    87,    71,    25,    92,    87,    44,    90,     8,
      91,    37,    38,    39,    42,    43,    44,    45,    46,    94,
       3,     4,     5,    96,    40,    86,    70,    86,    90,    88,
      90,    44,    49,    27,    27,    37,    38,    39,    43,    44,
      45,    46,    96,    96,    96,    96,    26,    26,    88,    87,
      44,    90,    19,    95,    96,    95,    49,    27,    27,    96,
      96,    96,    96,    87,    86,    90,    88,    28,    29,    28,
      19,    95,    95,    86,    88,    87,    96,    28,    28,    87,
      86,    86
  };

  const unsigned char
  schema_parser::yyr1_[] =
  {
       0,    55,    56,    57,    58,    59,    59,    60,    61,    61,
      62,    62,    63,    64,    65,    65,    66,    66,    66,    67,
      68,    68,    69,    70,    70,    71,    72,    72,    73,    73,
      74,    74,    75,    75,    76,    77,    78,    78,    79,    79,
      79,    79,    79,    79,    80,    81,    82,    83,    84,    85,
      86,    86,    87,    87,    88,    88,    89,    89,    90,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    92,    93,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    95,    95,    96,
      96,    96
  };

  const unsigned char
  schema_parser::yyr2_[] =
  {
       0,     2,     4,     6,     2,     0,     1,     3,     0,     1,
       1,     2,     4,     3,     0,     1,     1,     1,     1,     4,
       0,     1,     4,     1,     3,     2,     0,     1,     1,     1,
       0,     1,     1,     2,     5,     2,     1,     3,     1,     1,
       1,     1,     1,     1,     5,     6,     8,     9,    10,    11,
       0,     1,     0,     2,     0,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     3,     4,     4,     5,     4,     5,     2,
       3,     2,     3,     2,     3,     2,     3,     1,     3,     1,
       1,     1
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
  "TEXT", "LIST", "MAP", "LTYPEBRACE", "RTYPEBRACE", "LINBRACE",
  "RINBRACE", "LSZBRACE", "RSZBRACE", "COMMA", "ON", "DEFAULT", "TYPE_END",
  "KEY_FIELDS", "INDEX", "SCHEMA", "CONSTRAINT", "REGEX", "IN", "BETWEEN",
  "NULLABLE", "NAMESPACE", "NOT", "LT", "GT", "LTEQ", "GTEQ", "ASC",
  "DESC", "COLON", "DOT", "NEWLINE", "FULLTEXT_INDEX", "HASH_INDEX",
  "TREE_INDEX", "$accept", "parse", "schema", "schema_declare",
  "opt_namespace", "namespace", "opt_indexes", "indexes", "index",
  "index_fields", "opt_index_type", "index_type", "index_declare",
  "opt_key_fields", "key_fields", "columns", "column", "opt_sort", "sort",
  "opt_types", "types", "type", "type_declare", "declarations", "declare",
  "declare_native", "declare_ref", "declare_native_list",
  "declare_ref_list", "declare_netive_map", "declare_ref_map",
  "opt_nullable", "opt_default", "opt_constraint", "nested_variable",
  "variable", "datatype", "declare_finish", "constraint",
  "constraint_type", "values", "value", YY_NULLPTR
  };


  const unsigned short int
  schema_parser::yyrline_[] =
  {
       0,   132,   132,   136,   140,   146,   148,   152,   160,   162,
     166,   167,   171,   177,   184,   186,   190,   191,   192,   196,
     206,   208,   212,   221,   224,   238,   257,   260,   267,   270,
     275,   277,   281,   282,   285,   289,   296,   297,   302,   303,
     304,   305,   306,   307,   311,   322,   332,   345,   356,   370,
     384,   385,   388,   390,   396,   398,   402,   406,   420,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   439,   442,   445,   451,   457,   463,   469,   475,   481,
     487,   493,   499,   506,   512,   518,   524,   533,   543,   558,
     565,   572
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
    };
    const unsigned int user_token_number_max_ = 309;
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
#line 1669 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:1167
#line 576 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1168


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
