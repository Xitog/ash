from ashlang.ashparser import Block, Operation, Terminal

class TranspilerDirectPython:
    def __init__(self):
        pass

    def transpile(self, ast):
        return self.do_elem(ast.root)
    
    def do_elem(self, elem):
        if type(elem) == Block:
            res = ''
            for action in elem.actions:
                ending = '\n' if len(elem.actions) > 1 else ''
                res += self.do_elem(action) + ending
            return res
        elif type(elem) == Operation:
            op = elem.operator.content.val
            a1 = self.do_elem(elem.left)
            a2 = self.do_elem(elem.right)
            if op != 'call(':
                return f"{a1} {op} {a2}"
            else:
                return f"{a1}({a2})"
        elif type(elem) == Terminal:
            if elem.content.typ == Token.Boolean:
                return elem.content.val == "true"
            elif elem.content.typ == Token.String:
                return f"'{elem.content.val}'"
            else:
                return elem.content.val
        else:
            raise Exception(f"Type not known: {type(elem)}")


class TranspilerPython:
    
    def __init__(self):
        pass
    
    def transpile(self, ast, filename):
        s = self.do_elem(ast)
        print(s)
        f = open(filename, mode='w', encoding='utf8')
        f.write(s)
        f.close()
        
    def do_elem(self, elem, affectation=False, Scope=None):
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
                a = self.do_elem(elem.left)
                return self.do_elem(elem.left).__call__(self.do_elem(elem.right))
            # Range create
            elif elem.operator.content.val == '..':
                a = self.do_elem(elem.left)
                b = self.do_elem(elem.right)
                return range(a, b)
            # Call
            elif elem.operator.content.val == '.':
                #if self.debug:
                #    print(elem)
                obj = self.do_elem(elem.left)
                if isinstance(elem.right, Operation) and elem.right.operator.content.val == 'call(':
                    # TODO: PARAMETERS ARE NOT HANDLED
                    msg = elem.right.left.content.val
                else:
                    raise Exception("don't known what to do with" + str(type(elem.right)))
                #b = self.do_elem(elem.right, scope={random})
                if hasattr(obj, msg):
                    fun = getattr(obj, msg)
                    if callable(fun):
                        return fun()
                    else:
                        raise Exception("not callable :" + msg)
                elif b == 'random' and type(a) == range:
                    # TODO: HERE SHOULD BE THE BASE LIBRARY
                    import random
                    return random.sample(a, 1)[0]
                else:
                    raise Exception("not implemented yet")
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
                return f'AshObject(AshInteger, val={elem.content.val})'
            elif elem.content.typ == Token.Number:
                return f'AshObject(AshNumber, val={elem.content.val})'
            elif elem.content.typ == Token.Boolean:
                return f'AshObject(AshBoolean, val={elem.content.val == "true"})'
            elif elem.content.typ == Token.String:
                return elem.content.val
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
            res = ''
            for el in elem.actions:
                res += self.do_elem(el) + '\n'
                print(res)
            return res
        else:
            raise Exception(f"Elem not known {type(elem).__name__}")
