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
#line 133 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new schema [%s]", ss.c_str());
							driver.create_schema(ss);
							FREE_PTR((yystack_[0].value.str));
						}
#line 596 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 7:
#line 146 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
								debug_r("KEY FIELDS: (%s)", (yystack_[1].value.str));
								std::string keys((yystack_[1].value.str));
								driver.set_primary_key(keys);
								FREE_PTR((yystack_[1].value.str));
							}
#line 607 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 8:
#line 155 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 615 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 9:
#line 158 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 632 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 10:
#line 172 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 653 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 11:
#line 191 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = nullptr;
						}
#line 661 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 12:
#line 194 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							(yylhs.value.str) = strdup((yystack_[0].value.str));
							FREE_PTR((yystack_[0].value.str));
						}
#line 670 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 13:
#line 201 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 678 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 14:
#line 204 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							(yylhs.value.str) = strdup((yystack_[0].value.str));
						}
#line 686 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 19:
#line 219 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
											
										}
#line 694 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 20:
#line 223 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							std::string ss((yystack_[0].value.str));
							debug_r("new type [%s]", ss.c_str());
							driver.add_type(ss); 
							FREE_PTR((yystack_[0].value.str));
						}
#line 705 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 31:
#line 247 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yystack_[3].value.str), (yystack_[2].value.str));
							std::string t((yystack_[3].value.str));
							std::string n((yystack_[2].value.str));
							driver.add_declaration(n, t); 
							FREE_PTR((yystack_[3].value.str));
							FREE_PTR((yystack_[2].value.str));
						}
#line 718 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 32:
#line 258 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
							debug_r("[type=%s] varname=%s", (yystack_[3].value.str), (yystack_[2].value.str));
							std::string t((yystack_[3].value.str));
							std::string n((yystack_[2].value.str));
							driver.add_declaration(n, t, true); 
						}
#line 729 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 33:
#line 267 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", (yystack_[5].value.str), (yystack_[3].value.str), (yystack_[2].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									int s = (yystack_[2].value.lval);
									driver.add_array_decl(n, s, t, false);
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));
								}
#line 743 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 34:
#line 279 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[ARRAY] inner type=%s varname=%s size=%d", (yystack_[5].value.str), (yystack_[3].value.str), (yystack_[2].value.lval));
									std::string t((yystack_[5].value.str));
									std::string n((yystack_[3].value.str));
									int s = (yystack_[2].value.lval);
									driver.add_array_decl(n, s, t, true);
									FREE_PTR((yystack_[5].value.str));
									FREE_PTR((yystack_[3].value.str));
	
								}
#line 758 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 35:
#line 292 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[4].value.str), (yystack_[2].value.str));
									std::string t((yystack_[4].value.str));
									std::string n((yystack_[2].value.str));
									driver.add_list_decl(n, t, false);
									FREE_PTR((yystack_[4].value.str));
									FREE_PTR((yystack_[2].value.str));

								}
#line 772 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 36:
#line 304 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[LIST] inner type=%s varname=%s", (yystack_[4].value.str), (yystack_[2].value.str));
									std::string t((yystack_[4].value.str));
									std::string n((yystack_[2].value.str));
									driver.add_list_decl(n, t, true);
									FREE_PTR((yystack_[4].value.str));
									FREE_PTR((yystack_[2].value.str));	
								}
#line 785 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 37:
#line 314 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", (yystack_[6].value.str), (yystack_[4].value.str), (yystack_[2].value.str));
									std::string kt((yystack_[6].value.str));
									std::string vt((yystack_[4].value.str));
									std::string n((yystack_[2].value.str));
									driver.add_map_decl(n, kt, vt, false);
									FREE_PTR((yystack_[6].value.str));
									FREE_PTR((yystack_[4].value.str));
									FREE_PTR((yystack_[2].value.str));
								}
#line 800 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 38:
#line 327 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
									debug_r("type=[MAP] key type=%s value type=%s varname=%s", (yystack_[7].value.str), (yystack_[4].value.str), (yystack_[2].value.str));
									std::string kt((yystack_[7].value.str));
									std::string vt((yystack_[4].value.str));
									std::string n((yystack_[2].value.str));
									driver.add_map_decl(n, kt, vt, true);
									FREE_PTR((yystack_[7].value.str));
									FREE_PTR((yystack_[4].value.str));
									FREE_PTR((yystack_[2].value.str));
								}
