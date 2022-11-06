# Tiny Compiler Grammar

### Backus-Norm Form


program ::= {statement}

statement ::= "PRINT" (expression | string) nl
<br>        | "IF" comparison "THEN" nl {statement} "ENDIF" nl
<br>        | "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
<br>        | "LABEL" ident nl
<br>        | "GOTO" ident nl
<br>        | "LET" ident "=" expression nl
<br>        | "INPUT" ident nl

comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+

expression ::= term {( "-" | "+" ) term}

term ::= unary {( "/" | "*" ) unary}

unary ::= ["+" | "-"] primary

primary ::= number | ident

nl ::= '\n'+
