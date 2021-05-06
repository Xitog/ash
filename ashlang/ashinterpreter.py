from ashlang.ashparser import *
from ashlang.ashlib import *

class Interpreter:
    
    def __init__(self, io, debug=False):
        global console
        console = io
        self.vars = {}
        self.vars['writeln'] = writeln
        self.vars['write'] = write
        self.vars['readint'] = readint
        self.vars['readstr'] = readstr
        self.vars['sdl'] = AshModuleSDL
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
                self.vars[ids] = val
                if self.debug: print('[EXEC] =', val, 'to', ids)
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
                return method.__call__(caller)
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
        elif type(elem) == VarDeclaration:
            identifier = self.do_elem(elem.right, affectation=True)
            value = self.do_elem(elem.left)
            self.vars[identifier] = value
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
                    if elem.content.val not in self.vars:
                        raise Exception(f"[ERROR] Interpreter: Identifier not know: {elem.content.val}")
                    return self.vars[elem.content.val]
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
