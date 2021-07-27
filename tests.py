#------------------------------------------------------------------------------
# Imports
#------------------------------------------------------------------------------

from ashlang import Lexer, LANGUAGES, Parser, Interpreter, Console, TranspilerDirectPython
from ashlang import AshObject, AshInteger, AshNumber, AshBoolean, AshRange
from weyland import Token

import traceback

#------------------------------------------------------------------------------
# Fonctions
#------------------------------------------------------------------------------

def Integer(v):
    return AshObject(AshInteger, val=v)

def Number(v):
    return AshObject(AshNumber, val=v)

def Boolean(v):
    return AshObject(AshBoolean, val=v)

def Range(v):
    return AshObject(AshRange, val=v)

#------------------------------------------------------------------------------
# Classes
#------------------------------------------------------------------------------

class Test:

    mode = 'full'
    debug = False
    good = 0
    skipped = 0
    failed = 0
    all = []

    KO = 0
    OK = 1
    SKIP = 2

    @classmethod
    def run(cls):
        cls.console = Console()
        cls.lexer = Lexer(LANGUAGES['ash'], discards=['blank'], debug=cls.debug)
        cls.parser = Parser()
        cls.interpreter = Interpreter(cls.console)
        cls.transpiler = TranspilerDirectPython()
        cls.console.info("--------------------------------------------------------------------------------")
        cls.console.info(f"Running all tests ({len(cls.all)})")
        cls.console.info("--------------------------------------------------------------------------------")
        for i, t in enumerate(cls.all):
            try:
                r = t.execute(i+1)
                if r == cls.OK:
                    cls.good += 1
                elif r == cls.KO:
                    cls.failed += 1
                elif r == cls.SKIP:
                    cls.skipped += 1
            except Exception as e:
                cls.failed += 1
                cls.console.error(f"{i+1:03d} Test   : {t.code:45} error: {e}")
                traceback.print_exc()
        cls.console.info("--------------------------------------------------------------------------------")
        cls.console.info(f"All tests ({len(cls.all)}) executed:")
        cls.console.info(f"    {cls.good:03d} passed  / {len(cls.all)}")
        cls.console.info(f"    {cls.skipped:03d} skipped / {len(cls.all)}")
        cls.console.info(f"    {cls.failed:03d} failed  / {len(cls.all)}")
        cls.console.info("--------------------------------------------------------------------------------")

    def __init__(self, code, tokens=None, result=None):
        Test.all.append(self)
        self.code = code
        self.tokens = tokens # expected tokens
        self.result = result # expected result

    def execute(self, nb=0):
        tokens = Test.lexer.lex(self.code)
        if self.tokens is not None:
            max_len = max(len(tokens), len(self.tokens))
            error = len(tokens) == len(self.tokens)
            l1 = "expected:"
            l2 = "got:     "
            for i in range(0, max_len):
                if i < len(self.tokens):
                    t1 = self.tokens[i]
                    l1 += f"{str(t1):5}"
                else:
                    t1 = None
                    l1 += ' ' * 5
                if i < len(tokens):
                    t2 = tokens[i]
                    l2 += f"{str(t2):5}"
                else:
                    t2 = None
                    l2 += ' ' * 5
                if t1 is None or t2 is None or t1.typ != t2.typ or t1.val != t2.val: # replace by != when new version of weyland is out
                    error = True
                    break
            if error:
                Test.console.error(f"{nb:03d} Test   : {self.code:45} Error at lexing stage. Aborting.")
                Test.console.error(' ' * 13 + l1)
                Test.console.error(' ' * 13 + l2)
                return Test.KO
        if self.result is not None:
            ast = Test.parser.parse(tokens)
            result = Test.interpreter.do_ast(ast)
            if result != self.result:
                Test.console.error(f"{nb:03d} Error at parsing or interpreting: expected {self.result} got {result}. Aborting.")
                return Test.KO
            Test.console.info(f"{nb:03d} Test OK: {self.code:45} = {result}")
        if self.tokens is None and self.result is None:
            Test.console.info(f"{nb:03d} Test   : {self.code:45} skipped")
            return Test.SKIP
        return Test.OK

#------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------

def main():
    # Littéraux
    Test('4', result=Integer(4))
    Test('2.0', result=Number(2.0))
    Test('true', result=Boolean(True))
    # Littéraux range et problème du point
    Test('2..3', tokens=[Token('integer', '2', 0)])
    Test('2.3..4')
    # Les espaces sont neutres
    Test('2+3', result=Integer(5))
    Test('2 +3', result=Integer(5))
    Test('2+ 3', result=Integer(5))
    Test('2 + 3', result=Integer(5))
    # Les parenthèses
    Test('2 + 3 * 2', result=Integer(8))
    Test('(2 + 3) * 2', result=Integer(10))
    # Affectation
    Test('a = 5')
    # Code
    Test('if a == 5 then writeln("a equals 5") end')
    # Exécution
    Test.run()

if __name__ == '__main__':
    main()