#line 815 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 40:
#line 340 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    {
							debug_r("Defualt : %s", (yystack_[0].value.str));
							std::string v((yystack_[0].value.str));
							driver.set_default_value(v);
							FREE_PTR((yystack_[0].value.str));
						}
#line 826 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 43:
#line 353 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("varaible=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 832 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 44:
#line 357 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 838 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 45:
#line 358 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 844 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 46:
#line 359 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 850 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 47:
#line 360 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 856 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 48:
#line 361 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 862 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 49:
#line 362 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 868 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 50:
#line 363 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 874 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 51:
#line 364 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 880 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 52:
#line 365 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 886 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 53:
#line 366 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 892 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 54:
#line 367 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 898 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 55:
#line 368 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("datatype=%s", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 904 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 56:
#line 372 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("size=%d", (yystack_[1].value.lval)); (yylhs.value.lval)=(yystack_[1].value.lval); }
#line 910 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 57:
#line 375 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { driver.finish_def(); }
#line 916 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 59:
#line 381 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 927 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 60:
#line 387 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("REGEX"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[0].value.str));
									FREE_PTR((yystack_[0].value.str));
								}
#line 938 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 61:
#line 393 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 949 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 62:
#line 399 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("IN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 960 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 63:
#line 405 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 971 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 64:
#line 411 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("BETWEEN"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 982 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 65:
#line 417 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 993 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 66:
#line 423 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("LT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1004 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 67:
#line 429 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(false, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1015 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 68:
#line 435 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 
									const std::string p("GT"); 
									debug_r("Constraint %s", p.c_str()); 
									driver.set_constraint(true, p, (yystack_[1].value.str));
									FREE_PTR((yystack_[1].value.str));
								}
#line 1026 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 69:
#line 444 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1041 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 70:
#line 454 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
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
#line 1057 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 71:
#line 469 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%ld", (yystack_[0].value.lval));
					(yylhs.value.str) = ptr; 
				}
#line 1069 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 72:
#line 476 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { 	
					char *ptr = (char *)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
					CHECK_ALLOC(ptr, TYPE_NAME(char));
					memset(ptr, 0, DEFAULT_BUFFER_SIZE);
					sprintf(ptr, "%f", (yystack_[0].value.dval));
					(yylhs.value.str) = ptr; 
				}
#line 1081 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;

  case 73:
#line 483 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:859
    { debug_r("String value [%s]", (yystack_[0].value.str)); (yylhs.value.str) = strdup((yystack_[0].value.str)); }
#line 1087 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
    break;


