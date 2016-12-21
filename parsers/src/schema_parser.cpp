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
#line 141 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new schema [%s]", ss.c_str());
							driver.create_schema(ss);
							FREE_PTR((yystack_[0].value.str));
						}
#line 596 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 153 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[1].value.str));
							debug_r("namespace [%s]", ss.c_str());
							driver.set_namespace(ss);
							FREE_PTR((yystack_[1].value.str));

						}
#line 608 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 172 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {

						}
#line 616 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 178 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_index_fields(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 627 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 191 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 633 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 192 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 639 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 193 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 645 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 197 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 213 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_primary_key(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 669 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 222 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 677 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 225 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 239 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 258 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = nullptr;
						}
#line 723 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 261 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 732 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 268 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 740 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 271 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 748 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 286 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
											
										}
#line 756 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 290 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new type [%s]", ss.c_str());
							driver.add_type(ss); 
							FREE_PTR((yystack_[0].value.str));
						}
#line 767 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 314 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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

  case 47:
#line 325 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yystack_[4].value.str), (yystack_[3].value.str));
							debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
							std::string t((yystack_[4].value.str));
							std::string n((yystack_[3].value.str));
							driver.add_declaration(n, t, true, (yystack_[0].value.lval)); 
						}
#line 792 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 335 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", (yystack_[6].value.str), (yystack_[4].value.str), (yystack_[3].value.lval));
									debug_r("varname=%s nullable=%d", (yystack_[4].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[6].value.str));
									std::string n((yystack_[4].value.str));
									int s = (yystack_[3].value.lval);
									driver.add_array_decl(n, s, t, false, (yystack_[0].value.lval));
									FREE_PTR((yystack_[6].value.str));
									FREE_PTR((yystack_[4].value.str));
								}
#line 807 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 348 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", (yystack_[6].value.str), (yystack_[4].value.str), (yystack_[3].value.lval));
									debug_r("varname=%s nullable=%d", (yystack_[4].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[6].value.str));
									std::string n((yystack_[4].value.str));
									int s = (yystack_[3].value.lval);
									driver.add_array_decl(n, s, t, true, (yystack_[0].value.lval));
									FREE_PTR((yystack_[6].value.str));
									FREE_PTR((yystack_[4].value.str));
	
								}
#line 823 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 362 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_list_decl(n, t, false, (yystack_[0].value.lval));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));

								}
#line 838 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 375 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_list_decl(n, t, true, (yystack_[0].value.lval));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));	
								}
#line 852 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 52:
#line 386 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 868 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 53:
#line 400 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 884 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 54:
#line 414 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	(yylhs.value.lval) = 0; }
#line 890 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 55:
#line 415 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	(yylhs.value.lval) = 1; driver.set_nullable(); }
#line 896 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 420 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							debug_r("Defualt : %s", (yystack_[0].value.str));
							std::string v((yystack_[0].value.str));
							driver.set_default_value(v);
							FREE_PTR((yystack_[0].value.str));
						}
#line 907 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 432 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 916 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 61:
#line 436 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 931 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 62:
#line 450 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("varaible=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 937 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 63:
#line 454 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 943 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 455 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 949 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 456 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 955 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 457 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 961 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 458 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 967 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 459 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 973 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 69:
#line 460 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 979 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 70:
#line 461 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 985 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 71:
#line 462 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 991 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 72:
#line 463 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 997 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 464 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1003 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 74:
#line 465 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1009 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 75:
#line 469 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("size=%d", (yystack_[1].value.lval)); (yylhs.value.lval)=(yystack_[1].value.lval); }
#line 1015 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 76:
#line 472 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.finish_def(); }
#line 1021 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 78:
#line 478 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1032 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 79:
#line 484 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1043 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 80:
#line 490 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1054 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 81:
#line 496 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1065 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 82:
#line 502 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1076 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 83:
#line 508 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1087 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 84:
#line 514 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1098 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 85:
#line 520 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1109 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 86:
#line 526 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1120 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 87:
#line 532 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1131 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 88:
#line 539 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1142 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 89:
#line 545 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1153 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 90:
#line 551 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1164 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 91:
#line 557 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1175 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 92:
#line 566 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1190 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 93:
#line 576 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1206 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 94:
#line 591 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%ld", (yystack_[0].value.lval));
					(yylhs.value.str) = ptr; 
				}
