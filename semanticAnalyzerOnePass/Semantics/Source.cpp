#include<iostream>
#include<string>
#include<fstream>
#include<ctype.h>
#include<vector>
using namespace std;
//Semantic analyzer Amira abdel-Azeem
enum tokenType
{
	BEGIN_SY, END_SY, INTEGER_SY, BOOLEAN_SY, READ_SY,
	WRITE_SY, IF_SY, EndIf_SY, THEN_SY, ELSE_SY, WHILE_SY, EndWHILE_SY, DO_SY, OR_SY, AND_SY
	, NOT_SY, LPARN_SY, RPARN_SY, COMMA_SY, MINUS_SY, LOE_SY,
	LESS_SY, EQUALS_SY, GREATER_SY, GOE_SY, NE_SY, PLUS_SY,
	MULT_SY, DIVI_SY, SEMICOL_SY,
	END_SOURCE_SY, CALL_SY, PROC_SY,
	ID_SY, ASSIGNMENT_SY,
	ERROR_SY
};
const int SIZE = 10; // array length (symbol table array)
//const int MAX = 10;
enum Type { integer, boolean, procedure, undefined};
struct params 
{
	Type t;
	string n;
	params(Type type, string name) {
		t = type;
		n = name;
	}
};
//struct Names {
//	string names [];
//};
bool searchInVectors(vector<params>v, string s)
{
	bool flag = false; // it means that we didn't find the name in the vector
	for (int i = 0; i < v.size(); i++)
	{
		if (s == v[i].n) {
			flag = true;// it means that we found the name in the vector
			return flag;
		}
	}
	return flag;
}
class Token
{
public:
	tokenType type;
	string name;
	Token() {};
	Token(string s, tokenType type) {
		name = s;
		this->type = type;
	}

};

class Symbol
{
public:
	string name;
	Type type;
	//vector<string> Names;
	vector<params> param;
	Symbol* next;
	Symbol(string n, Type v, vector<params> p) // Constructor
	{
		name = n;
		type = v;
		param = p;
		//if (v == 2) // if we define a procedure
		//{
		//	for (int i = 0; i < p.size; i++)
		//	{
		//		this->param[i] = p[i]; // get the parameters
		//	}
		//	
		//}
		next = NULL;
	}
};

class SymbolTable
{
private:
	Symbol** htable; // array of linked list
public:
	SymbolTable()
		// Constructor that creates a symbol table
		// all indecies will be initialized with null value
	{
		htable = new Symbol * [SIZE];
		for (int i = 0; i < SIZE; i++)
			htable[i] = NULL;
	}
	int HashFunc(string word) // Hash function used
	{
		int sum = 0;
		for (int k = 0; k < word.length(); k++)
			sum = sum + int(word[k]); // convert all characters of the word to integers(ascii code) and sum them to get the key
		return sum % SIZE;
	}
	void Insert(string s, Type t , vector<params> ps);
	Type get_type(string s);
	vector<params> get_parameters(string s);
	bool Search(string s);
	void union_symbol(SymbolTable s2);
	bool intersect(SymbolTable s2);

};
void SymbolTable :: Insert(string s, Type t , vector<params> ps)
{
	int s_val = HashFunc(s);
	Symbol* p = new Symbol(s, t ,ps);
	Symbol* entry = htable[s_val];
	if (entry == NULL)
		htable[s_val] = p;
	else
	{
		while (entry->next != NULL)
			entry = entry->next; // untill find (last) empty place
		entry->next = p;
	}
}
Type SymbolTable::get_type(string s)
{
	int s_val = HashFunc(s);
	Symbol* entry = htable[s_val];
	while (entry != NULL)
	{
		if (entry->name == s) return entry->type;
		else entry = entry->next;
	}
	return undefined;

}
vector<params> SymbolTable::get_parameters(string s)
{
	int s_val = HashFunc(s);
	Symbol* entry = htable[s_val];
	while (entry != NULL)
	{
		if (entry->name == s) return entry->param;
		else entry = entry->next;
	}
	return {}; 

}
bool SymbolTable::Search(string s)
{
	bool flag = false;
	int s_val = HashFunc(s);
	Symbol* entry = htable[s_val];
	while (entry != NULL && !flag)
	{
		if (entry->name == s) flag = true;
		else entry = entry->next;
	}
	return flag;
}
void SymbolTable::union_symbol(SymbolTable s2)
{
	Symbol* entry;
	for (int i = 0; i < SIZE; i++)
	{
		entry = s2.htable[i];
		while (entry != NULL)
		{
			Insert(entry->name, entry->type, entry->param);
			entry = entry->next;
		}
	}
}
bool SymbolTable::intersect(SymbolTable s2)
{
	Symbol* entry;
	string sName;
	for (int i = 0; i < SIZE; i++)
	{
		entry = s2.htable[i];
		if (entry == NULL)
			continue;
		sName = entry->name;
		if (this->Search(sName))
		{
			cout << "repeated var in intersect : " << sName << endl;
			return false;//if the intersection is empty
		}
	}
	return true;//if the intersection is not empty
};
class stack
{
	int top;
public:
	SymbolTable* S = new SymbolTable [SIZE];    

