
class Class:

    Scope = {}

    def __init__(self, name : str, *generics):
        self.name = name
        self.generics = generics if generics is not None else []
        self.constraints = {}
        Class.Scope[name] = self

    def __str__(self):
        s = f"{self.name}"
        if self.is_generic():
            s += '<' + ', '.join(self.generics) + '>'
        return s

    def is_generic(self):
        return len(self.generics) > 0

    def constraint(self, gen, types):
        if gen not in self.generics:
            raise Exception(f'Unknown generic: {gen} for class {self.name}')
        self.constraints[gen] = types

Number = Class('num')
Integer = Class('int')
String = Class('str')
Boolean = Class('bool')
List = Class('list', 'value')
Dict = Class('dict', 'key', 'value')
Dict.constraint('key', [Number, Integer, String, Boolean])

def check_in_scope(ash_type):
    return ash_type in Class.Scope

def type_compare(py_type, ash_type):
    if py_type == int:
        return Class.Scope[ash_type] == Integer
    elif py_type == float:
        return Class.Scope[ash_type] == Number
    elif py_type == str:
        return Class.Scope[ash_type] == String
    elif py_type == bool:
        return Class.Scope[ash_type] == Boolean
    elif py_type == list:
        return Class.Scope[ash_type] == List
    elif py_type == dict:
        return Class.Scope[ash_type] == Dict
    else:
        raise Exception(f"Python type not handled: {py_type}")

class Expression:

    def __init__(self, value : object, kind : str):
        self.value = value
        self.type = kind

    def __str__(self):
        return f"|{self.value}:{self.type}|"

class NumberLiteral(Expression):

    def __init__(self, value : float):
        if type(value) not in [int, float]:
            raise Exception(f"Not an number: {value}")
        Expression.__init__(self, value, Number)

class IntegerLiteral(Expression):

    def __init__(self, value):
        if type(value) != int:
            raise Exception(f"Not an integer: {value}")
        Expression.__init__(self, value, Integer)

class StringLiteral(Expression):

    def __init__(self, value):
        if type(value) != str:
            raise Exception(f"Not a string: {value}")
        Expression.__init__(self, value, String)

class BooleanLiteral(Expression):

    def __init__(self, value : bool):
        if type(value) != bool:
            raise Exception(f"Not a boolean:  {value}")
        Expression.__init__(self, value, Boolean)

class ListLiteral(Expression):

    def __init__(self, value_type : str, values = None):
        if not check_in_scope(value_type):
            raise Exception(f"Ash type not handled for values: {value_type}")
        Expression.__init__(self, [], List)
        self.value_type : str = value_type
        if values is not None:
            for v in values:
                self.push(v)

    def push(self, value : Expression):
        if not type_compare(type(value), self.value_type):
            raise Exception(f"Type of {value} is not {self.value_type}")
        self.value.append(value)

    def __str__(self):
        return f"|{self.value}:{self.type.name}<{self.type.generics[0]}={self.value_type}>|"

class DictLiteral(Expression):

    def __init__(self, key_type : str, value_type : str, values = None):
        if not check_in_scope(key_type):
            raise Exception(f"Ash type not handled for keys: {key_type}")
        if not check_in_scope(value_type):
            raise Exception(f"Ash type not handled for values: {value_type}")
        Expression.__init__(self, {}, Dict)
        self.key_type : str = key_type
        self.value_type : str = value_type
        if values is not None:
            for k, v in values.items():
                self.push(k, v)

    def push(self, key, value):
        if not type_compare(type(key), self.key_type):
            raise Exception(f"Wrong Ash type for key: {key} is not of type {self.key_type}")
        if not type_compare(type(value), self.value_type):
            raise Exception(f"Wrong Ash type for value: {value} is not of type {self.value_type}")
        self.value[key] = value

    def __str__(self):
        return f"|{self.value}:{self.type.name}<{self.type.generics[0]}={self.key_type}, {self.type.generics[1]}={self.value_type}>|"


if __name__ == '__main__':
    i25 = IntegerLiteral(25)
    print(i25)
    n2p3 = NumberLiteral(2.3)
    print(n2p3)
    sHello = StringLiteral("Hello")
    print(sHello)
    bTrue = BooleanLiteral(True)
    print(bTrue)
    li123 = ListLiteral('int', [1, 2, 3])
    print(li123)
    li1234 = ListLiteral('int', [1, 2, 3])
    li1234.push(4)
    print(li1234)
    try:
        li1234.push("abc")
    except Exception as e:
        if str(e) == 'Type of abc is not int':
            print("Erreur ok : impossible d'ajouter une str à un list de int")
        else:
            raise e
    dsi = DictLiteral('str', 'int', {'A' : 5, 'Z' : 26})
    print(dsi)

