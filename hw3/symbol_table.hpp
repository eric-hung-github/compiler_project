#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

fstream jasm_file;
void jasm(string s)
{
    jasm_file << s << endl;
}

enum IDType
{
    ID_ERROR = 0,
    ID_CONST,
    ID_VAR,
    ID_ARR,
    ID_PROGRAM,
    ID_FUNC,
    ID_PROCEDURE,
    ID_ARG,
};

enum ValueType
{
    VALUE_NONE = 0,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_BOOL,
    VALUE_STR,
    VALUE_VOID,

};

string IDTypeToString(int type)
{

    switch (type)
    {
    case ID_ERROR:
        return "ID_ERROR";
        break;
    case ID_CONST:
        return "constant";
        break;
    case ID_VAR:
        return "variable";
        break;
    case ID_ARR:
        return "array";
        break;
    case ID_PROGRAM:
        return "program";
        break;
    case ID_FUNC:
        return "function";
        break;
    case ID_PROCEDURE:
        return "procedure";
        break;
    case ID_ARG:
        return "argument";
        break;

    default:
        return "error type";
        break;
    }
}

string ValueTypeToString(int type)
{

    switch (type)
    {
    case VALUE_NONE:
        return "VALUE_NONE";
        break;
    case VALUE_INT:
        return "int";
        break;
    case VALUE_FLOAT:
        return "float";
        break;
    case VALUE_BOOL:
        return "boolean";
        break;
    case VALUE_STR:
        return "string";
        break;
    case VALUE_VOID:
        return "void";
        break;
    default:
        return "error type";
        break;
    }
}

// Value *GetValue(int type,)

struct Value
{
    int value_type = VALUE_NONE;

    int int_value;
    float float_value;
    bool bool_value;
    string string_value;

public:
    string display()
    {
        switch (value_type)
        {
        case VALUE_INT:
            return to_string(int_value);
            break;
        case VALUE_FLOAT:
            return to_string(float_value);
            break;
        case VALUE_BOOL:
            return (bool_value) ? "true" : "false";
            break;
        case VALUE_STR:
            return string_value;
            break;
        case VALUE_VOID:
            return "VALUE_VOID";
            break;
        default:
            return "VALUE_NONE";
        }
    }
};

struct Symbol
{
    string name = "";
    int id_type = ID_ERROR;
    bool init = false;

    Value *value = new Value();
    vector<Symbol *> arguments;
    bool is_global = false;
    int counter = 0;
};

class SymbolTable
{
    string name = "";
    vector<Symbol *> symbols;

public:
    // bool is_global = false;

    SymbolTable(string table_name)
    {
        name = table_name;
        symbols.clear();
    }
    ~SymbolTable() {}

    int size()
    {
        return symbols.size();
    }

    bool insert(Symbol *id)
    {

        if (lookup(id->name) == nullptr)
        {
            symbols.push_back(id);
            return true;
        }
        else
        {
            // cout << "SymbolTable<" << name << "> insert exit: " << ID->name << endl;
            return false;
        }
    }

    Symbol *lookup(string name)
    {
        for (int i = 0; i < symbols.size(); i++)
        {
            if (symbols[i]->name == name)
            {
                return symbols[i];
            }
        }

        return nullptr;
    }

    void dump()
    {
        cout << "===================================================" << endl;
        cout << "Table: " << name << endl;
        cout << setw(20) << "name";
        cout << setw(20) << "id type" << endl;

        for (int i = 0; i < symbols.size(); i++)
        {
            cout << setw(20) << symbols[i]->name;

            cout << setw(20) << IDTypeToString(symbols[i]->id_type);

            cout << "(" << ValueTypeToString(symbols[i]->value->value_type) << ")";

            cout << endl;
        }
    }
};

class SymbolTableStack
{
    vector<SymbolTable> tableStack;

public:
    vector<Symbol *> argumentStack;
    vector<Value *> parseStack;
    Symbol *fun_ptr = nullptr;
    string program_name;

    SymbolTableStack()
    {
        tableStack.clear();
    }
    ~SymbolTableStack() {}

    int size()
    {
        return tableStack.size();
    }

    SymbolTable *top()
    {
        return &tableStack[size() - 1];
    }

    void push(string table_name)
    {
        tableStack.push_back(SymbolTable(table_name));
        cout << "SymbolTableStack push: " << tableStack.size() - 1 << "->" << tableStack.size() << endl;
    }

    void pop()
    {
        SymbolTable table = tableStack[tableStack.size() - 1];
        table.dump();
        tableStack.pop_back();
        cout << "SymbolTableStack push: " << tableStack.size() + 1 << "->" << tableStack.size() << endl;
    }

    bool insert(Symbol *id)
    {
        if (size() == 1)
        {
            id->is_global = true;
        }

        if (!top()->insert(id))
        {
            return false;
        }

        if (id->id_type == ID_VAR)
        {
            if (id->is_global)
            {
                jasm("field static int " + id->name);
            }
            else
            {
                cout << id->name + " = " + to_string(top()->size() - 1) + ", next number " + to_string(size()) << endl;
                if (id->init)
                {
                    store_value(id);
                }
            }
        }
        else if (id->id_type == ID_CONST)
        {
            cout << id->name + " = " + to_string(size() - 1) + ", next number " + to_string(size()) << endl;
            if (id->init)
            {
                store_value(id);
            }
        }
        return true;
    }

    Symbol *lookup(string name)
    {
        for (int i = tableStack.size() - 1; i >= 0; i--)
        {
            Symbol *symbol = tableStack[i].lookup(name);
            if (symbol != nullptr)
            {
                return symbol;
            }
        }
        return nullptr;
    }

    void store_value(Symbol *symbol)
    {
        int index = top()->size() - 1;
        symbol->counter = index;
        switch (symbol->value->value_type)
        {
        case VALUE_INT:
            jasm("sipush " + symbol->value->display());
            jasm("istore " + to_string(index));
            break;
        case VALUE_FLOAT:
            jasm("sipush " + symbol->value->display());
            jasm("istore " + to_string(index));
            break;
            break;
        default:
            break;
        }
    }

    void load_value(Symbol *symbol)
    {
        if (symbol->id_type == ID_VAR)
        {
            switch (symbol->value->value_type)
            {
            case VALUE_INT:
                jasm("iload " + to_string(symbol->counter));
                break;
            case VALUE_FLOAT:
                jasm("iload " + to_string(symbol->counter));
                break;
            default:
                break;
            }
        }
        else if (symbol->id_type == ID_CONST)
        {
            switch (symbol->value->value_type)
            {
            case VALUE_INT:
                jasm("sipush " + symbol->value->display());
                break;
            case VALUE_BOOL:
                jasm("iconst_" + symbol->value->display());
                break;
            case VALUE_FLOAT:
                jasm("sipush " + symbol->value->display());
                break;
            case VALUE_STR:
                jasm("ldc \"" + symbol->value->display() + "\"");
                break;
            default:
                break;
            }
        }
        else
        {
            printf("load value error\n");
        }
    }

    void update_value(Symbol *symbol)
    {
        if (symbol->is_global)
        {
            jasm("putstatic " + ValueTypeToString(symbol->value->value_type) + " " + program_name + "." + symbol->name);
        }
        else
        {
            int index = symbol->counter;
            switch (symbol->value->value_type)
            {
            case VALUE_INT:
                jasm("istore " + to_string(index));
                break;
            case VALUE_FLOAT:
                jasm("istore " + to_string(index));
                break;
                break;
            default:
                break;
            }
        }
    }
};
