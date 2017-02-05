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

#line 37 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "schema_tokens.h"

// User implementation prologue.

#line 51 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:412
// Unqualified %code blocks.
#line 44 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:413

#include "schema_driver.h"

#undef yylex
#define yylex scanner.yylex

void debug_r(const char *s, ...);

#line 62 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:413


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

#line 5 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:479
namespace com { namespace wookler { namespace reactfs { namespace core { namespace parsers {
#line 148 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:479

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
#line 140 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new schema [%s]", ss.c_str());
							driver.create_schema(ss);
							FREE_PTR((yystack_[0].value.str));
						}
#line 596 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 152 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[1].value.str));
							debug_r("namespace [%s]", ss.c_str());
							driver.set_namespace(ss);
							FREE_PTR((yystack_[1].value.str));

						}
#line 608 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 171 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {

						}
#line 616 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 177 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_index_fields(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 627 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 16:
#line 190 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 633 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 17:
#line 191 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 639 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 18:
#line 192 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.set_index_type((yystack_[0].value.str)); }
#line 645 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 196 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string n((yystack_[2].value.str));
							std::string s((yystack_[0].value.str));
							debug_r("Creating index [%s] on schema [%s]", n.c_str(), s.c_str());
							driver.create_index(n, s);
							FREE_PTR((yystack_[2].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 658 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 22:
#line 212 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_primary_key(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 669 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 23:
#line 221 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 677 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 24:
#line 224 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 694 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 25:
#line 238 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 715 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 26:
#line 257 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = nullptr;
						}
#line 723 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 27:
#line 260 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 732 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 28:
#line 267 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 740 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 29:
#line 270 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 748 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 285 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
											
										}
#line 756 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 289 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new type [%s]", ss.c_str());
							driver.add_type(ss); 
							FREE_PTR((yystack_[0].value.str));
						}
#line 767 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 311 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s nullable=%d", (yystack_[4].value.str), (yystack_[3].value.str), (yystack_[0].value.lval));
							std::string t((yystack_[4].value.str));
							std::string n((yystack_[3].value.str));
							driver.add_declaration(n, t, false, (yystack_[0].value.lval)); 
							FREE_PTR((yystack_[4].value.str));
							FREE_PTR((yystack_[3].value.str));
						}
#line 780 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 322 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yystack_[4].value.str), (yystack_[3].value.str));
							debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
							std::string t((yystack_[4].value.str));
							std::string n((yystack_[3].value.str));
							driver.add_declaration(n, t, true, (yystack_[0].value.lval)); 
						}
#line 792 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 332 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_list_decl(n, t, false, (yystack_[0].value.lval));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));

								}
#line 807 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 345 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[5].value.str), (yystack_[3].value.str));
									debug_r("varname=%s nullable=%d", (yystack_[3].value.str), (yystack_[0].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									driver.add_list_decl(n, t, true, (yystack_[0].value.lval));
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));	
								}
#line 821 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 356 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 837 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 370 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 853 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 384 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	(yylhs.value.lval) = 0; }
#line 859 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 385 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	(yylhs.value.lval) = 1; driver.set_nullable(); }
#line 865 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 53:
#line 390 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							debug_r("Defualt : %s", (yystack_[0].value.str));
							std::string v((yystack_[0].value.str));
							driver.set_default_value(v);
							FREE_PTR((yystack_[0].value.str));
						}
#line 876 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 56:
#line 402 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 885 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 406 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 900 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 58:
#line 420 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("varaible=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 906 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 59:
#line 424 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 912 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 425 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 918 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 61:
#line 426 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 924 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 62:
#line 427 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 930 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 63:
#line 428 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 936 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 429 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 942 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 430 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 948 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 431 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 954 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 432 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 960 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 433 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 966 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 69:
#line 434 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 972 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 70:
#line 435 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 981 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 71:
#line 443 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							debug_r("datatype=%s size=%d", (yystack_[3].value.str), (yystack_[1].value.lval));
							int s = sizeof(char) * 128;
							char *buff = (char *)malloc(s);
							memset(buff, 0, s);
							sprintf(buff, "%s(%d)", (yystack_[3].value.str), (int)(yystack_[1].value.lval));
							FREE_PTR((yystack_[3].value.str));
							(yylhs.value.str) = buff;
						}
