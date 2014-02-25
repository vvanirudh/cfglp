
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	float float_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
	relation_op op;
};

%token <integer_value> INTEGER_NUMBER
%token <integer_value> BB
%token <float_value> FLOAT_NUMBER
%token <string_value> NAME
%token RETURN INTEGER FLOAT DOUBLE
%token IF
%token ELSE
%token GOTO
%token ASSIGN_OP
%left NEP EQP
%left LTP LEP GTP GEP

%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> variable
%type <ast> constant
%type <ast> comparision_expression
%type <ast> goto_expression
%type <ast> if_expression
%type <ast> arith_expression
%type <ast> term_expression
%type <ast> factor
%type <ast> cast_factor

%start program

%%

program:
	declaration_statement_list procedure_name
	{
		
		program_object.set_global_table(*$1);
		return_statement_used_flag = false;				// No return statement in the current procedure till now
		
	}
	procedure_body
	{
		
		program_object.set_procedure_map(*current_procedure);

		if ($1)
			$1->global_list_in_proc_map_check(get_line_number());

		delete $1;
		
	}
|
	procedure_name
	{
		
		return_statement_used_flag = false;				// No return statement in the current procedure till now
		
	}
	procedure_body
	{
		
		program_object.set_procedure_map(*current_procedure);
		
	}
;

procedure_name:
	NAME '(' ')'
	{
		
		current_procedure = new Procedure(void_data_type, *$1);
		
	}
;

procedure_body:
	'{' declaration_statement_list
	{
		
		current_procedure->set_local_list(*$2);
		delete $2;
		
	}
	basic_block_list '}'
	{
		
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			//report_error("Atleast 1 basic block should have a return statement", line);
		}

		current_procedure->set_basic_block_list(*$4);

		delete $4;
		
	}
|
	'{' basic_block_list '}'
	{
		
		if (return_statement_used_flag == false)
		{
			int line = get_line_number();
			//report_error("Atleast 1 basic block should have a return statement", line);
		}

		current_procedure->set_basic_block_list(*$2);

		delete $2;
		
	}
;

declaration_statement_list:
	declaration_statement
	{
		
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
		
	}
|
	declaration_statement_list declaration_statement
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list
		
		int line = get_line_number();
		program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($2);
		
	}
;

declaration_statement:
	INTEGER NAME ';'
	{
		
		$$ = new Symbol_Table_Entry(*$2, int_data_type);

		delete $2;
		
	}
|
	FLOAT NAME ';'
	{
		
		$$ = new Symbol_Table_Entry(*$2, float_data_type);

		delete $2;
		
	}
;

basic_block_list:
	basic_block_list basic_block
	{
		
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
		
	}
|
	basic_block
	{
		
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
		
	}
	
;

basic_block:
	BB ':' executable_statement_list
	{
		
		if ($1 < 2)
		{
			int line = get_line_number();
			report_error("Illegal basic block lable", line);
		}

		if ($3 != NULL)
			$$ = new Basic_Block($1, *$3);
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
		}

		delete $3;
		
	}
;

executable_statement_list:
	assignment_statement_list
	{
		
		$$ = $1;
		
	}
|
	assignment_statement_list RETURN ';'
	{
		
		Ast * ret = new Return_Ast();

		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back(ret);
		
	}
|
	assignment_statement_list if_expression
	{
		
		if($1!=NULL)
			$$ = $1;
		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		
	}
|
	assignment_statement_list goto_expression
	{
		
		
		if($1!=NULL)
			$$ = $1;
		else
			$$ = new list<Ast *>;

		$$->push_back($2);
		
	}
;

assignment_statement_list:
	{
		
		$$ = NULL;
		
	}
|
	assignment_statement_list assignment_statement
	{
		
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		
	}
;

assignment_statement:
	variable ASSIGN_OP comparision_expression ';'
	{
		
		$$ = new Assignment_Ast($1, $3);

		int line = get_line_number();
		$$->check_ast(line);
		
	}
;