	stack() { top = -1; }
	void push(SymbolTable x)
	{
		if (top >= (SIZE - 1))
		{
			cout << "Stack Overflow";
			//return false;
		}
		else
		{
			S[++top] = x;
			cout << " pushed into stack\n";
			//return true;
		}
	}

	SymbolTable pop()
	{
		if (top < 0)
		{
			SymbolTable s;
			cout << "Stack Underflow" << endl;
			return s;
		}
		else
		{
			cout << "popped" << endl;
			SymbolTable x = S[top--];
			//top--;
			return x;
		}
	}
	bool isEmpty()
	{
		return (top < 0);
	}
	SymbolTable Top() {
		return S[top];
	}
	int stackSize() {
		return top+1;
	}
	// procedures for Nest stack

	Type getType(string s) {

		for (int i = 0; i < this->stackSize(); i++) {
			if(S[i].Search(s))
				return S[i].get_type(s);
		}
		return undefined;
	}

	vector<params> getPrameters(string s) {
		for (int i = 0; i < this->stackSize(); i++) {
			if (S[i].Search(s)) {
				return S[i].get_parameters(s);
			}
		}
		return {};
	}
};

stack Nest ; 

class Parser
{
private:
	ifstream f;
	Token current_token;
	//*************************

public:
	Parser(string name) {
		f.open(name);
		if (!f)
		{
			cerr << "Unable to open file" << endl;
			system("pause");
			exit(1);
		}
	}
	~Parser() {
		f.close();
	}
	//SymbolTable s;
	void sample_parser() //<SP>::=<PROGRAM> #x
	{
		current_token = getToken();
		program();
		match(END_SOURCE_SY);
	}
private:
	Token check_reserved(string s)
	{
		if (s == "begin")return Token("begin",BEGIN_SY);
		else if (s == "end")return Token("end",END_SY);
		else if (s == "integer")return Token("integer",INTEGER_SY);
		else if (s == "boolean")return Token("boolean",BOOLEAN_SY);
		else if (s == "read")return Token("read",READ_SY);
		else if (s == "write")return Token("write",WRITE_SY);
		else if (s == "if")return Token("if",IF_SY);
		else if (s == "endif")return Token("endif",EndIf_SY);
		else if (s == "then")return Token("then",THEN_SY);
		else if (s == "else")return Token("else",ELSE_SY);
		else if (s == "while")return Token("while",WHILE_SY);
		else if (s == "endwhile")return Token("endwhile",EndWHILE_SY);
		else if (s == "do")return Token("do",DO_SY);
		else if (s == "or")return Token("or",OR_SY);
		else if (s == "and") return Token("and",AND_SY);
		else if (s == "call")return Token("call",CALL_SY);
		else if (s == "proc")return Token("proc",PROC_SY);
		else if (s == "not")return Token("not",NOT_SY);
		else return Token(s,ID_SY);
	}
	//**************************
	Token getToken(void) {
		char ch;
		string s = "";
		f.get(ch);
		while (isspace(ch) && !f.eof())   //To ignore spaces
		{
			f.get(ch);
		}
		if (f.eof()) return Token("#", END_SOURCE_SY);
		//f.get(ch);
		if (!isspace(ch))
		{
			if (ch == '=')  return Token("=", EQUALS_SY);
			else if (ch == ',')  return Token(",", COMMA_SY);
			else if (ch == ';')  return Token(";", SEMICOL_SY);
			else if (ch == '(')return Token("(", LPARN_SY);
			else if (ch == ')')return Token(")", RPARN_SY);
			else if (ch == '+')  return Token("+", PLUS_SY);
			else if (ch == '-')  return Token("-", MINUS_SY);
			else if (ch == '/')  return Token("/", DIVI_SY);
			else if (ch == '*')  return Token("*", MULT_SY);
			else if (ch == '#')  return Token("#", END_SOURCE_SY);
			else if (ch == ':')
			{
				f.get(ch);
				if (ch == '=')  return Token("=", ASSIGNMENT_SY);
				else
				{
					f.putback(ch);
					return Token("error", ERROR_SY);
				}
			}
			else if (ch == '<')
			{
				f.get(ch);
				if (ch == '=')return Token("<=", LOE_SY);
				else if (ch == '>')
				{
					return Token("<>", NE_SY);
				}
				else {
					f.putback(ch);
					return Token("<", LESS_SY);
				}
			}
			else if (ch == '>')
			{
				f.get(ch);
				if (ch == '=')return Token(">=", GOE_SY);
				else {
					f.putback(ch);
					return Token(">", GREATER_SY);
				}
			}
			else if (isalpha(ch))
			{
				s = ch;
				f.get(ch);
				while (isalnum(ch))
				{
					s += ch;
					f.get(ch);
				}
				f.putback(ch); //insert the last input again
				Token t = check_reserved(s);
				return t;     //check on this word if it is reserved word
			}
			else if (isdigit(ch))
			{
				s = ch;
				f.get(ch);
				while (isdigit(ch))
				{
					s += ch;
					f.get(ch);
				}
				f.get(ch);
				if (!isdigit(ch))
				{
					f.putback(ch);
					return Token(s, INTEGER_SY);
				}
			}

		}
		// else return ERROR_SY;
		//return END_SOURCE_SY;
	}



