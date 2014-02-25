
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

#include<iostream>
#include<fstream>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	type = "A";
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

string Assignment_Ast::get_type()
{
	return type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}
	else if(lhs->get_data_type()==float_data_type && rhs->get_data_type()==int_data_type)
	{
		node_data_type = float_data_type;
		return true;
	}
	else if(lhs->get_data_type()==int_data_type && rhs->get_data_type()==float_data_type)
	{
		node_data_type = float_data_type;
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";

	if(lhs->get_type()!="REL" && lhs->get_type()!="ARITH")
		lhs->print_ast(file_buffer);
	else
	{
		file_buffer<<"\n";
		lhs->print_ast(file_buffer);
	}
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	if(rhs->get_type()!="REL" && rhs->get_type()!="ARITH")
		rhs->print_ast(file_buffer);
	else
	{
		file_buffer<<"\n";
		rhs->print_ast(file_buffer);
	}
	file_buffer << ")\n";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	// file_buffer<<result.get_float_value()<<"\n";
	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}
/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
	type = "N";
	node_data_type = variable_symbol_entry.get_data_type();
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

string Name_Ast::get_type()
{
	return type;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else if(loc_var_val->get_result_enum() == float_result)
		{
			// file_buffer << loc_var_val->get_float_value() << "\n";
			printf("%.2f\n", loc_var_val->get_float_value());
		}
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else if(glob_var_val->get_result_enum() == float_result)
		{
			if (glob_var_val == NULL)
				// file_buffer << "0\n";
				printf("0.00\n");
			else
				// file_buffer << glob_var_val->get_float_value() << "\n";
				printf("%.2f\n", glob_var_val->get_float_value());
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}
	else if(result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
	 	i->set_value(result.get_float_value());
	}
	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
	type = "NUM";
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
string Number_Ast<DATA_TYPE>::get_type()
{
	return type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	if(node_data_type==int_data_type)
		file_buffer << "Num : " << constant;
	else
	{
		file_buffer <<"Num : ";
		printf("%.2f", (float)constant);
	}
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
	else
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value(constant);

		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{
	type = "RET";
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
}

string Return_Ast::get_type()
{
	return type;
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
	print_ast(file_buffer);
	result.set_result_enum(return_result);
	return result;
}

template class Number_Ast<int>;
template class Number_Ast<float>;

///////////////////////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast * arg_lhs, Ast * arg_rhs, relation_op arg_op)
{
	lhs = arg_lhs;
	rhs = arg_rhs;
	op = arg_op;
	type = "REL";
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

string Relational_Expr_Ast::get_type()
{
	return type;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Relational_Expr_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		if(op==LE || op==LT || op==GE || op==GT || op==NE || op==EQ)
		{
			node_data_type = lhs->get_data_type();
			return true;
		}
		else
		{
			report_error("Operator given  not compatible", line);
		}
	}
	else if(lhs->get_data_type()==float_data_type && rhs->get_data_type()==int_data_type)
	{
		if(op==LE || op==LT || op==GE || op==GT || op==NE || op==EQ)
		{
			node_data_type = float_data_type;
			return true;
		}
		else
		{
			report_error("Operator given  not compatible", line);
		}
	}
	else if(rhs->get_data_type()==float_data_type && lhs->get_data_type()==int_data_type)
	{
		if(op==LE || op==LT || op==GE || op==GT || op==NE || op==EQ)
		{
			node_data_type = float_data_type;
			return true;
		}
		else
		{
			report_error("Operator given  not compatible", line);
		}
	}


	report_error("Relational Expression data type not compatible", line);
}

void Relational_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<AST_NODE_SPACE<<"Condition: ";
	if(op==LE)
		file_buffer<<"LE\n";
	else if(op==LT)
		file_buffer<<"LT\n";
	else if(op==GE)
		file_buffer<<"GE\n";
	else if(op==GT)
		file_buffer<<"GT\n";
	else if(op==NE)
		file_buffer<<"NE\n";
	else if(op==EQ)
		file_buffer<<"EQ\n";
	else
		file_buffer<<"wrong\n";

	file_buffer <<AST_NODE_NODE_SPACE"LHS (";
	if(lhs->get_type()!="REL" && lhs->get_type()!="ARITH")
		lhs->print_ast(file_buffer);
	else
	{
		file_buffer<<"\n";
		lhs->print_ast(file_buffer);
	}
	file_buffer << ")\n";

	file_buffer <<AST_NODE_NODE_SPACE << "RHS (";
	if(rhs->get_type()!="REL" && rhs->get_type()!="ARITH") rhs->print_ast(file_buffer);
	else
	{
		file_buffer<<"\n";
		rhs->print_ast(file_buffer);
	}
	file_buffer << ")";
}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	
	Eval_Result & result_rhs = rhs->evaluate(eval_env, file_buffer);
	if(result_rhs.is_variable_defined()==false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);
	if(result_rhs.is_variable_defined()==false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result & result = *new Eval_Result_Value_Int();

	float lhs_value;
	float rhs_value;

	if(result_lhs.get_result_enum()==int_result)
		lhs_value = result_lhs.get_value();
	else
		lhs_value = result_lhs.get_float_value();

	if(result_rhs.get_result_enum()==float_result)
		rhs_value = result_rhs.get_float_value();
	else
		rhs_value = result_rhs.get_value();

	if(op==LE)
	{
		if(lhs_value<=rhs_value)
			result.set_value(1);
		else
			result.set_value(0);
	}
	else if(op==LT)
	{
		if(lhs_value<rhs_value)
			result.set_value(1);
		else
			result.set_value(0);
	}
	else if(op==GE)
	{
		if(lhs_value>=rhs_value)
			result.set_value(1);
		else
			result.set_value(0);
	}
	else if(op==GT)
	{
		if(lhs_value>rhs_value)
			result.set_value(1);
		else
			result.set_value(0);
	}
	else if(op==NE)
	{
		if(lhs_value!=rhs_value)
			result.set_value(1);
		else
			result.set_value(0);
	}
	else
	{
		if(lhs_value==rhs_value)
			result.set_value(1);
		else
			result.set_value(0);
	}
	return result;
}

///////////////////////////////////////////////////////////////////

Goto_Ast::Goto_Ast(int arg_successor)
{
	successor = arg_successor;
	type = "GOTO";
}

Goto_Ast::~Goto_Ast()
{

}

string Goto_Ast::get_type()
{
	return type;
}

void Goto_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<AST_SPACE<<"Goto statement:\n";
	file_buffer<<AST_NODE_SPACE"Successor: "<<successor<<"\n";
}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_BB();
	result.set_value(successor);
	print_ast(file_buffer);
	file_buffer<<AST_SPACE<<"GOTO (BB "<<successor<<")\n";
	return result;
}

