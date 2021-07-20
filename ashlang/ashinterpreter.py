from ashlang.ashparser import *
from ashlang.ashlib import *


class Error(Exception):

    def __init__(self, message):
        Exception.__init__(self, message)


class Reference:

    def __init__(self, name, val, hint=None, const=False):
        self.name = name
        self.typ = CLASSES[hint] # if None = any type
        self.const = const
        self.val = val
    
    def change_val(self, val):
        if self.const:
            raise Error('Illegal change of a constant reference.')
        elif self.typ is not None and self.typ != val.cls:
            raise Error(f'Illegal type {val.cls} instead of {self.typ}.')
        else:
            self.val = val


class Scope:

    def __init__(self, over=None):
        self.vars = {}
        self.over = over

    def is_root(self):
        return self.over is None

    def get_path(self):
        if self.over is None:
            return self.name
        else:
            return self.over.get_path() + '.' + self.name

    def get_names(self):
        return sorted(self.vars.keys())

    def __len__(self):
        return len(self.vars)

    def __contains__(self, key):
        return key in self.vars

    def set(self, name, val, hint=None, const=False):
        # affectation
        if name in self.vars:
            if hint is not None:
                raise Error('Type can be assigned only once at declaration.')
            self.vars[name].change_val(val)
        # declaration
        else:
            self.vars[name] = Reference(name, val, hint, const)

    def get_val(self, name):
        return self.get_ref(name).val

    def get_ref(self, name):
        if name not in self.vars:
            raise Error(f"Identifier not known: '{name}'")
        return self.vars[name]


