
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

#ifndef AST_HH
#define AST_HH

#include<string>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define AST_NODE_NODE_SPACE "               "

using namespace std;

enum relation_op {LE, LT, GE, GT, NE, EQ};
enum arith_op {ADD, SUB, MUL, DIV};

class Ast;

class Ast
{
protected:
	Data_Type node_data_type;
	string type;
public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast(int line);

	virtual void print_ast(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual string get_type() = 0;

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Assignment_Ast();

	string get_type();

	Data_Type get_data_type();
	bool check_ast(int line);



	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Name_Ast:public Ast
{
	string variable_name;
	Symbol_Table_Entry variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry);
	~Name_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	string get_type();

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type);
	~Number_Ast();

	string get_type();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Return_Ast:public Ast
{

public:
	Return_Ast();
	~Return_Ast();

	string get_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

/// My code ////

class Relational_Expr_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;
	relation_op op;
public:
	Relational_Expr_Ast(Ast * arg_lhs, Ast * arg_rhs, relation_op arg_op);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	string get_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Goto_Ast:public Ast
{
	
public:
	int successor;
	Goto_Ast(int arg_successor);
	~Goto_Ast();

	string get_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class If_Else_Ast:public Ast
{
	Ast * condition;
	int true_successor;
	int false_successor;
public:
	If_Else_Ast(Ast * arg_condition, int arg_true_successor, int arg_false_successor);
	~If_Else_Ast();

	void print_ast(ostream & file_buffer);
	bool check_ast(int line);

	string get_type();

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Arith_Ast : public Ast
{
	Ast * lhs;
	Ast * rhs;
	arith_op op;
public:
	Arith_Ast(Ast * arg_lhs, Ast * arg_rhs, arith_op arg_op);
	~Arith_Ast();

	Data_Type get_data_type();
	void print_ast(ostream & file_buffer);
	bool check_ast(int line);

	string get_type();

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

};

class Unary_Ast : public Ast
{
	Ast * operand;
	arith_op op;
public:
	Unary_Ast(Ast * oper, arith_op arg_op);
	~Unary_Ast();

	Data_Type get_data_type();
	void print_ast(ostream & file_buffer);
	bool check_ast(int line);

	string get_type();
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Cast_Ast : public Ast
{
	Ast * operand;
	Data_Type datatype;
public:
	Cast_Ast(Ast * oper, Data_Type arg_type);
	~Cast_Ast();

	Data_Type get_data_type();
	void print_ast(ostream & file_buffer);
	bool check_ast(int line);

	string get_type();
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

#endif