	//**************************
	//<program> ::= <block>
	void program()//edit
	{
		block();
	}
	//************************    
	//<block> ::= begin <declaration-seq> <command-seq> end
	void block() { /* Rule : A  <name> contained in a command C must either be declared in some <block> containing C or listed in the <parameter-list> of some procedure containing C.
				   If there is more than one such defining occurrence of the <name>, 
				   it is the innermost one that applies in C.
				   */
		SymbolTable s;
  		match(BEGIN_SY);
		Nest.push(s);
		declaration_seq(s);
		command_seq();
		Nest.pop();
		match(END_SY);
	}
	//*********************
	/*<declaration - seq> :: = <declartion>{ <declaration> }
		<declaration> :: = <type> <name - list> |
		proc <name> <cproc>*/
	void declaration_seq(SymbolTable & s) {
		declaration(s);
		while (current_token.type == INTEGER_SY || current_token.type == BOOLEAN_SY || 
			current_token.type == PROC_SY)
		{
		SymbolTable s1;
			declaration(s1);
			if (s.intersect(s1)) 
				s.union_symbol(s1);    
			else 
				cout << " semantic error : name may be declared at most once in the declaration  "<<endl; 
			
		}
	}
	//*****************
	//<declaration> :: = <type> <name - list> | proc <name> <cproc>
	void declaration(SymbolTable & s) {
		if (current_token.type == INTEGER_SY || current_token.type == BOOLEAN_SY)
		{
			vector<string> Names;
			Type t= type();
		//	name_list();//need to return a vector of names
			Names = name_list();
			for (int i = 0; i < Names.size();i++) {
				if (!s.Search(Names[i]))
					s.Insert(Names[i], t, {});
				else
					cout << "Semantic error repeated var :" << Names[i] << endl;
			}
		}
		else if (current_token.type == PROC_SY)
		{
			string procName;
			match(PROC_SY);
			procName = Name(); //will return the name of procedure to send it to cproc
			cproc(procName, s);

		}
		else
			syntax_error(current_token);
	}
	//*****************
	//<cproc> ::= = <command>| (<parameter-list>) = <command>
	/*<command> :: = <name> : = <expr> | read <name> | write <expr>
		| if <expr> then <command - seq>[else <command - seq>] endif
		| while <expr> do <command - seq> endwhile
		| call <name>[(<name - list>)] | <block>
		<name> ::= <letter> | <ident> <dORl>*/
	void cproc(string name, SymbolTable& s)
	{
		vector<params> parameters;
		/*if (current_token.type == ID_SY || current_token.type == READ_SY || current_token.type == WRITE_SY ||
			current_token.type == IF_SY || current_token.type == WHILE_SY || current_token.type == CALL_SY || current_token.type == BEGIN_SY)
		{*/
		if(current_token.type==EQUALS_SY)
		{
			match(EQUALS_SY);
			command();
		}
		else if (current_token.type == LPARN_SY)
		{
			match(LPARN_SY);
			parameters = parameter_list();
			match(RPARN_SY);
			//now we required all the needed information for the proc
			//we need it in the table now
			s.Insert(name, procedure, parameters);
			match(EQUALS_SY);
			SymbolTable tableParam;
			for (int i = 0; i < parameters.size(); i++)
			{
				tableParam.Insert(parameters[i].n, parameters[i].t, {});
			}
			Nest.push(tableParam);
			command();
			Nest.pop();
		}
		else
			syntax_error(current_token);
	}

