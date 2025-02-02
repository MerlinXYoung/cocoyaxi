%{
#include <iostream>
#include "gen.h"
%}

%union {
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
}

%token<iden>     tok_identifier
%token<iden>     tok_literal

%token<iconst>   tok_int_constant
%token<dconst>   tok_dbl_constant
%token<bconst>   tok_bool_constant

%token<keyword>  tok_package
%token<keyword>  tok_service
%token<keyword>  tok_bool
%token<keyword>  tok_int
%token<keyword>  tok_int32
%token<keyword>  tok_int64
%token<keyword>  tok_uint32
%token<keyword>  tok_uint64
%token<keyword>  tok_double
%token<keyword>  tok_string
%token<keyword>  tok_object

%type<iden>      Method
%type<tservice>  MethodList
%type<ttype>     BaseType
%type<ttype>     FieldType
%type<tobject>   Object
%type<tobject>   AnonymousObject
%type<tobject>   FieldList
%type<iden>      FieldName
%type<tfield>    Field
%type<tvalue>    FieldValue
%type<tvalue>    ConstValue


%%

Program:
    HeaderList DefinitionList
    {}

HeaderList:
    HeaderList Header
    {}
  | {}

Header:
    Package
    {}

Package:
    tok_package tok_identifier
    {
        g_prog->add_pkg(S($2));
    }

DefinitionList:
    DefinitionList Definition
    {}
  | {}

Definition:
    Service
    {}
  | Object
    {}

Service:
    tok_service tok_identifier '{' MethodList '}'
    {
        if (g_prog->service()) {
            std::cout << "error: found multiple service" << std::endl;
            delete($4);
            exit(0);
        }
        $4->set_name(S($2));
        g_prog->set_service($4);
    }

MethodList:
    MethodList Method
    {
        $$ = $1;
        fastring m = S($2);
        if (!$$->add_method(m)) {
            std::cout << "error: method name " << m << " duplicated" << std::endl;
            exit(0);
        }
    }
  | {
        $$ = new Service();
    }

Method:
    tok_identifier Separ
    {
        $$ = $1;
    }

Object:
    tok_object tok_identifier '{' FieldList '}'
    {
        $$ = $4;
        $$->set_name(S($2));
        if (!g_prog->add_object($$)) {
            std::cout << "error: object type " << $$->name() << " duplicated" << std::endl;
            delete($$);
            exit(0);
        }
    }

FieldList:
    FieldList Field
    {
        $$ = $1;
        if (!($$->add_field($2))) {
            std::cout << "error: field " << $2->name() << " duplicated in object " << $$->name() << std::endl;
            exit(0);
        }
    }
  | {
        $$ = new Object();
    }

Field:
    FieldType FieldName FieldValue
    {
        $$ = new Field();
        $$->set_type($1);
        $$->set_name(S($2));
        $$->set_value($3);
    }
  | FieldName '[' FieldType ']'
    {
        $$ = new Field();
        auto a = new Array();
        a->set_element_type($3);
        $$->set_type(a);
        $$->set_name(S($1));
    }
  | FieldName '{' FieldList '}'
    {
        g_prog->add_anony_object($3);
        $$ = new Field();
        $$->set_type($3);
        $$->set_name(S($1));
    }

FieldType:
    tok_identifier
    {
        fastring s = S($1);
        $$ = g_prog->find_object(s);
        if (!$$) {
            std::cout << "unknown type " << s << " at line " << yylineno << std::endl;
            exit(0);
        }
    }
  | BaseType
    {
        $$ = $1;
    }
  | '[' FieldType ']'
    {
        auto a = new Array();
        a->set_element_type($2);
        $$ = a;
    }
  | AnonymousObject
    {
        $$ = $1;
        g_prog->add_anony_object($1);
    }

BaseType:
    tok_bool
    {
        $$ = new Type();
        $$->set_name("bool");
        $$->set_type(type_bool);
    }
  | tok_int
    {
        $$ = new Type();
        $$->set_name("int");
        $$->set_type(type_int);
    }
  | tok_int32
    {
        $$ = new Type();
        $$->set_name("int32");
        $$->set_type(type_int32);
    }
  | tok_int64
    {
        $$ = new Type();
        $$->set_name("int64");
        $$->set_type(type_int64);
    }
  | tok_uint32
    {
        $$ = new Type();
        $$->set_name("uint32");
        $$->set_type(type_uint32);
    }
  | tok_uint64
    {
        $$ = new Type();
        $$->set_name("uint64");
        $$->set_type(type_uint64);
    }
  | tok_double
    {
        $$ = new Type();
        $$->set_name("double");
        $$->set_type(type_double);
    }
  | tok_string
    {
        $$ = new Type();
        $$->set_name("string");
        $$->set_type(type_string);
    }

FieldName:
    tok_identifier
    {
        $$ = $1;
    }

FieldValue:
    '=' ConstValue
    {
        $$ = $2;
    }
  | {
        $$ = nullptr;
    }

ConstValue:
    tok_bool_constant
    {
        $$ = new Value();
        $$->set_bool($1);
    }
  | tok_int_constant
    {
        $$ = new Value();
        $$->set_integer($1);
    }
  | tok_dbl_constant
    {
        $$ = new Value();
        $$->set_double($1);
    }
  | tok_literal
    {
        $$ = new Value();
        $$->set_string($1);
    }

AnonymousObject:
    tok_object '{' FieldList '}'
    {
        $$ = $3;
    }
  | '{' FieldList '}'
    {
        $$ = $2;
    }

Separ:
    ','
    {}
  | ';'
    {}
  |
    {}

%%
