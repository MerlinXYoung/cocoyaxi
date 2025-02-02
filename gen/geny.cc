/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20140715

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "geny.yy"
#include <iostream>
#include "gen.h"
#line 6 "geny.yy"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
    char* iden;
    char* keyword;
    bool bconst;
    int64_t iconst;
    double dconst;
    Service* tservice;
    Field* tfield;
    Value* tvalue;
    Type* ttype;
    Object* tobject;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 44 "geny.cc"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define tok_identifier 257
#define tok_literal 258
#define tok_int_constant 259
#define tok_dbl_constant 260
#define tok_bool_constant 261
#define tok_package 262
#define tok_service 263
#define tok_bool 264
#define tok_int 265
#define tok_int32 266
#define tok_int64 267
#define tok_uint32 268
#define tok_uint64 269
#define tok_double 270
#define tok_string 271
#define tok_object 272
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,   12,   12,   14,   15,   13,   13,   16,   16,   17,
    2,    2,    1,    5,    7,    7,    9,    9,    9,    4,
    4,    4,    4,    3,    3,    3,    3,    3,    3,    3,
    3,    8,   10,   10,   11,   11,   11,   11,    6,    6,
   18,   18,   18,
};
static const YYINT yylen[] = {                            2,
    2,    2,    0,    1,    2,    2,    0,    1,    1,    5,
    2,    0,    2,    5,    2,    0,    3,    4,    4,    1,
    1,    3,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    0,    1,    1,    1,    1,    4,    3,
    1,    1,    0,
};
static const YYINT yydefred[] = {                         3,
    0,    0,    0,    0,    2,    4,    5,    0,    0,    9,
    6,    8,    0,    0,   12,   16,    0,    0,    0,   10,
   11,    0,   24,   25,   26,   27,   28,   29,   30,   31,
    0,   16,   14,    0,   21,    0,   23,    0,   15,   41,
   42,   13,   16,    0,   20,    0,   32,    0,   16,    0,
    0,   40,   22,    0,   17,    0,    0,   39,   38,   36,
   37,   35,   33,   19,   18,
};
static const YYINT yydgoto[] = {                          1,
   21,   17,   35,   36,   10,   37,   18,   38,   39,   55,
   63,    2,    4,    5,    6,   11,   12,   42,
};
static const YYINT yysindex[] = {                         0,
    0, -243, -249, -254,    0,    0,    0, -242, -237,    0,
    0,    0, -102,  -99,    0,    0, -123,  -91,  -37,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -98,    0,    0,   -9,    0, -230,    0,  -86,    0,    0,
    0,    0,    0,  -75,    0,  -65,    0,  -32,    0,   -9,
  -58,    0,    0, -247,    0,  -42,  -63,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    1,    0,   31,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -121,    0,
    0,  -88,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -25,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static const YYINT yygindex[] = {                         0,
    0,    0,    0,  -24,    0,    0,  -26,    2,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 273
static const YYINT yytable[] = {                         34,
    7,   20,   32,   43,   50,   44,   40,    7,    8,   46,
   59,   60,   61,   62,   13,   34,   51,    9,    3,   14,
   15,   41,   56,   16,   43,   57,   47,   53,   54,   65,
    1,   32,   34,   33,   32,    0,   49,   48,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   32,   34,   52,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   32,   34,   58,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   32,   34,   64,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   34,    0,   34,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   32,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   19,    0,   43,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   22,    0,    0,   20,    0,
    0,    0,   23,   24,   25,   26,   27,   28,   29,   30,
   31,   22,    0,    0,    0,    0,    0,    0,   23,   24,
   25,   26,   27,   28,   29,   30,   31,    0,   22,    0,
    0,    0,    0,    0,    0,   23,   24,   25,   26,   27,
   28,   29,   30,   31,   22,    0,    0,    0,    0,    0,
    0,   23,   24,   25,   26,   27,   28,   29,   30,   31,
    0,   34,    0,    0,    0,    0,    0,    0,   34,   34,
   34,   34,   34,   34,   34,   34,   34,   45,    0,    0,
    0,    0,    0,    0,   23,   24,   25,   26,   27,   28,
   29,   30,   31,    7,    0,    0,    0,    0,    0,    0,
    0,    0,    7,
};
static const YYINT yycheck[] = {                         91,
    0,  125,   91,  125,   91,   32,   44,  257,  263,   34,
  258,  259,  260,  261,  257,   91,   43,  272,  262,  257,
  123,   59,   49,  123,  123,   50,  257,   93,   61,   93,
    0,  123,   91,  125,  123,   -1,  123,   36,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   91,  125,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  123,   91,  125,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  123,   91,  125,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,  125,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  123,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  257,   -1,  257,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  257,   -1,   -1,  257,   -1,
   -1,   -1,  264,  265,  266,  267,  268,  269,  270,  271,
  272,  257,   -1,   -1,   -1,   -1,   -1,   -1,  264,  265,
  266,  267,  268,  269,  270,  271,  272,   -1,  257,   -1,
   -1,   -1,   -1,   -1,   -1,  264,  265,  266,  267,  268,
  269,  270,  271,  272,  257,   -1,   -1,   -1,   -1,   -1,
   -1,  264,  265,  266,  267,  268,  269,  270,  271,  272,
   -1,  257,   -1,   -1,   -1,   -1,   -1,   -1,  264,  265,
  266,  267,  268,  269,  270,  271,  272,  257,   -1,   -1,
   -1,   -1,   -1,   -1,  264,  265,  266,  267,  268,  269,
  270,  271,  272,  263,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  272,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 272
#define YYUNDFTOKEN 293
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,"';'",0,"'='",0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"tok_identifier","tok_literal","tok_int_constant","tok_dbl_constant",
"tok_bool_constant","tok_package","tok_service","tok_bool","tok_int",
"tok_int32","tok_int64","tok_uint32","tok_uint64","tok_double","tok_string",
"tok_object",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : Program",
"Program : HeaderList DefinitionList",
"HeaderList : HeaderList Header",
"HeaderList :",
"Header : Package",
"Package : tok_package tok_identifier",
"DefinitionList : DefinitionList Definition",
"DefinitionList :",
"Definition : Service",
"Definition : Object",
"Service : tok_service tok_identifier '{' MethodList '}'",
"MethodList : MethodList Method",
"MethodList :",
"Method : tok_identifier Separ",
"Object : tok_object tok_identifier '{' FieldList '}'",
"FieldList : FieldList Field",
"FieldList :",
"Field : FieldType FieldName FieldValue",
"Field : FieldName '[' FieldType ']'",
"Field : FieldName '{' FieldList '}'",
"FieldType : tok_identifier",
"FieldType : BaseType",
"FieldType : '[' FieldType ']'",
"FieldType : AnonymousObject",
"BaseType : tok_bool",
"BaseType : tok_int",
"BaseType : tok_int32",
"BaseType : tok_int64",
"BaseType : tok_uint32",
"BaseType : tok_uint64",
"BaseType : tok_double",
"BaseType : tok_string",
"FieldName : tok_identifier",
"FieldValue : '=' ConstValue",
"FieldValue :",
"ConstValue : tok_bool_constant",
"ConstValue : tok_int_constant",
"ConstValue : tok_dbl_constant",
"ConstValue : tok_literal",
"AnonymousObject : tok_object '{' FieldList '}'",
"AnonymousObject : '{' FieldList '}'",
"Separ : ','",
"Separ : ';'",
"Separ :",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 55 "geny.yy"
	{}
break;
case 2:
#line 59 "geny.yy"
	{}
break;
case 3:
#line 60 "geny.yy"
	{}
break;
case 4:
#line 64 "geny.yy"
	{}
break;
case 5:
#line 68 "geny.yy"
	{
        g_prog->add_pkg(S(yystack.l_mark[0].iden));
    }
break;
case 6:
#line 74 "geny.yy"
	{}
break;
case 7:
#line 75 "geny.yy"
	{}
break;
case 8:
#line 79 "geny.yy"
	{}
break;
case 9:
#line 81 "geny.yy"
	{}
break;
case 10:
#line 85 "geny.yy"
	{
        if (g_prog->service()) {
            std::cout << "error: found multiple service" << std::endl;
            delete(yystack.l_mark[-1].tservice);
            exit(0);
        }
        yystack.l_mark[-1].tservice->set_name(S(yystack.l_mark[-3].iden));
        g_prog->set_service(yystack.l_mark[-1].tservice);
    }
break;
case 11:
#line 97 "geny.yy"
	{
        yyval.tservice = yystack.l_mark[-1].tservice;
        fastring m = S(yystack.l_mark[0].iden);
        if (!yyval.tservice->add_method(m)) {
            std::cout << "error: method name " << m << " duplicated" << std::endl;
            exit(0);
        }
    }
break;
case 12:
#line 105 "geny.yy"
	{
        yyval.tservice = new Service();
    }
break;
case 13:
#line 111 "geny.yy"
	{
        yyval.iden = yystack.l_mark[-1].iden;
    }
break;
case 14:
#line 117 "geny.yy"
	{
        yyval.tobject = yystack.l_mark[-1].tobject;
        yyval.tobject->set_name(S(yystack.l_mark[-3].iden));
        if (!g_prog->add_object(yyval.tobject)) {
            std::cout << "error: object type " << yyval.tobject->name() << " duplicated" << std::endl;
            delete(yyval.tobject);
            exit(0);
        }
    }
break;
case 15:
#line 129 "geny.yy"
	{
        yyval.tobject = yystack.l_mark[-1].tobject;
        if (!(yyval.tobject->add_field(yystack.l_mark[0].tfield))) {
            std::cout << "error: field " << yystack.l_mark[0].tfield->name() << " duplicated in object " << yyval.tobject->name() << std::endl;
            exit(0);
        }
    }
break;
case 16:
#line 136 "geny.yy"
	{
        yyval.tobject = new Object();
    }
break;
case 17:
#line 142 "geny.yy"
	{
        yyval.tfield = new Field();
        yyval.tfield->set_type(yystack.l_mark[-2].ttype);
        yyval.tfield->set_name(S(yystack.l_mark[-1].iden));
        yyval.tfield->set_value(yystack.l_mark[0].tvalue);
    }
break;
case 18:
#line 149 "geny.yy"
	{
        yyval.tfield = new Field();
        auto a = new Array();
        a->set_element_type(yystack.l_mark[-1].ttype);
        yyval.tfield->set_type(a);
        yyval.tfield->set_name(S(yystack.l_mark[-3].iden));
    }
break;
case 19:
#line 157 "geny.yy"
	{
        g_prog->add_anony_object(yystack.l_mark[-1].tobject);
        yyval.tfield = new Field();
        yyval.tfield->set_type(yystack.l_mark[-1].tobject);
        yyval.tfield->set_name(S(yystack.l_mark[-3].iden));
    }
break;
case 20:
#line 166 "geny.yy"
	{
        fastring s = S(yystack.l_mark[0].iden);
        yyval.ttype = g_prog->find_object(s);
        if (!yyval.ttype) {
            std::cout << "unknown type " << s << " at line " << yylineno << std::endl;
            exit(0);
        }
    }
break;
case 21:
#line 175 "geny.yy"
	{
        yyval.ttype = yystack.l_mark[0].ttype;
    }
break;
case 22:
#line 179 "geny.yy"
	{
        auto a = new Array();
        a->set_element_type(yystack.l_mark[-1].ttype);
        yyval.ttype = a;
    }
break;
case 23:
#line 185 "geny.yy"
	{
        yyval.ttype = yystack.l_mark[0].tobject;
        g_prog->add_anony_object(yystack.l_mark[0].tobject);
    }
break;
case 24:
#line 192 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("bool");
        yyval.ttype->set_type(type_bool);
    }
break;
case 25:
#line 198 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("int");
        yyval.ttype->set_type(type_int);
    }
break;
case 26:
#line 204 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("int32");
        yyval.ttype->set_type(type_int32);
    }