////////////////////////////////////////////////////////////////////

If_Else_Ast::If_Else_Ast(Ast * arg_condition, int arg_true_successor, int arg_false_successor)
{
	condition = arg_condition;
	true_successor = arg_true_successor;
	false_successor = arg_false_successor;
	type="IF";
}

If_Else_Ast::~If_Else_Ast()
{
	delete condition;
}

string If_Else_Ast::get_type()
{
	return type;
}

void If_Else_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<AST_SPACE<<"If_Else statement:\n";
	// file_buffer<<AST_NODE_SPACE;
	condition->print_ast(file_buffer);
	file_buffer<<"\n";
	file_buffer<<AST_NODE_SPACE"True Successor: ";
	file_buffer<<true_successor<<"\n";
	file_buffer<<AST_NODE_SPACE"False Successor: ";
	file_buffer<<false_successor<<"\n";
}

Eval_Result & If_Else_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_BB();
	Eval_Result & result_condition = condition->evaluate(eval_env, file_buffer);
	bool predicate;
	if(result_condition.get_value()==1){
		result.set_value(true_successor);
		predicate = true;
	}
	else{
		result.set_value(false_successor);
		predicate = false;
	}

	print_ast(file_buffer);
	file_buffer<<AST_SPACE<<"Condition ";
	if(predicate)
		file_buffer<<"True : Goto (BB "<<true_successor<<")\n";
	else
		file_buffer<<"False : Goto (BB "<<false_successor<<")\n";

	return result;
}