	//*****************************
	//<type> ::= integer | boolean
	Type type() {//return the type
		if (current_token.type == INTEGER_SY)
		{
			match(INTEGER_SY);
			return integer ;
		}
		else if (current_token.type == BOOLEAN_SY)
		{
			match(BOOLEAN_SY);
			return boolean;
		}
		else syntax_error(current_token);
		return undefined ;
	}
	//*******************
	//<parameter-list> ::= <type> <name-list> {; <type> <name-list>}
	vector<params> parameter_list()
	{
		vector<params> parameters;	
		vector<string> Names;
		Type t;
		t = type();
		Names = name_list();
		for (int i = 0; i < Names.size(); i++) {
			if (!searchInVectors(parameters , Names[i]))
			{
				parameters.push_back(params(t, Names[i]));
			}
			else cout << "Semantic Error you can't identify the id more than once " << endl;
			
		}
		while (current_token.type == SEMICOL_SY)
		{
			match(SEMICOL_SY);
			t = type();
			Names = name_list();
			for (int i = 0; i < Names.size(); i++) {
				if (!searchInVectors(parameters, Names[i]))
				{
					parameters.push_back(params(t, Names[i]));
				}
				else cout << "Semantic Error you can't identify the id more than once " << endl;
			}
		}
		return parameters;
	}
	//********************
	//<name-list> ::= <name> {, <name>}
	vector<string> name_list() {
		vector<string>namesHolder;
		string s = Name();
		namesHolder.push_back(s);
		while (current_token.type == COMMA_SY) {
			match(COMMA_SY);
			namesHolder.push_back(Name());
		}
		return namesHolder;
	}
	//***********************
	/*<command - seq> :: = <command>{ ; <command> }
		<command> :: = <name> : = <expr> | read <name> | write <expr>
		| if <expr> then <command - seq>[else <command - seq>] endif
		| while <expr> do <command - seq> endwhile
		| call <name>[(<name - list>)] | <block>*/
	void command_seq() {
		command();
		while (current_token.type == SEMICOL_SY) {
			match(SEMICOL_SY);
			command();
		}


	}
	//********************
	//<command> :: = <name> : = <expr> | read <name> | write <expr>
	//	| if <expr> then <command - seq>[else <command - seq>] endif
	//	| while <expr> do <command - seq> endwhile
	//	| call <name>[(<name - list>)] | <block>
	void command() {
		Type t1, t2; // to get the type of expr and Name-Type
		if (current_token.type == ID_SY) //rule : left side of := statment must be in the same type of the right side
		{
			t1 = Nest.getType(Name());
			match(ASSIGNMENT_SY);
			expr(t2);
			if (t1 != t2)
			{
				cout << " semantic error : the two sides of assignment statment must be in the same type \n";
			}
		}
		else if (current_token.type == READ_SY) // Rule : The name occurring in a read command must be an integer variable
		{
			match(READ_SY);
			t1 = Nest.getType(Name());
			if (t1 != integer) {
				cout << " semantic error : The name occurring in a read command must be an integer variable \n";
			}
		}
		else if (current_token.type == WRITE_SY)
		{
			match(WRITE_SY);
			expr(t1);
		}
		else if (current_token.type == IF_SY)
		{
			match(IF_SY);
			expr(t1);
			if (t1 != boolean) {
				cout << " semantic error : The name occurring in if must be a boolan expr \n";
			}
			match(THEN_SY);
			command_seq();
			if (current_token.type == ELSE_SY) {
				match(ELSE_SY);
				command_seq();
			}
			match(EndIf_SY);
		}
		else if (current_token.type == WHILE_SY)
		{
			match(WHILE_SY);
			expr(t1);
			if (t1 != boolean) {
				cout << " semantic error : The name occurring in while must be a boolan expr \n";
			}
			match(DO_SY);
			command_seq();
			match(EndWHILE_SY);
		}
		else if (current_token.type == CALL_SY) /*Rule 1 : The <name> in call command must be of type proc. 
												  Rule 2 : If the procedure so named has parameters,
												  an equal number of <name>s must be present in the call , 
												  and the type  of each name must match the type of the corresponding parameter.*/
		{
			string procName; // Rule 1
			vector<string> Names;
			vector<params> parameters;
			match(CALL_SY);
			procName = Name();
			if (Nest.getType(procName) != procedure)
			{
				cout << " The <name> in call command must be of type proc. \n";
			}
			if (current_token.type == LPARN_SY) {
				parameters = Nest.getPrameters(procName);
				match(LPARN_SY);
				Names = name_list();
				if (Names.size() == parameters.size())
				{
					for (int i = 0; i < Names.size(); i++)
					{
						if (Nest.getType(Names[i]) != parameters[i].t)
						{
							cout << "semantic error : the type  of each name must match the type of the corresponding parameter \n ";
						}
					}
				}
				else cout << "semantic error :  parameters in call statment must equal to which identified in the procedure initialization \n";
				match(RPARN_SY);
			}
		}
		else if (current_token.type == BEGIN_SY) {
			block();
		}
		else
		{
			syntax_error(current_token);
		}
	}
	//********************
	//<expr> :: = <expr1>{ or <expr1> }
	//<expr1> :: = <expr2>{ and <expr2> }
	//<expr2> :: = <expr3> | not <expr>
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	void expr(Type &type) {
		Type t1, t2;
		expr1(type);//get the type
		t1 = type;
		while (current_token.type == OR_SY) // Rule : A  <name> occurring as a Boolean element must be a Boolean variable. 
		{
			match(OR_SY);
			expr1(type);
			t2 = type;
			if((t1 != boolean) && (t2 != boolean))
			{
				cout << " A  <name> occurring as a Boolean element must be a Boolean variable. \n";
			}
		}
	}
	//********************
	//<expr1> :: = <expr2>{ and <expr2> }
	//<expr2> :: = <expr3> | not <expr>
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	void expr1(Type &type) {
		Type t1, t2;
		expr2(type);
		t1 = type;
		while (current_token.type == AND_SY) // Rule : A  <name> occurring as a Boolean element must be a Boolean variable. 
		{
			//same
			match(AND_SY);
			expr2(type);
			t2 = type;
			if ((t1 != boolean) && (t2 != boolean))
			{
				cout << " A  <name> occurring as a Boolean element must be a Boolean variable. \n";
			}
		}
	}
	//********************
	//<expr2> :: = <expr3> | not <expr>
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	//<relation> ::= <= | < | = | > | >= | <>
	void expr2(Type &type) {
		Type t;
		if (current_token.type == ID_SY ||
			current_token.type == INTEGER_SY ||
			current_token.type == MINUS_SY ||
			current_token.type == LPARN_SY)
		{
			expr3(type);
		}
		else if (current_token.type == NOT_SY) // Rule : A  <name> occurring as a Boolean element must be a Boolean variable.
		{
			match(NOT_SY);
			expr(type);
			t = type;
			if (t != boolean)
			{
				cout << " A  <name> occurring as a Boolean element must be a Boolean variable. in not statment \n";
			}
		}
		else syntax_error(current_token);
	}
	//********************
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	//<relation> ::= <= | < | = | > | >= | <>
	void expr3(Type &type) {
		Type t1, t2;
		expr4(type);
		t1 = type;
		while (current_token.type == LOE_SY || current_token.type == LESS_SY || current_token.type == EQUALS_SY
			|| current_token.type == GREATER_SY || current_token.type == GOE_SY || current_token.type == NE_SY)
			// Rule :  A  <name> occurring as an (integer) element must be an integer variable. 
		{
			relation();
			expr4(type);
			t2 = type;
			if ((t1 != integer) && (t2 != integer))
			{
				cout << " A  <name> occurring as an integer element must be an integer variable. \n";
			}
			type = boolean;//reset the type variable
		}
	}
	//********************
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	//<relation> ::= <= | < | = | > | >= | <>
	void expr4(Type &type) {
		Type t1, t2;
		term(type);
		t1 = type;
		//<weak op> ::= + | –
		while (current_token.type == PLUS_SY || current_token.type== MINUS_SY)
		{
			weak_op();
			term(type);
			t2 = type;
			if ((t1 != integer) && (t2 != integer))
			{
				cout << " A  <name> occurring as an integer element must be an integer variable. \n";
			}
			type = boolean;//reset the type variable

		}
	}
	//*****************
	//<term> ::= <element> {<strong-op> <element>} 
	void term(Type &type) {
		Type t1, t2;
		element(type);
		t1 = type;
		//<STRONG op> ::= / | *
		while (current_token.type == DIVI_SY || current_token.type == MULT_SY)
		{
			strong_op();
			element(type);
			t2 = type;
			if ((t1 != integer) && (t2 != integer))
			{
				cout << " A  <name> occurring as an integer element must be an integer variable. \n";
			}

		}
	}
	//**********************
	//<element> ::= <numeral> | <name> | ( <expr> ) | – <element> 
	void element(Type &type) {
		if (current_token.type == INTEGER_SY) {
			match(INTEGER_SY);
			type = integer;
		}
		else if (current_token.type == ID_SY)
		{
			type = Nest.getType(Name());
			if (type == undefined)
				cout << "semantic error undeclared var" << endl;
		}
		else if (current_token.type == LPARN_SY)
		{
			match(LPARN_SY);
			expr(type);
			match(RPARN_SY);
		}
		else if (current_token.type == MINUS_SY)
		{
			match(MINUS_SY);
			element(type);
		}
		else
		{
			syntax_error(current_token);
		}
	}
	//********************
	//<relation> ::= <= | < | = | > | >= | <>
	void relation()
	{
		if (current_token.type == LOE_SY)
		{
			match(LOE_SY);
		}
		else if (current_token.type == LESS_SY)
		{
			match(LESS_SY);
		}
		else if (current_token.type == EQUALS_SY)
		{
			match(EQUALS_SY);
		}
		else if (current_token.type == GREATER_SY)
		{
			match(GREATER_SY);
		}
		else if (current_token.type == GOE_SY)
		{
			match(GOE_SY);
		}
		else if (current_token.type == NE_SY)
		{
			match(NE_SY);
		}
		else syntax_error(current_token);
	}
	//********************
	//<weak op> :: = +| -
	void weak_op()
	{
		if (current_token.type == PLUS_SY)
		{
			match(PLUS_SY);
		}
		else if (current_token.type == MINUS_SY)
		{
			match(MINUS_SY);
		}
		else syntax_error(current_token);
	}
	//********************
	//<strong op> :: = *| /
	void strong_op()
	{
		if (current_token.type == DIVI_SY)
		{
			match(DIVI_SY);
		}
		else if (current_token.type == MULT_SY)
		{
			match(MULT_SY);
		}
		else syntax_error(current_token);
	}

