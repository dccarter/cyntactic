%token SP
%token NL
%token DOT
%token IDENTIFIER

%%
Program
    : Import
;

Import
    : 'import' SP IDENTIFIER
    | 'import' SP IDENTIFIER '.' IDENTIFIER
    | 'import' SP IDENTIFIER '.' '{' CommaSeperatedIdentifiers '}'

CommaSeperatedIdentifiers
    : IDENTIFIER
    | CommaSeperatedIdentifiers ',' IDENTIFIER
%%