#line 995 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 72:
#line 455 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1001 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 459 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.finish_def(); }
#line 1007 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 75:
#line 465 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1018 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 76:
#line 471 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1029 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 77:
#line 477 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1040 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 78:
#line 483 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1051 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 79:
#line 489 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1062 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 80:
#line 495 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1073 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 81:
#line 501 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1084 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 82:
#line 507 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1095 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 83:
#line 513 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1106 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 84:
#line 519 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1117 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 85:
#line 526 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1128 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 86:
#line 532 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1139 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 87:
#line 538 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1150 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 88:
#line 544 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GTEQ"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 1161 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 89:
#line 553 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1176 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 90:
#line 563 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1192 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 91:
#line 578 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%ld", (yystack_[0].value.lval));
					(yylhs.value.str) = ptr; 
				}
#line 1204 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 92:
#line 585 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%f", (yystack_[0].value.dval));
					(yylhs.value.str) = ptr; 
				}
#line 1216 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 93:
#line 592 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("String value [%s]", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1222 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;


#line 1226 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
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


  const short int schema_parser::yypact_ninf_ = -139;

  const signed char schema_parser::yytable_ninf_ = -1;

  const short int
  schema_parser::yypact_[] =
  {
     -27,    16,    38,    21,  -139,  -139,   -21,  -139,  -139,    16,
      14,    21,  -139,    30,  -139,    16,  -139,    16,    26,    55,
    -139,   120,  -139,  -139,    16,  -139,    26,  -139,    37,   120,
      16,  -139,  -139,  -139,  -139,  -139,  -139,  -139,  -139,  -139,
    -139,  -139,  -139,     8,    36,   -17,  -139,  -139,  -139,  -139,
    -139,  -139,  -139,    16,  -139,    44,    51,  -139,    16,     2,
     -11,    16,   139,   164,    50,   120,    47,    81,    16,    -6,
    -139,   -31,  -139,  -139,  -139,    50,  -139,    67,    47,    16,
      57,    73,  -139,  -139,  -139,    79,    75,  -139,    82,  -139,
    -139,    16,  -139,  -139,  -139,  -139,  -139,    84,    50,  -139,
      75,    63,    16,   152,    53,    28,    70,  -139,  -139,    16,
    -139,    70,    16,    47,    16,    68,    62,    86,    87,    -3,
      28,    28,    28,    28,    89,  -139,  -139,  -139,  -139,  -139,
    -139,     1,  -139,    47,    75,    74,    16,    98,    28,    28,
      71,    92,    94,    28,    28,    28,    28,  -139,  -139,  -139,
    -139,  -139,  -139,    75,    70,    16,    47,  -139,    17,  -139,
      39,   103,    28,    28,  -139,  -139,  -139,  -139,    70,  -139,
      47,    75,  -139,    28,  -139,  -139,    43,    46,  -139,    75,
      70,  -139,  -139,  -139,    70,  -139,  -139
  };

  const unsigned char
  schema_parser::yydefact_[] =
  {
       5,     0,     0,    30,     6,    58,     0,    56,     1,     0,
       0,    31,    32,     0,     7,     0,    35,     0,     8,     0,
      33,     0,    57,     4,     0,     2,     9,    10,     0,     0,
       0,    60,    61,    62,    63,    64,    65,    66,    59,    67,
      68,    72,    69,     0,     0,     0,    36,    38,    39,    40,
      41,    42,    43,     0,    70,     0,     0,    11,     0,    14,
       0,     0,     0,     0,     0,     0,    54,     0,     0,     0,
      23,    26,    16,    17,    18,     0,    15,    20,    54,     0,
       0,     0,    73,    34,    37,     0,    52,    55,     0,    19,
      13,     0,    28,    29,    25,    27,    12,     0,     0,    21,
      52,     0,     0,     0,     0,     0,    50,    71,    24,     0,
       3,    50,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    93,    91,    92,    53,    51,
      44,     0,    45,    54,    52,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    81,    85,    83,
      87,    74,    22,    52,    50,     0,    54,    75,     0,    89,
       0,     0,     0,     0,    82,    86,    84,    88,    50,    46,
      54,    52,    77,     0,    79,    76,     0,     0,    47,    52,
      50,    90,    78,    80,    50,    48,    49
  };

  const short int
  schema_parser::yypgoto_[] =
  {
    -139,  -139,  -139,  -139,  -139,  -139,  -139,  -139,    97,  -139,
    -139,  -139,  -139,  -139,  -139,    15,    34,  -139,  -139,  -139,
    -139,   115,  -139,   114,    80,  -139,  -139,  -139,  -139,  -139,
    -139,  -107,   -95,   -76,   143,    -9,   -53,  -139,  -139,   -72,
    -139,  -139,  -138,   -57
  };

  const short int
  schema_parser::yydefgoto_[] =
  {
      -1,     2,    18,    19,     3,     4,    25,    26,    27,    59,
      75,    76,    28,    98,    99,    69,    70,    94,    95,    10,
      11,    12,    13,    45,    46,    47,    48,    49,    50,    51,
      52,   130,   106,    86,    71,     7,    53,    54,    55,    83,
      87,   124,   158,   159
  };

  const unsigned char
  schema_parser::yytable_[] =
  {
      16,   160,   100,    96,   132,   111,    22,    64,    23,    80,
      81,    14,    65,    77,     1,    56,    92,    93,    65,    15,
      90,    61,     5,    91,   176,   177,   110,   152,     9,    15,
      91,   125,   126,   127,   140,   141,   142,   134,     8,   154,
     143,   144,   145,   146,    66,   172,   173,   169,   128,    17,
     115,    62,    78,    21,    72,    73,    74,   153,   168,    89,
      24,   178,    58,   147,   148,   149,   150,   174,   173,    67,
     101,   182,   173,   185,   183,   173,   180,   186,    29,    63,
     171,    68,    82,    85,   184,    88,   164,   165,   166,   167,
     116,   117,   118,   113,   179,   119,   120,   121,   122,   123,
      97,   102,   103,   133,   104,   135,   105,   112,   107,   109,
     129,   137,   136,   138,   139,   151,   181,   157,   155,   162,
     161,   163,   175,    57,   131,   108,    20,   156,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    60,     6,    84,   170,    79,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
     114,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42
  };

  const unsigned char
  schema_parser::yycheck_[] =
  {
       9,   139,    78,    75,   111,   100,    15,    24,    17,    62,
      63,    32,    29,    24,    41,    24,    47,    48,    29,    50,
      26,    30,     6,    29,   162,   163,    98,    26,     7,    50,
      29,     3,     4,     5,    37,    38,    39,   113,     0,   134,
      43,    44,    45,    46,    53,    28,    29,   154,   105,    35,
     103,    43,    61,    23,    52,    53,    54,   133,   153,    68,
      34,   168,    25,   120,   121,   122,   123,    28,    29,    25,
      79,    28,    29,   180,    28,    29,   171,   184,    23,    43,
     156,    30,    32,    36,   179,     4,   143,   144,   145,   146,
      37,    38,    39,   102,   170,    42,    43,    44,    45,    46,
      33,    44,    29,   112,    25,   114,    31,    44,    26,    25,
      40,    49,    44,    27,    27,    26,   173,    19,    44,    27,
      49,    27,    19,    26,   109,    91,    11,   136,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    29,     1,    65,   155,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20
  };

  const unsigned char
  schema_parser::yystos_[] =
  {
       0,    41,    56,    59,    60,     6,    89,    90,     0,     7,
      74,    75,    76,    77,    32,    50,    90,    35,    57,    58,
      76,    23,    90,    90,    34,    61,    62,    63,    67,    23,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    78,    79,    80,    81,    82,
      83,    84,    85,    91,    92,    93,    90,    63,    25,    64,
      78,    90,    43,    43,    24,    29,    90,    25,    30,    70,
      71,    89,    52,    53,    54,    65,    66,    24,    90,     8,
      91,    91,    32,    94,    79,    36,    88,    95,     4,    90,
      26,    29,    47,    48,    72,    73,    94,    33,    68,    69,
      88,    90,    44,    29,    25,    31,    87,    26,    71,    25,
      94,    87,    44,    90,     8,    91,    37,    38,    39,    42,
      43,    44,    45,    46,    96,     3,     4,     5,    98,    40,
      86,    70,    86,    90,    88,    90,    44,    49,    27,    27,
      37,    38,    39,    43,    44,    45,    46,    98,    98,    98,
      98,    26,    26,    88,    87,    44,    90,    19,    97,    98,
      97,    49,    27,    27,    98,    98,    98,    98,    87,    86,
      90,    88,    28,    29,    28,    19,    97,    97,    86,    88,
      87,    98,    28,    28,    87,    86,    86
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
      91,    92,    93,    94,    95,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    97,
      97,    98,    98,    98
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
       1,     4,     1,     1,     4,     3,     4,     4,     5,     4,
       5,     2,     3,     2,     3,     2,     3,     2,     3,     1,
       3,     1,     1,     1
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
  "variable", "datatype", "datatype_string", "string_decl",
  "declare_finish", "constraint", "constraint_type", "values", "value", YY_NULLPTR
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
     435,   442,   455,   459,   462,   465,   471,   477,   483,   489,
     495,   501,   507,   513,   519,   526,   532,   538,   544,   553,
     563,   578,   585,   592
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

#line 5 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1167
} } } } } // com::wookler::reactfs::core::parsers
#line 1692 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:1167
#line 596 "/home/subho/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1168


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