	//*****************
	string Name()
	{
		string name=current_token.name;
		match(ID_SY);
		return name;
	}
	//***********************

	void match(tokenType terminal) {
		if (current_token.type == terminal)
		{
			cout << current_token.name << " is matched" << endl;
			current_token = getToken();
		}
		else {
			syntax_error(current_token);
		}

	}
	//*****************************
	/*string name(Token t) {
		switch (t.type)
		{
		case ERROR_SY:  return "lexical error: illegal token"; break;
		case ID_SY:return "id token"; break;
		case PLUS_SY:return"plus token"; break;
		case MULT_SY:return"multe token"; break;
		case LPARN_SY:return"left p token"; break;
		case RPARN_SY:return "right P token"; break;
		case  END_SOURCE_SY:return "end source token"; break;
		case BEGIN_SY: return "begin token"; break;
		case END_SY: return "end token"; break;
		case MINUS_SY: return "- token"; break;
		case DIVI_SY: return "/ token"; break;
		case LOE_SY: return "<= token"; break;
		case LESS_SY: return "< token"; break;
		case EQUALS_SY: return "= token"; break;
		case GREATER_SY: return "> token"; break;
		case GOE_SY: return ">= token"; break;
		case NE_SY: return "<> token"; break;
		case INTEGER_SY: return "integer token"; break;
		case BOOLEAN_SY: return "boolean  token"; break;
		case READ_SY: return "read token"; break;
		case WRITE_SY: return "write token"; break;
		case IF_SY: return "if token"; break;
		case THEN_SY: return "then token"; break;
		case ELSE_SY: return "else token"; break;
		case DO_SY:return "do token"; break;
		case WHILE_SY: return "while token"; break;
		case COMMA_SY: return ", token"; break;
		case ASSIGNMENT_SY: return ":= token"; break;
		case SEMICOL_SY: return"; token"; break;
		case AND_SY: return "and token"; break;
		case OR_SY: return "or token"; break;
		case NOT_SY: return "not token"; break;
		case CALL_SY: return "call token"; break;
		case PROC_SY: return "proc token"; break;
		case EndWHILE_SY: return "endwhile token"; break;
		case EndIf_SY: return "endif token"; break;
		default: return "ERROR_SY";
		}
		return "end_of_file ";
	}*/
	//****************************

	void syntax_error(Token t) {
		cout << t.name << " is unExpected token" << endl;
	}
};
void main()
{
	SymbolTable test;
	//test.HashFunc("test");
	Parser pa("Text.txt");
	pa.sample_parser();
	system("pause");


}