#line 1218 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 95:
#line 598 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%f", (yystack_[0].value.dval));
					(yylhs.value.str) = ptr; 
				}
#line 1230 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 96:
#line 605 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("String value [%s]", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1236 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;


#line 1240 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
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


  const short int schema_parser::yypact_ninf_ = -147;

  const signed char schema_parser::yytable_ninf_ = -1;

  const short int
  schema_parser::yypact_[] =
  {
     -39,    14,    40,    35,  -147,  -147,   -29,  -147,  -147,    14,
       8,    35,  -147,    41,  -147,    14,  -147,    14,    24,    48,
    -147,   128,  -147,  -147,    14,  -147,    24,  -147,    55,   128,
      14,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,    42,    43,    47,     4,  -147,  -147,  -147,
    -147,  -147,  -147,  -147,  -147,  -147,    14,    61,  -147,    14,
      15,    18,    14,   103,   154,   179,    60,   128,    59,    14,
      19,  -147,   -35,  -147,  -147,  -147,    60,  -147,    69,    59,
      14,    64,    14,    79,    74,  -147,  -147,  -147,   104,    97,
    -147,  -147,  -147,    14,  -147,  -147,  -147,  -147,  -147,   105,
      60,  -147,    97,    87,    14,    88,    14,   167,    -8,    73,
      93,  -147,    14,  -147,    93,    14,    77,    14,    59,    14,
     107,   106,   125,   126,   -21,    73,    73,    73,    73,   131,
    -147,  -147,  -147,  -147,  -147,  -147,    25,  -147,    77,   151,
      59,    59,    97,   114,    14,   141,    73,    73,   150,   173,
     174,    73,    73,    73,    73,  -147,  -147,  -147,  -147,  -147,
    -147,    59,   175,    97,    97,    93,    14,    59,  -147,    21,
    -147,    27,   184,    73,    73,  -147,  -147,  -147,  -147,    97,
    -147,    93,    93,  -147,    59,    97,  -147,    73,  -147,  -147,
      32,    50,    93,  -147,  -147,    97,    93,  -147,  -147,  -147,
    -147,    93,  -147,  -147
  };

  const unsigned char
  schema_parser::yydefact_[] =
  {
       5,     0,     0,    30,     6,    62,     0,    60,     1,     0,
       0,    31,    32,     0,     7,     0,    35,     0,     8,     0,
      33,     0,    61,     4,     0,     2,     9,    10,     0,     0,
       0,    64,    65,    66,    67,    68,    69,    70,    63,    71,
      72,    74,    73,     0,     0,     0,     0,    36,    38,    39,
      40,    41,    42,    43,    44,    45,     0,     0,    11,     0,
      14,     0,     0,     0,     0,     0,     0,     0,    58,     0,
       0,    23,    26,    16,    17,    18,     0,    15,    20,    58,
       0,     0,     0,     0,     0,    76,    34,    37,     0,    56,
      59,    19,    13,     0,    28,    29,    25,    27,    12,     0,
       0,    21,    56,     0,     0,     0,     0,     0,     0,     0,
      54,    24,     0,     3,    54,     0,     0,     0,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      96,    94,    95,    57,    55,    46,     0,    47,     0,     0,
      58,    58,    56,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    88,    86,    90,    77,
      22,    58,     0,    56,    56,    54,     0,    58,    78,     0,
      92,     0,     0,     0,     0,    85,    89,    87,    91,    56,
      75,    54,    54,    50,    58,    56,    80,     0,    82,    79,
       0,     0,    54,    48,    51,    56,    54,    93,    81,    83,
      49,    54,    52,    53
  };

  const short int
  schema_parser::yypgoto_[] =
  {
    -147,  -147,  -147,  -147,  -147,  -147,  -147,  -147,   180,  -147,
    -147,  -147,  -147,  -147,  -147,    95,   112,  -147,  -147,  -147,
    -147,   197,  -147,   181,   142,  -147,  -147,  -147,  -147,  -147,
    -147,  -147,  -147,  -107,   -97,   -77,   210,    -9,   -53,    75,
     -67,  -147,  -147,  -146,   -26
  };

  const short int
  schema_parser::yydefgoto_[] =
  {
      -1,     2,    18,    19,     3,     4,    25,    26,    27,    60,
      76,    77,    28,   100,   101,    70,    71,    96,    97,    10,
      11,    12,    13,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,   135,   110,    89,    72,     7,    56,   140,
      86,    90,   129,   169,   170
  };

  const unsigned char
  schema_parser::yytable_[] =
  {
      16,   171,   102,     1,    14,   114,    22,   137,    23,    98,
      81,    83,    84,    94,    95,    57,    15,   148,   149,   150,
       5,    62,    15,   151,   152,   153,   154,   190,   191,    66,
     121,   122,   123,   113,    67,   124,   125,   126,   127,   128,
       8,   142,     9,    78,    17,   165,    92,    68,    67,    93,
     186,   187,   160,    79,   120,    93,   188,   187,   183,    24,
      91,   198,   187,   163,   164,    21,   181,   182,    73,    74,
      75,   103,    29,   105,   193,   194,   130,   131,   132,   199,
     187,    59,   192,   133,   179,   200,    63,    64,   196,   202,
     185,    65,    69,    85,   203,   116,    88,   118,   201,   155,
     156,   157,   158,    99,   107,   139,   138,   195,   141,   104,
     143,    80,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,   106,   175,   176,   177,   178,   109,
     108,   112,   115,   117,   134,   167,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,   144,   146,   147,   162,   145,   184,   159,   166,
     168,   197,    82,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,   119,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
     172,   173,   174,   189,   180,   111,    58,   136,    20,    87,
      61,     6,     0,   161
  };

  const short int
  schema_parser::yycheck_[] =
  {
       9,   147,    79,    42,    33,   102,    15,   114,    17,    76,
      63,    64,    65,    48,    49,    24,    51,    38,    39,    40,
       6,    30,    51,    44,    45,    46,    47,   173,   174,    25,
      38,    39,    40,   100,    30,    43,    44,    45,    46,    47,
       0,   118,     7,    25,    36,   142,    27,    56,    30,    30,
      29,    30,    27,    62,   107,    30,    29,    30,   165,    35,
      69,    29,    30,   140,   141,    24,   163,   164,    53,    54,
      55,    80,    24,    82,   181,   182,     3,     4,     5,    29,
      30,    26,   179,   109,   161,   192,    44,    44,   185,   196,
     167,    44,    31,    33,   201,   104,    37,   106,   195,   125,
     126,   127,   128,    34,    30,    28,   115,   184,   117,    45,
     119,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    45,   151,   152,   153,   154,    32,
      26,    26,    45,    45,    41,   144,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    45,    28,    28,     4,    50,   166,    27,    45,
      19,   187,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      50,    28,    28,    19,    29,    93,    26,   112,    11,    67,
      29,     1,    -1,   138
  };

  const unsigned char
  schema_parser::yystos_[] =
  {
       0,    42,    57,    60,    61,     6,    92,    93,     0,     7,
      75,    76,    77,    78,    33,    51,    93,    36,    58,    59,
      77,    24,    93,    93,    35,    62,    63,    64,    68,    24,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    94,    93,    64,    26,
      65,    79,    93,    44,    44,    44,    25,    30,    93,    31,
      71,    72,    92,    53,    54,    55,    66,    67,    25,    93,
       8,    94,     8,    94,    94,    33,    96,    80,    37,    91,
      97,    93,    27,    30,    48,    49,    73,    74,    96,    34,
      69,    70,    91,    93,    45,    93,    45,    30,    26,    32,
      90,    72,    26,    96,    90,    45,    93,    45,    93,     8,
      94,    38,    39,    40,    43,    44,    45,    46,    47,    98,
       3,     4,     5,   100,    41,    89,    71,    89,    93,    28,
      95,    93,    91,    93,    45,    50,    28,    28,    38,    39,
      40,    44,    45,    46,    47,   100,   100,   100,   100,    27,
      27,    95,     4,    91,    91,    90,    45,    93,    19,    99,
     100,    99,    50,    28,    28,   100,   100,   100,   100,    91,
      29,    90,    90,    89,    93,    91,    29,    30,    29,    19,
      99,    99,    90,    89,    89,    91,    90,   100,    29,    29,
      89,    90,    89,    89
  };

  const unsigned char
  schema_parser::yyr1_[] =
  {
       0,    56,    57,    58,    59,    60,    60,    61,    62,    62,
      63,    63,    64,    65,    66,    66,    67,    67,    67,    68,
      69,    69,    70,    71,    71,    72,    73,    73,    74,    74,
      75,    75,    76,    76,    77,    78,    79,    79,    80,    80,
      80,    80,    80,    80,    80,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    89,    90,    90,    91,    91,
      92,    92,    93,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    95,    96,    97,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    99,    99,   100,   100,   100
  };

  const unsigned char
  schema_parser::yyr2_[] =
  {
       0,     2,     4,     6,     2,     0,     1,     3,     0,     1,
       1,     2,     4,     3,     0,     1,     1,     1,     1,     4,
       0,     1,     4,     1,     3,     2,     0,     1,     1,     1,
       0,     1,     1,     2,     5,     2,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     6,     9,    10,
       8,     9,    10,    11,     0,     1,     0,     2,     0,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     4,     3,     4,
       4,     5,     4,     5,     2,     3,     2,     3,     2,     3,
       2,     3,     1,     3,     1,     1,     1
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
  "RINBRACE", "LSZBRACE", "RSZBRACE", "COMMA", "ON", "DEFAULT", "TYPE_END",
  "KEY_FIELDS", "INDEX", "SCHEMA", "CONSTRAINT", "REGEX", "IN", "BETWEEN",
  "NULLABLE", "NAMESPACE", "NOT", "LT", "GT", "LTEQ", "GTEQ", "ASC",
  "DESC", "COLON", "DOT", "NEWLINE", "FULLTEXT_INDEX", "HASH_INDEX",
  "TREE_INDEX", "$accept", "parse", "schema", "schema_declare",
  "opt_namespace", "namespace", "opt_indexes", "indexes", "index",
  "index_fields", "opt_index_type", "index_type", "index_declare",
  "opt_key_fields", "key_fields", "columns", "column", "opt_sort", "sort",
  "opt_types", "types", "type", "type_declare", "declarations", "declare",
  "declare_native", "declare_ref", "declare_native_arr", "declare_ref_arr",
  "declare_native_list", "declare_ref_list", "declare_netive_map",
  "declare_ref_map", "opt_nullable", "opt_default", "opt_constraint",
  "nested_variable", "variable", "datatype", "size_def", "declare_finish",
  "constraint", "constraint_type", "values", "value", YY_NULLPTR
  };


  const unsigned short int
  schema_parser::yyrline_[] =
  {
       0,   133,   133,   137,   141,   147,   149,   153,   161,   163,
     167,   168,   172,   178,   185,   187,   191,   192,   193,   197,
     207,   209,   213,   222,   225,   239,   258,   261,   268,   271,
     276,   278,   282,   283,   286,   290,   297,   298,   303,   304,
     305,   306,   307,   308,   309,   310,   314,   325,   335,   348,
     362,   375,   386,   400,   414,   415,   418,   420,   426,   428,
     432,   436,   450,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   469,   472,   475,   478,   484,
     490,   496,   502,   508,   514,   520,   526,   532,   539,   545,
     551,   557,   566,   576,   591,   598,   605
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55
    };
    const unsigned int user_token_number_max_ = 310;
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
#line 1719 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:1167
#line 609 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1168


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