variable:
	NAME
	{
		
		Symbol_Table_Entry var_table_entry;

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = program_object.get_symbol_table_entry(*$1);

		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
		
	}
;

constant:
	INTEGER_NUMBER
	{
		
		$$ = new Number_Ast<int>($1, int_data_type);
		
	}
|
	FLOAT_NUMBER
	{
		
		$$ = new Number_Ast<float>($1, float_data_type);
		
	}
;

goto_expression:
	GOTO BB ';'
	{
		

		if ($2 < 2)
		{
			int line = get_line_number();
			report_error("Illegal basic block lable", line);
		}

		$$ = new Goto_Ast($2);
		
	}
;

if_expression:
	IF '(' comparision_expression ')' GOTO BB ';' ELSE GOTO BB ';'
	{
		
		$$ = new If_Else_Ast($3, $6, $10);
		
	}
;

comparision_expression:
	comparision_expression LEP comparision_expression
	{
		
		$$ = new Relational_Expr_Ast($1, $3, LE);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	comparision_expression LTP comparision_expression
	{
		
		$$ = new Relational_Expr_Ast($1, $3, LT);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	comparision_expression GEP comparision_expression
	{
		
		$$ = new Relational_Expr_Ast($1, $3, GE);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	comparision_expression GTP comparision_expression
	{
		
		$$ = new Relational_Expr_Ast($1, $3, GT);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	comparision_expression NEP comparision_expression
	{
		
		$$ = new Relational_Expr_Ast($1, $3, NE);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	comparision_expression EQP comparision_expression
	{
		
		$$ = new Relational_Expr_Ast($1, $3, EQ);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	arith_expression
	{
		
		$$ = $1;
		
	}
;

arith_expression:
	arith_expression '+' term_expression
	{
		
		$$ = new Arith_Ast($1,$3, ADD);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	arith_expression '-' term_expression
	{
		
		$$ = new Arith_Ast($1,$3, SUB);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	term_expression
	{
		
		$$ = $1;
		
	}
;

term_expression:
	term_expression '*' factor
	{
		
		$$ = new Arith_Ast($1,$3, MUL);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	term_expression '/' factor
	{
		$$ = new Arith_Ast($1,$3, DIV);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	term_expression '*' cast_factor
	{
		
		$$ = new Arith_Ast($1,$3, MUL);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	term_expression '/' cast_factor
	{
		$$ = new Arith_Ast($1,$3, DIV);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	factor
	{
		
		$$ = $1;
		
	}
|
	cast_factor
	{
		$$ = $1;
	}
;

factor:
	variable
	{
		
		$$ = $1;
		
		
	}
|
	constant
	{
		
		$$ = $1;
		
		
	}
|
	'-' variable
	{
		
		$$ = new Unary_Ast($2,SUB);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	'-' constant
	{
		
		$$ = new Unary_Ast($2,SUB);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	'(' comparision_expression ')'
	{
		$$ = $2;

	}
|
	'-' '(' comparision_expression ')'
	{
		$$ = new Unary_Ast($3,SUB);
		int line = get_line_number();
		$$->check_ast(line);
	}
;

cast_factor:
	'(' FLOAT ')' variable
	{
		$$ = new Cast_Ast($4, float_data_type);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	'(' INTEGER ')' variable
	{
		$$ = new Cast_Ast($4, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);
	}
|
	'(' DOUBLE ')' variable
	{
		
		$$ = new Cast_Ast($4, float_data_type);
		int line = get_line_number();
		$$->check_ast(line);
		
	}
|
	'(' INTEGER ')' '(' arith_expression ')'
	{
		$$ = new Cast_Ast($5, int_data_type);
		int line = get_line_number();
		$$->check_ast(line);

	}
|
	'(' FLOAT ')' '(' arith_expression ')'
	{
		$$ = new Cast_Ast($5, float_data_type);
		int line = get_line_number();
		$$->check_ast(line);

	}
|
	'(' DOUBLE ')' '(' arith_expression ')'
	{
		$$ = new Cast_Ast($5, float_data_type);
		int line = get_line_number();
		$$->check_ast(line);

	}
;