bool If_Else_Ast::check_ast(int line)
{
	return condition->check_ast(line);
}

////////////////////////////////////////////////////////////////////////////////////////////////

Arith_Ast::Arith_Ast(Ast * arg_lhs, Ast * arg_rhs, arith_op arg_op)
{
	lhs = arg_lhs;
	rhs = arg_rhs;
	op = arg_op;
	type = "ARITH";
}

Arith_Ast::~Arith_Ast()
{
	delete lhs;
	delete rhs;
}

bool Arith_Ast::check_ast(int line)
{
	if(lhs->get_data_type()==rhs->get_data_type())
	{
		if(op==ADD || op==SUB || op==MUL || op==DIV)
		{
			node_data_type = lhs->get_data_type();
			return true;
		}
		else
		{
			report_error("Operator given  not compatible", line);
		}
	}
	else if(lhs->get_data_type()==float_data_type && rhs->get_data_type()==int_data_type)
	{
		if(op==ADD || op==SUB || op==MUL || op==DIV)
		{
			node_data_type = float_data_type;
			return true;
		}
		else
		{
			report_error("Operator given  not compatible", line);
		}
	}
	else if(rhs->get_data_type()==float_data_type && lhs->get_data_type()==int_data_type)
	{
		if(op==ADD || op==SUB || op==MUL || op==DIV)
		{
			node_data_type = float_data_type;
			return true;
		}
		else
		{
			report_error("Operator given  not compatible", line);
		}
	}

	report_error("Relational Expression data type not compatible", line);
}


Data_Type Arith_Ast::get_data_type()
{
	return node_data_type;
}

string Arith_Ast::get_type()
{
	return type;
}

void Arith_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<AST_NODE_SPACE<<"Arith: ";
	if(op==ADD)
	{
		file_buffer<<"PLUS\n";
	}
	else if(op==SUB)
	{
		file_buffer<<"MINUS\n";
	}
	else if(op==MUL)
	{
		file_buffer<<"MULT\n";
	}
	else
	{
		file_buffer<<"DIV\n";
	}

	file_buffer<<AST_NODE_NODE_SPACE<<"LHS (";
	if(lhs->get_type()!="REL" && lhs->get_type()!="ARITH")
	{
		lhs->print_ast(file_buffer);
	}
	else
	{
		file_buffer<<"\n";
		lhs->print_ast(file_buffer);
	}
	file_buffer<<")\n";	
	file_buffer<<AST_NODE_NODE_SPACE<<"RHS (";
	if(rhs->get_type()!="REL" && rhs->get_type()!="ARITH")
	{
		rhs->print_ast(file_buffer);
	}
	else
	{
		file_buffer<<"\n";
		rhs->print_ast(file_buffer);
	}
	file_buffer<<")";
}

