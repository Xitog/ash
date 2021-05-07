# Basic Library
import sys

def writeln(arg):
    if isinstance(arg, list):
        res = 0
        for i, a in enumerate(arg):
            res += write(a, i == 0)
        print()
        return res + 1 # for the newline
    else:
        msg = str(arg.val)
        sys.stdout.write(msg + '\n')
        return AshObject(AshInteger, val=len(msg) + 1) # for the newline

def write(arg, first=True):
    if isinstance(arg, list):
        res = 0
        for i, a in enumerate(arg):
            res += write(a, i == 0)
        return res
    else:
        msg = str(arg.val)
        sys.stdout.write(msg)
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
            if type(self.val) != str:
                return f'{self.val} : {self.cls.name}'
            else:
                return f'"{self.val}" : {self.cls.name}'
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

def int_to_s(a):
    return AshObject(AshString, val=str(a.val))

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
AshInteger.instance_methods['to_s'] = int_to_s

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