class Interpreter:
    
    def __init__(self, io, debug=False):
        global console
        console = io
        self.vars = Scope()
        self.vars.set('writeln', writeln, 'NativeFunction', True)
        self.vars.set('write', write, 'NativeFunction', True)
        self.vars.set('readint', readint, 'NativeFunction', True)
        self.vars.set('readstr', readstr, 'NativeFunction', True)
        self.vars.set('sdl', AshModuleSDL, 'NativeModule', const=True)
        self.debug = debug

    #def set_debug(self):
    #    self.debug = not self.debug
    
    def do_elem(self, elem, affectation=False, Scope=None):
        if self.debug:
            if type(elem) == Terminal:
                print(type(elem), '::', elem)
            else:
                print(type(elem), '::\n', elem)
        if type(elem) == Operation:
            op = elem.operator.content.val
            # Arithmetic
            if op in ['+', '-', '*', '/', '//', '%', '**', '<<', '>>']:
                if elem.left is None:
                    return self.do_elem(elem.right).send('unary-')
                else:
                    return self.do_elem(elem.left).send(op, self.do_elem(elem.right))
            # Comparison
            elif op in ['==', '<=', '<', '>=', '>', '!=']:
                return self.do_elem(elem.left).send(op, self.do_elem(elem.right))
            elif op in ['+=', '-=', '*=', '/=', '//=', '%=', '**=']:
                ids = self.do_elem(elem.left, affectation=True)
                val = self.do_elem(elem.right)
                true_op = op[:-1]
                self.vars[ids] = self.vars[ids].send(true_op, val)
                return self.vars[ids]
            # Affectation
            elif elem.operator.content.val == '=':
                val = self.do_elem(elem.right)
                ids = self.do_elem(elem.left, affectation=True)
                self.vars.set(ids, val)
                return self.vars[ids]
            # Boolean
            elif op in ['and', 'or']:
                return self.do_elem(elem.left).send(op, self.do_elem(elem.right))
            # Not
            elif op == 'not':
                return self.do_elem(elem.right).send(op)
            # Function Call
            elif elem.operator.content.val == 'call(':
                method = self.do_elem(elem.left)
                caller = self.do_elem(elem.left.left)
                if elem.right is not None:
                    parameters = self.do_elem(elem.right)
                else:
                    parameters = None
                if caller is not None and parameters is not None:
                    return method.__call__(caller, parameters)
                elif caller is not None:
                    return method.__call__(caller)
                elif parameters is not None:
                    return method.__call__(parameters)
                else:
                    return method.__call__()
            # Range create
            elif elem.operator.content.val == '..':
                a = self.do_elem(elem.left)
                b = self.do_elem(elem.right)
                return range(a, b)
            # Call
            elif elem.operator.content.val == '.':
                obj = self.do_elem(elem.left)
                msg = elem.right.content.val
                return obj.get_method(msg)
                # NOT USED BELOW
                #if isinstance(elem.right, Operation) and elem.right.operator.content.val == 'call(':
                #    # TODO: PARAMETERS ARE NOT HANDLED
                #    msg = elem.right.left.content.val
                #else:
                #    raise Exception("don't known what to do with <" + type(elem.right).__name__ + '>' + str(elem.right))
                ##b = self.do_elem(elem.right, scope={random})
                #if hasattr(obj, msg):
                #    fun = getattr(obj, msg)
                #    if callable(fun):
                #        return fun()
                #    else:
                #        raise Exception("not callable :" + msg)
                #elif b == 'random' and type(a) == range: # TODO: do not work
                #    # TODO: HERE SHOULD BE THE BASE LIBRARY
                #    import random
                #    return random.sample(a, 1)[0]
                #else:
                #    raise Exception("not implemented yet")
            # Concat expression
            elif elem.operator.content.val == ',':
                args = []
                args.append(self.do_elem(elem.left))
                right = self.do_elem(elem.right)
                if isinstance(right, list):
                    args.extend(right)
                else:
                    args.extend([right])
                return args
            else:
                raise Exception("Operator not known: " + elem.operator.content.val)
        elif type(elem) == ListTypedVar:
            typed_ids = []
            for e in elem.content:
                idv = e.content.content.val
                typ = e.right.content.val if e.right is not None else None
                typed_ids.append((idv, typ))
            return typed_ids
        elif type(elem) == VarDeclaration:
            typed_ids = self.do_elem(elem.right, affectation=True)
            value = self.do_elem(elem.left)
            for identifier in typed_ids:
                self.vars.set(identifier[0], value, hint=identifier[1], const=elem.const_ref)
            return value
        elif type(elem) == Statement:
            cond = self.do_elem(elem.cond)
            executed = 0
            result = False
            if self.debug: print('[EXEC] Statement cond=', cond, 'executed=', executed)
            while cond.val and (executed == 0 or elem.loop):
                result = self.do_elem(elem.action)
                executed += 1
                if elem.loop:
                    cond = self.do_elem(elem.cond)
                if self.debug: print('[EXEC] Looping cond=', cond, 'executed=', executed, 'loop=', elem.loop)
            if executed == 0 and elem.alter is not None:
                return self.do_elem(elem.alter)
            else:
                return result
        elif type(elem) == Terminal:
            if elem.content.typ == Token.Integer:
                return AshObject(AshInteger, val=int(elem.content.val))
            elif elem.content.typ == Token.Number:
                return AshObject(AshNumber, val=float(elem.content.val))
            elif elem.content.typ == Token.Boolean:
                return AshObject(AshBoolean, val=elem.content.val == "true")
            elif elem.content.typ == Token.String:
                return AshObject(AshString, val=elem.content.val[1:-1])
            elif elem.content.typ == Token.Identifier:
                if affectation == True:
                    return elem.content.val
                else:
                    return self.vars.get_val(elem.content.val)
            else:
                raise Exception(f"Terminal not known:\nelem.content.typ = {elem.content.typ} and type(elem) = {type(elem)}")
        #elif type(elem) == FunCall:
        #    if elem.name.content.val == 'writeln':
        #        arg = self.do_elem(elem.arg)
        #        print(arg)
        #        return len(str(arg))
        #    else:
        #        raise Exception("Function not known: " + str(elem.name))
        elif elem is None:
            return None
        elif type(elem) == Block:
            res = None
            for el in elem.actions:
                res = self.do_elem(el)
            return res
        else:
            raise Exception(f"Elem not known {elem}")

    def do(self, data):
        parser = Parser()
        res = Tokenizer().tokenize(data)
        ast = parser.parse(res)
        res = self.do_ast(ast)
    
    def do_ast(self, ast):
        last = None
        for elem in ast.root.actions:
            last = self.do_elem(elem)
        return last
