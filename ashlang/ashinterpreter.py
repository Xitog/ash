from ashlang.ashparser import *

# Basic Library

GLOBAL_DEBUG = False

def writeln(arg):
    if isinstance(arg, list):
        res = 0
        for i, a in enumerate(arg):
            res += write(a, i == 0)
        print()
        return res + 1 # for the newline
    else:
        msg = str(arg)
        prompt = '          ' if GLOBAL_DEBUG else ''
        console.puts(prompt + msg)
        return AshObject(AshInteger, val=len(msg) + 1) # for the newline

def write(arg, first=True):
    if isinstance(arg, list):
        res = 0
        for i, a in enumerate(arg):
            res += write(a, i == 0)
        return res
    else:
        msg = str(arg)
        prompt = '          ' if first and GLOBAL_DEBUG else ''
        console.put(prompt + msg)
        return AshObject(AshInteger, val=len(msg))

def readint(arg=None):
    if isinstance(arg, str) or arg is None:
        if arg is not None:
            res = input(arg)
        else:
            res = input()
        i = int(res)
        return i
    else:
        raise Exception('readint arg should be a string or None and is ' + str(type(arg)))

def readstr(arg=None):
    if isinstance(arg, str) or arg is None:
        if arg is not None:
            res = input(arg)
        else:
            res = input()
        return res
    else:
        raise Exception('readstr arg should be a string or None' + str(type(arg)))
    
# Engine

class AshObject:

    def __init__(self, cls=None, val=None):
        self.val = val
        self.cls = cls
        self.attributes = {}
        if self.cls is not None and hasattr(self.cls, 'instance_methods'):
            self.methods = self.cls.instance_methods

    def send(self, msg, *params):
        res = self.methods[msg](self, *params)
        return res

    def get_method(self, name):
        return self.methods[name]
    
    def __gt__(self, o):
        if self.val is not None and type(o) == AshObject and o.val is not None:
            return self.val > o.val
        else:
            raise Exception("Unable to compare to: " + str(o))
    
    def __repr__(self):
        if self.val is not None:
            return f'{self.val} : {self.cls.name}'
        else:
            return '{AshObject}'


class AshClass(AshObject):

    def __init__(self, name):
        super().__init__(cls=self)
        self.name = name
        self.instance_attributes = {}
        self.instance_methods = {}

def int_flt(op, res, v1, v2):
    if v1.cls not in [AshInteger, AshNumber] or v2.cls not in [AshInteger, AshNumber]:
        raise Exception(f'[ERROR] Interpreter: v1 and v2 must be Integer or Float, not {v1.cls} and {v2.cls}')
    if op not in ['/', '//']:
        if v1.cls == v2.cls:
            return v1.cls
        else:
            return AshNumber
    elif op == '/':
        return AshNumber
    elif op == '//':
        return AshInteger
    else:
        raise Exception(f'[ERROR] Interpreter: Operator {op} not known')

#-------------------------------------------------------------------------------
# Integer
#-------------------------------------------------------------------------------

AshInteger = AshClass('Integer')

def ari_add(a1, a2):
    val = a1.val + a2.val
    return AshObject(int_flt('+', val, a1, a2), val=val)
def ari_sub(a1, a2):
    val=a1.val - a2.val
    return AshObject(int_flt('-', val, a1, a2), val=val)
def ari_mul(a1, a2):
    val=a1.val * a2.val
    return AshObject(int_flt('*', val, a1, a2), val=val)
def ari_div(a1, a2):
    val=a1.val / a2.val
    return AshObject(int_flt('/', val, a1, a2), val=val)
def ari_divint(a1, a2):
    val=a1.val // a2.val
    return AshObject(int_flt('//', val, a1, a2), val=val)
def ari_mod(a1, a2):
    val=a1.val % a2.val
    return AshObject(int_flt('%', val, a1, a2), val=val)
def ari_pow(a1, a2):
    val=a1.val ** a2.val
    return AshObject(int_flt('**', val, a1, a2), val=val)
def ari_lshift(a1, a2):
    val=a1.val << a2.val
    return AshObject(int_flt('<<', val, a1, a2), val=val)
def ari_rshift(a1, a2):
    val=a1.val >> a2.val
    return AshObject(int_flt('>>', val, a1, a2), val=val)
def ari_unary_minus(a1):
    return AshObject(a1.cls, val=-a1.val)

def ari_cmp_eq(a1, a2):
    return AshObject(AshBoolean, val=a1.val == a2.val)
def ari_cmp_le(a1, a2):
    return AshObject(AshBoolean, val=a1.val <= a2.val)
def ari_cmp_lt(a1, a2):
    return AshObject(AshBoolean, val=a1.val < a2.val)
