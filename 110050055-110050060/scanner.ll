
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

%filenames="scanner"
%lex-source="scanner.cc"

%%

int		{
			store_token_name("INTEGER");
			return Parser::INTEGER; 
		}

float 	{
			store_token_name("FLOAT");
			return Parser::FLOAT;
		}

double	{
			store_token_name("DOUBLE");
			return Parser::DOUBLE;
		}

return		{ 
			store_token_name("RETURN");
			return Parser::RETURN; 
		}

if		{
			store_token_name("IF");
			return Parser::IF;
		}

goto	{
			store_token_name("GOTO");
			return Parser::GOTO;
		}

else	{
			store_token_name("ELSE");
			return Parser::ELSE;
		}
"!="	{
			store_token_name("NE");
			return Parser::NEP;
		}

"=="	{
			store_token_name("EQ");
			return Parser::EQP;
		}

"<"		{
			store_token_name("LT");
			return Parser::LTP;
		}

"<="		{
			store_token_name("LE");
			return Parser::LEP;
		}

">"		{
			store_token_name("GT");
			return Parser::GTP;
		}

">="		{
			store_token_name("GE");
			return Parser::GEP;
		}


[-]?[[:digit:]]+ 	{ 
				store_token_name("NUM");

				ParserBase::STYPE__ * val = getSval();
				val->integer_value = atoi(matched().c_str());

				return Parser::INTEGER_NUMBER; 
			}

[[:alpha:]_][[:alpha:][:digit:]_]* {
					store_token_name("NAME");

					ParserBase::STYPE__ * val = getSval();
					val->string_value = new std::string(matched());

					return Parser::NAME; 
				}

"<bb "[[:digit:]]+">"	{
				store_token_name("BASIC BLOCK");

				string bb_num_str = matched().substr(4, matched().length() - 2);
				CHECK_INPUT_AND_ABORT((atoi(bb_num_str.c_str()) >= 2), "Illegal basic block lable", lineNr());

				ParserBase::STYPE__ * val = getSval();
				val->integer_value = atoi(bb_num_str.c_str());

				return Parser::BBNUM;
			}

"="	{
		store_token_name("ASSIGN_OP");
		return Parser::ASSIGN;
	}

[:{}();]	{
			store_token_name("META CHAR");
			return matched()[0];
		}


\n    		|
";;".*  	|
[ \t]*";;".*	|
[ \t]*"//".*	|
[ \t]		{
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}

.		{ 
			string error_message;
			error_message =  "Illegal character `" + matched();
			error_message += "' on line " + lineNr();
			
			CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
		}
