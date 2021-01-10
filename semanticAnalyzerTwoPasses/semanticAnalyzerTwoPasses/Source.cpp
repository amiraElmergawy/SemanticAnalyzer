//Amira abd el-Azeem Mohamed el-Mergawy
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;
const int SIZE = 10; // array length (symbol table array)
const int MAX = 10;
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
enum DeclKind
{
	Variable_k, Proc_k, Undefined_k
};
enum CprocKind
{
	Cproc_k, CprocParams_k, Empty_k
};
enum ComdKind
{
	Assign_k, Read_k, Write_k, Ifthen_k, Ifelse_k, While_k, Call_k, CallWithParams_k,
	ComdBlock_k, Empty2_k
};
// Number | Name | Parn(Expr) | -(Expr) | Not(Expr) | OP(Expr ,Expr)
enum ExprKind
{
	Num_k, Name_k, Parn_k, Minos_k, Not_k, OP_k, Empty3_k
};
//OP <== OR | AND | <= | < | = | > | => | <> | + | - | * | /
enum OPKind
{
	Or_k, And_k, GE_k, LE_k, L_k, G_k, NE_k, Plus_k, Minus_k, Mult_k, Div_k,
	E_k, Empty4_k
};
class Token
{
public:
	tokenType type;
	string name;
	Token() { type = ERROR_SY; name = ""; }
	Token(string n, tokenType t)
	{
		this->name = n;
		this->type = t;
	}

};
enum Type
{
	int_k, bool_k, undefined, procedure_k
};
struct params
{
	Type t;
	string n;
	params(Type type, string name) {
		t = type;
		n = name;
	}
};
// return true if string was found & false if s wasn't found
bool searchInParamsVec(vector<params>v, string s)
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
// return true if empty & false if it wasn't
bool intersectParams(vector<params>p1, vector<params> p2) {
	for (int i = 0; i < p1.size(); i++) {
		if (searchInParamsVec(p2, p1[i].n))
		{
			cout << " semantics error : parameter declared more than once " << p1[i].n << endl;
			return false;
		}
	}
	return true;
}
vector<params> unionParams(vector<params>p1, vector<params> p2) {

	vector<params> hamada;
	for (int i = 0; i < p2.size(); i++) {
		p1.push_back(p2[i]);
	}
	hamada = p1;
	return hamada;
}

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
	void Insert(string s, Type t, vector<params> ps);
	Type get_type(string s);
	vector<params> get_parameters(string s);
	bool Search(string s);
	void union_symbol(SymbolTable s2);
	bool intersect(SymbolTable s2);

};
void SymbolTable::Insert(string s, Type t, vector<params> ps)
{
	int s_val = HashFunc(s);
	Symbol* p = new Symbol(s, t, ps);
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
//true if we found it
bool SymbolTable::Search(string s)
{
	bool flag = false;//we didn't find the name
	int s_val = HashFunc(s);
	Symbol* entry = htable[s_val];
	while (entry != NULL && !flag)
	{
		if (entry->name == s) flag = true;//if we find the name
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
	string sName = "";
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
enum NodeType
{
	Program_n, Block_n, Decl_n, Cproc_n, CprocWithParams_n, Type_n,
	Params_n, Comd_n, Expr_n
	, OP_n, DecSeq_n, ComSeq_n, NameList_n
	, ParamSeq_n,
	EmptyNode_n
};
class Node
{
public:
	Type TK;
	NodeType Type;
	string Name; // if token is id
	DeclKind DK;
	CprocKind CPK;
	ComdKind CK;
	ExprKind EK;
	OPKind OK;
	SymbolTable  S;
	vector<params> Parameters;
	vector<string> Names;
	Node* Left;
	Node* Right;
	Node* RRight;
	Node()
	{
		Type = EmptyNode_n;
		Name = "";
		//value = 0;
		DK = Undefined_k;
		CPK = Empty_k;
		TK = undefined;
		CK = Empty2_k;
		EK = Empty3_k;
		OK = Empty4_k;
		Left = Right = RRight = NULL;
		Parameters = {};
		Names = {};
	}
}*root;
class stack
{
	int top;
public:
	SymbolTable* S = new SymbolTable[SIZE];

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
		return top + 1;
	}
	// procedures for Nest stack
	bool Search(string s)
	{
		bool flag = false;//we didn't find the name
		for (int i = 0; i < this->stackSize(); i++)
		{
			if (S[i].Search(s)) return true;
		}
		return flag;
	}

	Type getType(string s) {

		for (int i = 0; i < this->stackSize(); i++) {
			if (S[i].Search(s))
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
	Type getExprType(Node* p) {
		//Num_k ,Name_k ,Parn_k ,Minos_k ,Not_k ,OP_k ,Empty3_k
		Type t = undefined;
		switch (p->EK)
		{
		case Num_k:
			t = int_k;
			break;
		case Name_k:
			t = this->getType(p->Name);// search then get the type
			break;
		case Parn_k:
			t = this->getExprType(p->Left);
			break;
		case Minos_k:
			//t = this->getExprType(p->Left);
			t = int_k;
			break;
		case Not_k:
			//t = this->getExprType(p->Left);
			t = bool_k;
			break;
		case OP_k:
			switch (p->OK)
			{   //Or_k ,And_k ,GE_k ,LE_k ,L_k ,G_k ,NE_k ,Plus_k ,Minus_k ,Mult_k ,Div_k , 
				//E_k, Empty4_k
			case Or_k:
				t = bool_k;
				break;
			case And_k:
				t = bool_k;
				break;
			case GE_k:
				t = bool_k;
				break;
			case LE_k:
				t = bool_k;
				break;
			case L_k:
				t = bool_k;
				break;
			case G_k:
				t = bool_k;
				break;
			case NE_k:
				t = bool_k;
				break;
			case E_k:
				t = bool_k;
				break;
			case Plus_k:
				t = int_k;
				break;
			case Minus_k:
				t = int_k;
				break;
			case Div_k:
				t = int_k;
				break;
			case Mult_k:
				t = int_k;
				break;
			default:
				t = undefined;
				break;
			}
		default:
			break;
		}
		return t;
	}
};

stack Nest;
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
	void display_tree(Node* Tree) {
		if (Tree)
		{
			cout << "node type : " << Tree->Type << "\n ";
			cout << "var name : " << Tree->Name << "\n ";
			//cout << "var value : " << Tree->value << "\n ";
			cout << "declaration kind : " << Tree->DK << "\n ";
			cout << "command kind : " << Tree->CK << "\n ";
			cout << "Type kind : " << Tree->TK << "\n ";
			cout << "expr kind : " << Tree->EK << "\n ";
			cout << "operation kind : " << Tree->OK << "\n ";
			cout << "procedure kind : " << Tree->CPK << "\n ";
			for (int i = 0; i < Tree->Names.size(); i++)
			{
				cout << "var name is : " << Tree->Names[i] << endl;
			}
			for (int i = 0; i < Tree->Parameters.size(); i++)
			{
				cout << "param type: " << Tree->Parameters[i].t << "param name: " << Tree->Parameters[i].n << endl;
			}
			cout << "**************************** end of node \n";
			display_tree(Tree->Left);
			display_tree(Tree->Right);
			display_tree(Tree->RRight);

		}
	}
	//<program> ::= <block>#
	void program()
	{
		current_token = getToken();
		root = block();
		match(END_SOURCE_SY);
	}
private:
	Token check_reserved(string s)
	{
		if (s == "begin")return Token("begin", BEGIN_SY);
		else if (s == "end")return Token("end", END_SY);
		else if (s == "integer")return Token("integer", INTEGER_SY);
		else if (s == "boolean")return Token("boolean", BOOLEAN_SY);
		else if (s == "read")return Token("read", READ_SY);
		else if (s == "write")return Token("write", WRITE_SY);
		else if (s == "if")return Token("if", IF_SY);
		else if (s == "endif")return Token("endif", EndIf_SY);
		else if (s == "then")return Token("then", THEN_SY);
		else if (s == "else")return Token("else", ELSE_SY);
		else if (s == "while")return Token("while", WHILE_SY);
		else if (s == "endwhile")return Token("endwhile", EndWHILE_SY);
		else if (s == "do")return Token("do", DO_SY);
		else if (s == "or")return Token("or", OR_SY);
		else if (s == "and") return Token("and", AND_SY);
		else if (s == "call")return Token("call", CALL_SY);
		else if (s == "proc")return Token("proc", PROC_SY);
		else if (s == "not")return Token("not", NOT_SY);
		else return Token(s, ID_SY);
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
				f.putback(ch);         //insert the last input again
				return check_reserved(s);     //check on this word if it is reserved word
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

	//************************
	//<block> ::= begin <declaration-seq> <command-seq> end
	Node* block() {
		Node* ptr = new Node();
		ptr->Type = Block_n;
		match(BEGIN_SY);
		ptr->Left = declaration_seq();
		ptr->Right = command_seq();
		match(END_SY);
		return ptr;
	}
	//*********************
	/*<declaration - seq> :: = <declartion>{ <declaration> }
		<declaration> :: = <type> <name - list> |
		proc <name> <cproc>*/
	Node* declaration_seq() {
		Node* ptr = declaration();
		//ptr->Type = Decl_n;
		Node* ptr1;
		while (current_token.type == INTEGER_SY || current_token.type == BOOLEAN_SY
			|| current_token.type == PROC_SY)
		{
			ptr1 = new Node();
			ptr1->Type = DecSeq_n;
			ptr1->Left = ptr;// to keep the arrangement of declarations
			ptr1->Right = declaration();
			ptr = ptr1;
		}
		return ptr;
	}
	//*****************
	//<declaration> :: = <type> <name - list> | proc <name> <cproc>
	Node* declaration() {
		Node* ptr = new Node();
		ptr->Type = Decl_n;
		if (current_token.type == INTEGER_SY || current_token.type == BOOLEAN_SY)
		{
			ptr->DK = Variable_k;
			ptr->Left = type();
			ptr->Right = name_list();
		}
		else if (current_token.type == PROC_SY)
		{
			ptr->DK = Proc_k;
			match(PROC_SY);
			ptr->Left = Name();
			ptr->Right = cproc();
		}
		else
			syntax_error(current_token);
		return ptr;
	}
	//*****************
	//<cproc> ::= = <command>| (<parameter-list>) = <command>
	/*<command> :: = <name> : = <expr> | read <name> | write <expr>
		| if <expr> then <command - seq>[else <command - seq>] endif
		| while <expr> do <command - seq> endwhile
		| call <name>[(<name - list>)] | <block>
		<name> ::= <letter> | <ident> <dORl>*/
	Node* cproc()
	{
		Node* ptr = new Node();
		if (current_token.type == EQUALS_SY)
		{
			match(EQUALS_SY);
			ptr->Type = Cproc_n;
			ptr->Left = command();
		}
		else if (current_token.type == LPARN_SY)
		{
			ptr->Type = CprocWithParams_n;
			match(LPARN_SY);
			ptr->Left = parameter_list();
			match(RPARN_SY);
			match(EQUALS_SY);
			ptr->Right = command();
		}
		else
			syntax_error(current_token);
		return ptr;
	}

	//*****************************
	//<type> ::= integer | boolean
	Node* type() {
		Node* ptr = new Node();
		ptr->Type = Type_n;
		if (current_token.type == INTEGER_SY)
		{
			ptr->TK = int_k;
			match(INTEGER_SY);
		}
		else if (current_token.type == BOOLEAN_SY)
		{
			ptr->TK = bool_k;
			match(BOOLEAN_SY);
		}
		else syntax_error(current_token);
		return ptr;
	}
	//*******************
	//<parameter-list> ::= <type> <name-list> {;<type> <name-list>}
	Node* parameter_list()
	{
		Node* ptr = new Node();
		ptr->Type = Params_n;
		ptr->Left = type();
		ptr->Right = name_list();

		if (current_token.type == SEMICOL_SY)
		{
			match(SEMICOL_SY);
			Node* ptr1 = new Node();
			ptr1->Type = ParamSeq_n;
			ptr1->Left = ptr;
			ptr1->Right = parameter_list();
			ptr = ptr1;
		}
		return ptr;
	}

	//********************
	//<name-list> ::= <name> <cname-list>
	Node* name_list() {
		Node* ptr = new Node();
		ptr->Type = NameList_n;
		ptr->Left = Name();
		ptr->Right = cname_list();
		return ptr;
	}
	//***********************
	//<cname-list> ::= $ | , <name-list>
	Node* cname_list()
	{
		Node* ptr;
		if (current_token.type == COMMA_SY)
		{
			match(COMMA_SY);
			ptr = name_list();
		}
		else ptr = NULL;
		return ptr;
	}
	//*********************** end of saving Data
	/*<command - seq> :: = <command>{ ; <command> }
		<command> :: = <name> : = <expr> | read <name> | write <expr>
		| if <expr> then <command - seq>[else <command - seq>] endif
		| while <expr> do <command - seq> endwhile
		| call <name>[(<name - list>)] | <block>*/
	Node* command_seq() {
		Node* ptr;
		ptr = command();
		Node* ptr1;
		while (current_token.type == SEMICOL_SY) {
			ptr1 = new Node();
			ptr1->Type = ComSeq_n;
			match(SEMICOL_SY);
			ptr1->Right = command();
			ptr1->Left = ptr;
			ptr = ptr1;
		}

		return ptr;
	}
	//********************
	//<command> :: = <name> : = <expr> | read <name> | write <expr>
	//	| if <expr> then <command - seq>[else <command - seq>] endif
	//	| while <expr> do <command - seq> endwhile
	//	| call <name>[(<name - list>)] | <block>
	Node* command() {
		Node* ptr = new Node();
		ptr->Type = Comd_n;
		if (current_token.type == ID_SY)
		{
			ptr->CK = Assign_k;
			ptr->Left = Name();
			match(ASSIGNMENT_SY);
			ptr->Right = expr();
		}
		else if (current_token.type == READ_SY)
		{
			ptr->CK = Read_k;
			match(READ_SY);
			ptr->Left = Name();
		}
		else if (current_token.type == WRITE_SY)
		{
			ptr->CK = Write_k;
			match(WRITE_SY);
			ptr->Left = expr();
		}
		else if (current_token.type == IF_SY)
		{
			ptr->CK = Ifthen_k;
			match(IF_SY);
			ptr->Left = expr();
			match(THEN_SY);
			ptr->Right = command_seq();
			if (current_token.type == ELSE_SY) {
				ptr->CK = Ifelse_k;
				match(ELSE_SY);
				ptr->RRight = command_seq();
			}
			match(EndIf_SY);
		}
		else if (current_token.type == WHILE_SY)
		{
			ptr->CK = While_k;
			match(WHILE_SY);
			ptr->Left = expr();
			match(DO_SY);
			ptr->Right = command_seq();
			match(EndWHILE_SY);
		}
		else if (current_token.type == CALL_SY)
		{
			ptr->CK = Call_k;
			match(CALL_SY);
			ptr->Left = Name();
			if (current_token.type == LPARN_SY)
			{
				ptr->CK = CallWithParams_k;
				match(LPARN_SY);
				ptr->Right = name_list();
				match(RPARN_SY);
			}
		}
		else if (current_token.type == BEGIN_SY) {
			ptr->CK = ComdBlock_k;
			ptr->Left = block();
		}
		else
		{
			ptr = NULL;
			syntax_error(current_token);
		}
		return ptr;
	}
	//********************
	//<expr> :: = <expr1>{ or <expr1> }
	//<expr1> :: = <expr2>{ and <expr2> }
	//<expr2> :: = <expr3> | not <expr>
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	Node* expr() {
		Node* ptr = new Node();
		//ptr->Type = Expr_n;
		ptr = expr1();
		Node* ptr1;
		while (current_token.type == OR_SY)
		{
			ptr1 = new Node();
			//ptr1->Type = Expr_n;
			ptr1->EK = OP_k;
			ptr1->OK = Or_k;
			match(OR_SY);
			ptr1->Left = ptr;
			ptr1->Right = expr1();
			ptr = ptr1;
		}
		return ptr;
	}
	//********************
	//<expr1> :: = <expr2>{ and <expr2> }
	//<expr2> :: = <expr3> | not <expr>
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	Node* expr1() {
		Node* ptr;
		ptr = expr2();
		Node* ptr1;
		while (current_token.type == AND_SY)
		{
			ptr1 = new Node();
			ptr1->Type = Expr_n;
			ptr1->EK = OP_k;
			ptr1->OK = And_k;
			match(AND_SY);
			ptr1->Left = ptr;
			ptr1->Right = expr2();
			ptr = ptr1;
		}
		return ptr;
	}
	//********************
	//<expr2> :: = <expr3> | not <expr>
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	//<relation> ::= <= | < | = | > | >= | <>
	Node* expr2() {
		Node* ptr = new Node();
		if (current_token.type == ID_SY ||
			current_token.type == INTEGER_SY ||
			current_token.type == MINUS_SY ||
			current_token.type == LPARN_SY)
		{
			ptr = expr3();
		}
		else if (current_token.type == NOT_SY)
		{
			match(NOT_SY);
			ptr = expr();
			ptr->EK = Not_k;
		}
		else syntax_error(current_token);
		return ptr;
	}
	//********************
	//<expr3> :: = <expr4>{ <relation> <expr4> }
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	//<relation> ::= <= | < | = | > | >= | <>
	Node* expr3() {
		Node* ptr;
		ptr = expr4();
		Node* ptr1;
		while (current_token.type == LOE_SY || current_token.type == LESS_SY || current_token.type == EQUALS_SY
			|| current_token.type == GREATER_SY || current_token.type == GOE_SY || current_token.type == NE_SY)
		{
			ptr1 = new Node();
			ptr1->Type = Expr_n;
			ptr1->EK = OP_k;
			ptr1->OK = relation();
			ptr1->Right = expr4();
			ptr1->Left = ptr;
			ptr = ptr1;
		}
		return ptr;
	}
	//********************
	//<expr4> :: = <term>{ <weak op> <term> }
	//<term> ::= <element> { <strong op> <element> }
	//<element> :: = <numeral> | <name> | (<expr>) | -<element>
	//<relation> ::= <= | < | = | > | >= | <>
	Node* expr4() {
		Node* ptr;
		ptr = term();
		Node* ptr1;
		//<weak op> ::= + | –
		while (current_token.type == PLUS_SY || current_token.type == MINUS_SY)
		{
			ptr1 = new Node();
			ptr1->EK = OP_k;
			ptr1->OK = weak_op();
			ptr1->Left = ptr;
			ptr1->Right = term();
			ptr = ptr1;
		}
		return ptr;
	}
	//*****************
	//<term> ::= <element> {<strong-op> <element>} 
	Node* term() {
		Node* ptr;
		ptr = element();
		Node* ptr1;
		//<STRONG op> ::= / | *
		while (current_token.type == DIVI_SY || current_token.type == MULT_SY)
		{
			ptr1 = new Node();
			ptr1->EK = OP_k;
			ptr1->OK = strong_op();
			ptr1->Left = ptr;
			ptr1->Right = element();
			ptr = ptr1;
		}
		return ptr;
	}
	//**********************
	//<element> ::= <numeral> | <name> | ( <expr> ) | – <element> 
	Node* element() {
		Node* ptr;
		ptr = new Node();
		ptr->Type = Expr_n;
		if (current_token.type == INTEGER_SY) {
			match(INTEGER_SY);
			//ptr->value = stoi(current_token.name);
			ptr->TK = int_k;
		}
		else if (current_token.type == ID_SY)
		{
			ptr = Name();
			
		}
		else if (current_token.type == LPARN_SY)
		{
			ptr->EK = Parn_k;
			match(LPARN_SY);
			ptr->Left = expr();
			match(RPARN_SY);
		}
		else if (current_token.type == MINUS_SY)
		{
			ptr->EK = Minos_k;
			match(MINUS_SY);
			ptr->Left = element();
		}
		else
		{
			ptr = NULL;
			syntax_error(current_token);
		}
		return ptr;
	}
	//********************
	//Or_k ,And_k ,GE_k ,LE_k ,L_k ,G_k ,NE_k ,Plus_k ,Minus_k ,Mult_k ,Div_k
	//<relation> ::= <= | < | = | > | >= | <>
	OPKind relation()
	{
		/*Node* ptr = new Node();
		ptr->Type = OP_n;*/
		if (current_token.type == LOE_SY)
		{
			match(LOE_SY);
			return LE_k;
		}
		else if (current_token.type == LESS_SY)
		{
			match(LESS_SY);
			return L_k;
		}
		else if (current_token.type == EQUALS_SY)
		{
			match(EQUALS_SY);
			return E_k;
		}
		else if (current_token.type == GREATER_SY)
		{
			match(GREATER_SY);
			return G_k;
		}
		else if (current_token.type == GOE_SY)
		{
			match(GOE_SY);
			return GE_k;
		}
		else if (current_token.type == NE_SY)
		{
			match(NE_SY);
			return NE_k;
		}
		else {
			syntax_error(current_token);
			return Empty4_k;
		}
	}
	//********************
	//<weak op> :: = +| -
	OPKind weak_op()
	{
		//Node* ptr = new Node();
		if (current_token.type == PLUS_SY)
		{
			match(PLUS_SY);
			return Plus_k;
		}
		else if (current_token.type == MINUS_SY)
		{
			match(MINUS_SY);
			return Minus_k;
		}
		else {
			syntax_error(current_token);
			return Empty4_k;
		}
		//return ptr;
	}
	//********************
	//<strong op> :: = *| /
	OPKind strong_op()
	{
		//Node* ptr = new Node();
		if (current_token.type == DIVI_SY)
		{
			match(DIVI_SY);
			return Div_k;
		}
		else if (current_token.type == MULT_SY)
		{
			match(MULT_SY);
			return Mult_k;
		}
		else {
			syntax_error(current_token);
			return Empty4_k;
		}
		//return ptr;
	}

	//*****************
	Node* Name()
	{
		Node* ptr = new Node();
		ptr->Type = Expr_n;
		ptr->Name = current_token.name;
		ptr->EK = Name_k;
		match(ID_SY);
		return ptr;
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

	void syntax_error(Token t) {
		cout << "syntax error " << t.name << " is unExpected token." << endl;
	}
};
//return true if s is found
bool searchInNames(vector<string> vs, string s) {
	for (int i = 0; i < vs.size(); i++) {
		if (vs[i] == s) {
			cout << " semantics error : Name is declared more than once " << s << endl;
			return true;
		}
	}
	return false;
}
void  Semantic(Node* tree)
{
	/*Program_n, Block_n, Decl_n, Cproc_n, CprocWithParams_n, Type_n,
		Params_n, Comd_n, Expr_n
		, OP_n, DecSeq_n, ComSeq_n, NameList_n
		, ParamSeq_n,
		EmptyNode_n*/
	if (tree)
	{
		switch (tree->Type)
		{
		case Program_n:
			Semantic(tree->Left);
			break;
		case Block_n:
		{
			Nest.push(tree->Left->S);
			Semantic(tree->Left);
			Semantic(tree->Right);
			Nest.pop();
		}
		break;
		case DecSeq_n:
		{
			tree->Left->S = tree->S;//you should pass the table to the left so when it fills it the nest also get filled
			Semantic(tree->Left);
			Semantic(tree->Right);
			//if (tree->S.intersect(tree->Left->S))//intersection is empty
			//{
			//	tree->S.union_symbol(tree->Left->S);
			//}
			//else
			//	cout << " Semanti Error : id declared > 1 \n";
			if (tree->S.intersect(tree->Right->S))//intersection is empty
			{
				tree->S.union_symbol(tree->Right->S);
			}
			else
				cout << " Semanti Error : id declared > 1 \n";
		}
		break;
		case Decl_n:
			switch (tree->DK)
			{
			case Variable_k: {
				Semantic(tree->Right);// cacluate the name list first
				tree->Names = tree->Right->Names;// and pass it to the tree
				//vector<string> names = Names(tree->Right);
				for (int i = 0; i < tree->Names.size(); i++) {
					if (!tree->S.Search(tree->Names[i]))
						tree->S.Insert(tree->Names[i], tree->Left->TK, {});
					//else//doesn't get to this part because serchInnames function remove the repeated 
					//	cout << "semantic error : you cannot define " << tree->Names[i] << " more than once " << endl;
				}
			}
						   break;
			case Proc_k: {
				if (!tree->S.Search(tree->Left->Name)) {

					if (tree->Right->Type == CprocWithParams_n)
					{
						Semantic(tree->Right->Left);
						//vector<params> parameters = Paramters(tree->Right->Left);//to access the parameters
						tree->S.Insert(tree->Left->Name, procedure_k, tree->Right->Left->Parameters);
						SymbolTable tableParam;
						for (int i = 0; i < tree->Right->Left->Parameters.size(); i++)
						{
							tableParam.Insert(tree->Right->Left->Parameters[i].n, tree->Right->Left->Parameters[i].t, {});
						}
						tree->Right->S = tree->S;
						Nest.push(tableParam);
						Semantic(tree->Right->Right);
						Nest.pop();
					}
					else {//procedure without parameters
						tree->S.Insert(tree->Left->Name, procedure_k, {});
						tree->Right->S = tree->S;// send stable of lift to its command as we might use it as recursive function 
						Semantic(tree->Right->Left);
					}
				}
				else
					cout << "semantic error : proc " << tree->Left->Name << " declared more than once " << endl;
			}
			}
		break;
		case ParamSeq_n:
		{
			Semantic(tree->Left);
			Semantic(tree->Right);
			if (intersectParams(tree->Left->Parameters, tree->Right->Parameters))
			{
				tree->Parameters = unionParams(tree->Left->Parameters, tree->Right->Parameters);
			}

		}
		break;
		case Params_n:
		{
			Semantic(tree->Right);
			vector<string> names = tree->Right->Names;
			for (int i = 0; i < names.size(); i++)
			{// for all names push name && type as a parameter
				tree->Parameters.push_back(params(tree->Left->TK, names[i]));
			}

		}
		break;
		case NameList_n:
		{
			tree->Names.push_back(tree->Left->Name);
			Node* saveNames = tree->Right;
			while (saveNames)
			{
				if (!searchInNames(tree->Names, saveNames->Left->Name)) {
					tree->Names.push_back(saveNames->Left->Name);
				}
				saveNames = saveNames->Right;
			}

		}
		break;
		case ComSeq_n:
		{
			tree->Left->S = tree->Right->S = tree->S; //malha4 lazme el mafrod (ela lw konte 7atha 3la4an el recursive f okay momkn 
			Semantic(tree->Left);
			Semantic(tree->Right);
		}
		break;
		case Comd_n:
		{
			switch (tree->CK)
			{
			case Assign_k: {
				//Type t1 = Nest.getType(tree->Left->Name); //to check left hand side type
				//Type t2 = Nest.getExprType(tree->Right);//to check expr type
				Semantic(tree->Left);
				Semantic(tree->Right);
				if (tree->Left->TK != tree->Right->TK)
					cout << "semantic error : left side of := statment must be in the same type of the right side \n";
			}
						 break;
			case Read_k: {
				Type t1 = Nest.getType(tree->Left->Name);
				if (t1 != int_k) {
					cout << " semantic error : The name occurring in a read command must be an integer variable \n";
				}
			}
					   break;
			case Ifthen_k: {
				Semantic(tree->Left);
				if (tree->Left->TK != bool_k) {
					cout << " semantic error : The expr occurring in if must be a boolan expr \n";
				}
				Semantic(tree->Right);
			}break;
			case Ifelse_k: {
				Semantic(tree->Left);
				if (tree->Left->TK != bool_k) {
					cout << " semantic error : The expr occurring in ifElse statment must be a boolan expr \n";
				}
				Semantic(tree->Right);
				Semantic(tree->RRight);
			}break;
			case While_k: {
				Semantic(tree->Left);
				if (tree->Left->TK != bool_k) {
					cout << " semantic error : The expr occurring in while statment must be a boolan expr \n";
				}
				Semantic(tree->Right);
			}break;
			case Call_k:
				if (Nest.getType(tree->Left->Name) != Proc_k)
				{
					cout << " The <name> in call command must be of type proc. \n";
				}
				break;
			case CallWithParams_k: {
				// Rule 1 : proc must be defined as procedure before
				// to check # of parameters and types
				string procName;
				/*vector<string> names;
				vector<params> parameters;*/
				procName = tree->Left->Name;
				if (Nest.getType(procName) == procedure_k)
				{
					vector <params> parameters = Nest.getPrameters(procName);
					Semantic(tree->Right);//nsete de 
					vector<string> names = tree->Right->Names;
					if (names.size() == parameters.size())
					{
						for (int i = 0; i < names.size(); i++)
						{
							if (Nest.getType(names[i]) != parameters[i].t)
							{
								cout << "semantic error : the type  of each name must match the type of the corresponding parameter \n ";
							}
						}
					}
					else cout << "semantic error :  parameters in call statment must equal to which identified in the procedure initialization \n";
				}
				else
					cout << " The name in call command must be of type procedure " << procName << "\n";
			}
								 break;
			case ComdBlock_k:
				Semantic(tree->Left);
				break;
			default:
				break;
			}
		}
		break;
		case Expr_n:
		{//Num_k, Name_k, Parn_k, Minos_k, Not_k, OP_k, Empty3_k
			switch (tree->EK)
			{
			case Parn_k:
				Semantic(tree->Left);
				tree->TK = tree->Left->TK;
				break;
			case Num_k:
				tree->TK = int_k;
				break;
			case Name_k :
				tree->TK = Nest.getType(tree->Name);
				break;
			case OP_k:
				Type tExpr1, tExpr2;
				tExpr1 = Nest.getExprType(tree->Left);
				tExpr2 = Nest.getExprType(tree->Right);
				switch (tree->OK)
				{//Or_k, And_k, GE_k, LE_k, L_k, G_k, NE_k, Plus_k, Minus_k, Mult_k, Div_k,
					//E_k, Empty4_k
				case Or_k:
					if (tExpr1 == tExpr2)
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case And_k:
					if (tExpr1 == tExpr2)
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case E_k:
					if (tExpr1 == tExpr2)
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case NE_k:
					if (tExpr1 == tExpr2)
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case GE_k:
					if ((tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case LE_k:
					if ((tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case L_k:
					if ((tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case G_k:
					if ((tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = bool_k;
					else
						tree->TK = undefined;
					break;
				case Plus_k:
					if ((tExpr1 == int_k)&& (tExpr2 == int_k))
						tree->TK = int_k;
					else
						tree->TK = undefined;
					break;
				case Minus_k:
					if ((tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = int_k;
					else
						tree->TK = undefined;
					break;
				case Div_k:
					if ( (tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = int_k;
					else
						tree->TK = undefined;
					break;
				case Mult_k:
					if ((tExpr1 == int_k) && (tExpr2 == int_k))
						tree->TK = int_k;
					else
						tree->TK = undefined;
					break;
				default:
					tree->TK = Nest.getExprType(tree->Left);
					break;
				}
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
}
void main()
{
	Parser p("test.txt");
	p.program();
	//p.display_tree(root);
	Semantic(root);
	system("pause");

}