def ari_cmp_ge(a1, a2):
    return AshObject(AshBoolean, val=a1.val >= a2.val)
def ari_cmp_gt(a1, a2):
    return AshObject(AshBoolean, val=a1.val > a2.val)
def ari_cmp_dif(a1, a2):
    return AshObject(AshBoolean, val=a1.val != a2.val)

AshInteger.instance_methods['+'] = ari_add
AshInteger.instance_methods['-'] = ari_sub
AshInteger.instance_methods['*'] = ari_mul
AshInteger.instance_methods['/'] = ari_div
AshInteger.instance_methods['//'] = ari_divint
AshInteger.instance_methods['%'] = ari_mod
AshInteger.instance_methods['**'] = ari_pow
AshInteger.instance_methods['<<'] = ari_lshift
AshInteger.instance_methods['>>'] = ari_rshift
AshInteger.instance_methods['=='] = ari_cmp_eq
AshInteger.instance_methods['<'] = ari_cmp_lt
AshInteger.instance_methods['<='] = ari_cmp_le
AshInteger.instance_methods['>='] = ari_cmp_ge
AshInteger.instance_methods['>'] = ari_cmp_gt
AshInteger.instance_methods['!='] = ari_cmp_dif
AshInteger.instance_methods['unary-'] = ari_unary_minus

#-------------------------------------------------------------------------------
# Boolean
#-------------------------------------------------------------------------------

AshBoolean = AshClass('Boolean')

def boo_and(b1, b2):
    return AshObject(AshBoolean, val=b1.val and b2.val)
def boo_or(b1, b2):
    return AshObject(AshBoolean, val=b1.val or b2.val)
def boo_not(b):
    return AshObject(AshBoolean, val=not b.val)

AshBoolean.instance_methods['and'] = boo_and
AshBoolean.instance_methods['or'] = boo_or
AshBoolean.instance_methods['not'] = boo_not

#-------------------------------------------------------------------------------
# Float
#-------------------------------------------------------------------------------

AshNumber = AshClass('Number')

AshNumber.instance_methods['+'] = ari_add
AshNumber.instance_methods['-'] = ari_sub
AshNumber.instance_methods['*'] = ari_mul
AshNumber.instance_methods['/'] = ari_div
AshNumber.instance_methods['//'] = ari_divint
AshNumber.instance_methods['%'] = ari_mod
AshNumber.instance_methods['**'] = ari_pow
AshNumber.instance_methods['<<'] = ari_lshift
AshNumber.instance_methods['>>'] = ari_rshift
AshNumber.instance_methods['=='] = ari_cmp_eq
AshNumber.instance_methods['<'] = ari_cmp_lt
AshNumber.instance_methods['<='] = ari_cmp_le
AshNumber.instance_methods['>='] = ari_cmp_ge
AshNumber.instance_methods['>'] = ari_cmp_gt
AshNumber.instance_methods['!='] = ari_cmp_dif

#-------------------------------------------------------------------------------
# String
#-------------------------------------------------------------------------------

AshString = AshClass('String')

def str_add(s1, s2):
    return AshObject(AshString, val=s1.val + s2.val)
def str_mul(s1, a1):
    return AshObject(AshString, val=s1.val * a1.val)
def str_sub(s1, s2):
    return AshObject(AshString, val=s1.val.replace(s2.val, ""))

AshString.instance_methods['+'] = str_add
AshString.instance_methods['*'] = str_mul
AshString.instance_methods['-'] = str_sub

#-------------------------------------------------------------------------------

AshModule = AshClass('Module')

#-------------------------------------------------------------------------------

AshModuleSDL = AshObject(cls=AshModule)

global tk_root
global tk_screen

def SDL_init(self):
    from tkinter import Tk, Label, Canvas
    global tk_root, tk_screen
    tk_root = Tk()
    tk_root.title('Init')
    tk_root.geometry("645x485")
    #w = Label(root, text="Hello")
    tk_screen = Canvas(tk_root, width=640, height=480, background='#000000')
    tk_screen.create_text(30, 30, text="Hello", fill='#FF0000')
    tk_screen.pack()
    tk_root.wm_attributes("-topmost", 1)
    #root.focus_force()

def SDL_run(self):
    global tk_root
    tk_root.mainloop()

def SDL_text(self, args):
    global tk_screen
    x = args[0]
    y = args[1]
    t = args[2]
    tk_screen.create_text(x, y, text=t, fill='#FF0000')

AshModuleSDL.methods['init'] = SDL_init
AshModuleSDL.methods['run'] = SDL_run
AshModuleSDL.methods['text'] = SDL_text

console = None

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
                a = self.do_elem(elem.left)
                return self.do_elem(elem.left).__call__(self.do_elem(elem.right))
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
        return last #18h59 8/9