break;
case 27:
#line 210 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("int64");
        yyval.ttype->set_type(type_int64);
    }
break;
case 28:
#line 216 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("uint32");
        yyval.ttype->set_type(type_uint32);
    }
break;
case 29:
#line 222 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("uint64");
        yyval.ttype->set_type(type_uint64);
    }
break;
case 30:
#line 228 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("double");
        yyval.ttype->set_type(type_double);
    }
break;
case 31:
#line 234 "geny.yy"
	{
        yyval.ttype = new Type();
        yyval.ttype->set_name("string");
        yyval.ttype->set_type(type_string);
    }
break;
case 32:
#line 242 "geny.yy"
	{
        yyval.iden = yystack.l_mark[0].iden;
    }
break;
case 33:
#line 248 "geny.yy"
	{
        yyval.tvalue = yystack.l_mark[0].tvalue;
    }
break;
case 34:
#line 251 "geny.yy"
	{
        yyval.tvalue = nullptr;
    }
break;
case 35:
#line 257 "geny.yy"
	{
        yyval.tvalue = new Value();
        yyval.tvalue->set_bool(yystack.l_mark[0].bconst);
    }
break;
case 36:
#line 262 "geny.yy"
	{
        yyval.tvalue = new Value();
        yyval.tvalue->set_integer(yystack.l_mark[0].iconst);
    }
break;
case 37:
#line 267 "geny.yy"
	{
        yyval.tvalue = new Value();
        yyval.tvalue->set_double(yystack.l_mark[0].dconst);
    }
break;
case 38:
#line 272 "geny.yy"
	{
        yyval.tvalue = new Value();
        yyval.tvalue->set_string(yystack.l_mark[0].iden);
    }
break;
case 39:
#line 279 "geny.yy"
	{
        yyval.tobject = yystack.l_mark[-1].tobject;
    }
break;
case 40:
#line 283 "geny.yy"
	{
        yyval.tobject = yystack.l_mark[-1].tobject;
    }
break;
case 41:
#line 289 "geny.yy"
	{}
break;
case 42:
#line 291 "geny.yy"
	{}
break;
case 43:
#line 293 "geny.yy"
	{}
break;
#line 805 "geny.cc"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