Eval_Result & Arith_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_rhs = rhs->evaluate(eval_env, file_buffer);
	if(result_rhs.is_variable_defined()==false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);
	if(result_rhs.is_variable_defined()==false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	if(node_data_type==int_data_type){
		Eval_Result & result = *new Eval_Result_Value_Int(); //Should account for float also
		if(op==ADD)
		{
			result.set_value(result_lhs.get_value() + result_rhs.get_value());		
		}
		else if(op==SUB)
		{
			result.set_value(result_lhs.get_value() - result_rhs.get_value());
		}
		else if(op==MUL)
		{
			result.set_value(result_lhs.get_value() * result_rhs.get_value());
		}
		else
		{
			result.set_value(result_lhs.get_value() / result_rhs.get_value());
		}
		// print_ast(file_buffer);
		return result;
	}
	else{
		Eval_Result & result = *new Eval_Result_Value_Float();
		float lhs_value;
		float rhs_value;

		if(result_lhs.get_result_enum()==int_result)
			lhs_value = result_lhs.get_value();
		else
			lhs_value = result_lhs.get_float_value();

		if(result_rhs.get_result_enum()==int_result)
			rhs_value = result_rhs.get_value();
		else
			rhs_value = result_rhs.get_float_value();

		if(op==ADD)
		{
			result.set_value(lhs_value + rhs_value);		
		}
		else if(op==SUB)
		{
			result.set_value(lhs_value - rhs_value);
		}
		else if(op==MUL)
		{
			result.set_value(lhs_value * rhs_value);
		}
		else
		{
			result.set_value(lhs_value / rhs_value);
		}

		// print_ast(file_buffer);

		return result;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

Unary_Ast::Unary_Ast(Ast * oper, arith_op arg_op)
{
	operand = oper;
	op = arg_op;
	type = "ARITH";
}

Unary_Ast::~Unary_Ast()
{
	delete operand;
}

Data_Type Unary_Ast::get_data_type()
{
	return node_data_type;
}

bool Unary_Ast::check_ast(int line)
{
	node_data_type = operand->get_data_type();
}

string Unary_Ast::get_type()
{
	return type;
}

void Unary_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<<AST_NODE_SPACE<<"Arith: ";
	if(op==ADD)
	{
		file_buffer<<"UPLUS\n";
	}
	else if(op==SUB)
	{
		file_buffer<<"UMINUS\n";
	}
	else if(op==MUL)
	{
		file_buffer<<"UMULT\n";
	}
	else
	{
		file_buffer<<"UDIV\n";
	}
	file_buffer<<AST_NODE_NODE_SPACE<<"LHS (";
	if(operand->get_type()!="REL" && operand->get_type()!="ARITH")
	{
		operand->print_ast(file_buffer);
	}
	else
	{
		file_buffer<<"\n";
		operand->print_ast(file_buffer);
	}
	file_buffer<<")";
}

Eval_Result & Unary_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_operand = operand->evaluate(eval_env, file_buffer);
	if(result_operand.is_variable_defined()==false)
		report_error("Variable should be defined to be as an operand", NOLINE);
	
	if(node_data_type==int_data_type){
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(-1*result_operand.get_value());
		// print_ast(file_buffer);
		return result;
	}
	else{
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value(-1*result_operand.get_float_value());
		// print_ast(file_buffer);
		return result;
	}
}


/////////////////////////////////////////////////////////////////////////////////////

Cast_Ast::Cast_Ast(Ast * oper, Data_Type arg_type)
{
	operand = oper;
	datatype = arg_type;
	type = "CAST";
}

Cast_Ast::~Cast_Ast()
{
	delete operand;
}

Data_Type Cast_Ast::get_data_type()
{
	return datatype;
}

void Cast_Ast::print_ast(ostream & file_buffer)
{
	if(operand->get_type()!="REL" && operand->get_type()!="ARITH")
		operand->print_ast(file_buffer);
	else
	{
		file_buffer<<"\n";
		operand->print_ast(file_buffer);
	}
}

bool Cast_Ast::check_ast(int line)
{

}

string Cast_Ast::get_type()
{
	return type;
}

Eval_Result & Cast_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_operand = operand->evaluate(eval_env, file_buffer);

	if(datatype==int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		if(result_operand.get_result_enum()==int_result)
			result.set_value((int)result_operand.get_value());
		else
			result.set_value((int)result_operand.get_float_value());
		return result;
	}
	else
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		if(result_operand.get_result_enum()==int_result)
			result.set_value((float)result_operand.get_value());
		else
			result.set_value((float)result_operand.get_float_value());
		return result;
	}
}