#line 1091 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:859
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


  const signed char schema_parser::yypact_ninf_ = -105;

  const signed char schema_parser::yytable_ninf_ = -1;

  const short int
  schema_parser::yypact_[] =
  {
      -3,    13,    21,    -8,    -3,  -105,     6,  -105,  -105,  -105,
      13,  -105,    17,  -105,    98,  -105,    98,    13,  -105,  -105,
    -105,  -105,  -105,  -105,  -105,  -105,  -105,  -105,  -105,  -105,
      18,    19,    22,   -19,  -105,  -105,  -105,  -105,  -105,  -105,
    -105,  -105,  -105,    13,   -18,    13,   121,   134,   159,    20,
      98,    26,    23,    26,    13,    25,    13,    28,    33,  -105,
    -105,  -105,    31,    34,  -105,    40,    20,  -105,    34,    37,
      13,    39,    13,   147,   -16,    42,  -105,    13,  -105,  -105,
      13,    45,    13,    26,    13,    43,    29,    47,    52,    48,
      56,    62,    66,  -105,  -105,  -105,  -105,   -24,  -105,   -11,
      45,    92,    26,    26,    34,    67,    13,    80,    42,    42,
      53,    73,    74,    75,    76,    42,    42,  -105,  -105,    13,
    -105,  -105,  -105,  -105,    26,    93,    34,    34,  -105,    13,
      26,  -105,   -14,  -105,   -12,   104,    42,    42,    42,    42,
      95,    96,  -105,    34,  -105,  -105,  -105,    26,    34,  -105,
      42,  -105,  -105,     7,    11,    97,   151,  -105,  -105,  -105,
      34,  -105,  -105,  -105,  -105,  -105,  -105,  -105
  };

  const unsigned char
  schema_parser::yydefact_[] =
  {
      15,     0,     0,     0,    16,    17,     0,    43,    20,     1,
       0,     2,     0,    18,     0,     4,     0,     0,    45,    46,
      47,    48,    49,    50,    51,    44,    52,    53,    55,    54,
       0,     0,     0,     0,    21,    23,    24,    25,    26,    27,
      28,    29,    30,     0,     0,     0,     0,     0,     0,     0,
       0,    41,     5,    41,     0,     0,     0,     0,     0,    57,
      19,    22,     0,    39,    42,     0,     0,     6,    39,     0,
       0,     0,     0,     0,     0,     0,    31,     0,     3,    32,
       0,     0,     0,    41,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    73,    71,    72,    40,     0,     8,    11,
       0,     0,    41,    41,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    58,     7,     0,
      13,    14,    10,    12,    41,     0,    39,    39,    35,     0,
      41,    59,     0,    69,     0,     0,     0,     0,     0,     0,
       0,     0,     9,    39,    56,    33,    36,    41,    39,    61,
       0,    63,    60,     0,     0,     0,     0,    65,    67,    34,
      39,    37,    70,    62,    64,    66,    68,    38
  };

  const short int
  schema_parser::yypgoto_[] =
  {
    -105,  -105,  -105,  -105,  -105,  -105,  -105,     8,  -105,  -105,
    -105,  -105,   177,  -105,   166,   133,  -105,  -105,  -105,  -105,
    -105,  -105,  -105,  -105,   -66,   -52,    -1,   -36,    84,   119,
    -105,  -105,  -104,   -53
  };

  const short int
  schema_parser::yydefgoto_[] =
  {
      -1,     2,    11,    12,    66,    67,    97,    98,   122,   123,
       3,     4,     5,     6,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    76,    63,    99,    43,   102,    60,
      64,    92,   132,   133
  };

  const unsigned char
  schema_parser::yytable_[] =
  {
       8,    68,    79,   118,     1,   134,    49,    52,   119,    15,
      55,    57,    58,    50,    50,   149,    45,   151,   150,     7,
     150,     9,    96,    86,    87,    88,    89,    90,    91,    10,
      14,   104,   153,   154,   120,   121,   163,    85,   128,   150,
     164,    16,    51,   150,    53,    93,    94,    95,    46,    47,
     126,   127,    48,    69,    59,    71,    70,    74,    65,    72,
     145,   146,   140,   141,    62,    73,    77,    75,    80,    81,
      82,    83,   143,   101,   106,   108,   107,   159,   148,   100,
     109,   103,   161,   105,   115,   155,   156,   110,   111,   112,
     116,   113,   114,   117,   167,   160,   125,   162,   129,   131,
     135,   136,   137,   138,   139,   130,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,   144,   152,   157,   158,   165,   142,   147,    54,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    56,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    84,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
     166,    13,    44,    61,   124,    78
  };

  const unsigned char
  schema_parser::yycheck_[] =
  {
       1,    53,    68,    27,     7,   109,    25,    25,    32,    10,
      46,    47,    48,    32,    32,    29,    17,    29,    32,     6,
      32,     0,    75,    39,    40,    41,    42,    43,    44,    37,
      24,    83,   136,   137,    45,    46,    29,    73,   104,    32,
      29,    24,    43,    32,    45,     3,     4,     5,    30,    30,
     102,   103,    30,    54,    34,    56,    31,    26,    35,    31,
     126,   127,   115,   116,    38,    32,    26,    33,    31,    70,
      31,    72,   124,    28,    31,    28,    47,   143,   130,    80,
      28,    82,   148,    84,    28,   138,   139,    39,    40,    41,
      28,    43,    44,    27,   160,   147,     4,   150,    31,    19,
      47,    28,    28,    28,    28,   106,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    29,    19,    29,    29,    29,   119,   129,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      29,     4,    16,    50,   100,    66
  };

  const unsigned char
  schema_parser::yystos_[] =
  {
       0,     7,    50,    59,    60,    61,    62,     6,    75,     0,
      37,    51,    52,    61,    24,    75,    24,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    76,    63,    75,    30,    30,    30,    25,
      32,    75,    25,    75,     8,    76,     8,    76,    76,    34,
      78,    64,    38,    74,    79,    35,    53,    54,    74,    75,
      31,    75,    31,    32,    26,    33,    73,    26,    78,    73,
      31,    75,    31,    75,     8,    76,    39,    40,    41,    42,
      43,    44,    80,     3,     4,     5,    82,    55,    56,    75,
      75,    28,    77,    75,    74,    75,    31,    47,    28,    28,
      39,    40,    41,    43,    44,    28,    28,    27,    27,    32,
      45,    46,    57,    58,    77,     4,    74,    74,    73,    31,
      75,    19,    81,    82,    81,    47,    28,    28,    28,    28,
      82,    82,    56,    74,    29,    73,    73,    75,    74,    29,
      32,    29,    19,    81,    81,    82,    82,    29,    29,    73,
      74,    73,    82,    29,    29,    29,    29,    73
  };

  const unsigned char
  schema_parser::yyr1_[] =
  {
       0,    49,    50,    51,    52,    53,    53,    54,    55,    55,
      56,    57,    57,    58,    58,    59,    59,    60,    60,    61,
      62,    63,    63,    64,    64,    64,    64,    64,    64,    64,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      73,    74,    74,    75,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    77,    78,    79,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    81,
      81,    82,    82,    82
  };

  const unsigned char
  schema_parser::yyr2_[] =
  {
       0,     2,     2,     6,     2,     0,     1,     4,     1,     3,
       2,     0,     1,     1,     1,     0,     1,     1,     2,     5,
       2,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     5,     8,     9,     7,     8,     9,    10,     0,
       2,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     4,     3,
       4,     4,     5,     4,     5,     4,     5,     4,     5,     1,
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
  "TEXT", "ARRAY", "LIST", "MAP", "LTYPEBRACE", "RTYPEBRACE", "LINBRACE",
  "RINBRACE", "LSZBRACE", "RSZBRACE", "LINTYPBRACE", "RINTYPBRACE",
  "COMMA", "DEFAULT", "TYPE_END", "KEY_FIELDS", "INDEX", "SCHEMA",
  "CONSTRAINT", "REGEX", "IN", "BETWEEN", "NOT", "LT", "GT", "ASC", "DESC",
  "COLON", "NEWLINE", "$accept", "parse", "schema", "schema_declare",
  "opt_key_fields", "key_fields", "columns", "column", "opt_sort", "sort",
  "opt_types", "types", "type", "type_declare", "declarations", "declare",
  "declare_native", "declare_ref", "declare_native_arr", "declare_ref_arr",
  "declare_native_list", "declare_ref_list", "declare_netive_map",
  "declare_ref_map", "opt_default", "opt_constraint", "variable",
  "datatype", "size_def", "declare_finish", "constraint",
  "constraint_type", "values", "value", YY_NULLPTR
  };


  const unsigned short int
  schema_parser::yyrline_[] =
  {
       0,   125,   125,   129,   133,   140,   142,   146,   155,   158,
     172,   191,   194,   201,   204,   209,   211,   215,   216,   219,
     223,   230,   231,   236,   237,   238,   239,   240,   241,   242,
     243,   247,   258,   267,   279,   292,   304,   314,   327,   338,
     340,   346,   348,   353,   357,   358,   359,   360,   361,   362,
     363,   364,   365,   366,   367,   368,   372,   375,   378,   381,
     387,   393,   399,   405,   411,   417,   423,   429,   435,   444,
     454,   469,   476,   483
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
      45,    46,    47,    48
    };
    const unsigned int user_token_number_max_ = 303;
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
#line 1540 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.cpp" // lalr1.cc:1167
#line 487 "/work/dev/wookler/ReactFS/parsers/src/schema_parser.yy" // lalr1.cc:1168


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
