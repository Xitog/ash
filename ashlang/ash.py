"""
    Ash
    ---
    A simple lexer / parser / interpreter / transpiler for a small language
    inspired by Lua/Ruby. The transpiler targets Python.

    Tokenizer / Symbolizer / Lexer   string   -> [tokens]
    Parser                           [tokens] -> abstract syntax tree (AST)
    Interpreter                      AST      -> result
    Compiler                         AST      -> low level code
    Transpiler                       AST      -> high level code
"""

#
# Imports
#

import os.path
import sys # for writing debug info
import traceback

import weyland
from weyland import Lexer, LANGUAGES
from ashlang.ashparser import Parser
from ashlang.ashtranspiler import TranspilerDirectPython
from ashlang.ashinterpreter import Interpreter, Error

LANGUAGES['ash'].tokens['keyword'].append('const')

#
# Globals
#
__version__ = '0.0.2'
__py_version__ = f'{sys.version_info[0]}.{sys.version_info[1]}.{sys.version_info[2]}'

#
# Console
#

class FallBack:
    def write(self, msg, color):
        sys.stdout.write(msg)


class Console:

    def __init__(self):
        try:
            # Works only in IDLE
            self.out = sys.stdout.shell
        except AttributeError:
            self.out = FallBack()
        self.outputs = []
    
    def error(self, msg):
        self.out.write('[ERROR] ' + str(msg) + '\n', 'COMMENT')

    def info(self, msg):
        self.out.write('[INFO]  ' + str(msg) + '\n', 'DEFINITION')
    
    def put(self, msg):
        self.outputs.append(msg)
        self.out.write(str(msg), 'TODO')

    def puts(self, msg):
        self.outputs.append(msg)
        self.out.write(str(msg) + '\n', 'TODO')




#
# Main
#

def read(filepath):
    f = open(filepath, mode='r', encoding='utf8')
    c = f.read()
    f.close()
    return c

class REPL:

    def __init__(self, debug=False):
        self.console = Console()
        self.lexer = Lexer(LANGUAGES['ash'], discards=['blank'], debug=debug)
        self.parser = Parser()
        self.interpreter = Interpreter(self.console)
        self.transpiler = TranspilerDirectPython()
        self.mode = 'full'
        self.debug = debug
        self.output = None

    def exec(self, command):
        if command == 'exit':
            return False
        elif command == 'help':
            print(f'Ash {__version__} on Python {__py_version__}. 2017-2020')
            print('help           : this help')
            print('tests          : run multiple tests')
            print('reset          : reset interpreter')
            print('debug          : set/unset debug. You can specify true or false')
            print('dump           : get variables of root scope')
            print('exec <f>       : execute a file')
            print('trans <f>      : transpile a file')
            print('mode           : print current mode')
            print('mode full      : tokenize, parse and interpret')
            print('mode tokenize  : mode only produce tokens (no parse, no execution)')
            print('mode parse     : mode only produce an ast tree (no execution)')
            print('mode transpile : mode only transpile to python')
            print('exit           : exit this shell')
        elif command.startswith('debug'):
            args = command.split(' ')
            if len(args) < 2:
                self.debug = not self.debug
            else:
                debug = True if args[1] == 'true' else False
            self.console.info(f'Debug set to {debug}.')
        elif command == 'dump':
            for k in sorted(self.interpreter.vars.get_names()):
                ref = self.interpreter.vars.get_ref(k)
                cst = 'const' if ref.const else 'var'
                typ = 'any' if ref.typ is None else ref.typ
                print(f"{k:10}{typ:15}{cst:5}", ref.val)
        elif command.startswith('transXXX'):
            args = command.split(' ')
            args = command.split(' ')
            if len(args) < 2:
                self.console.error('You must indicate a file to process.')
            else:
                arg = args[1]
                if not arg.endswith('.ash'):
                    arg += '.ash'
                if not os.path.isfile(arg):
                    self.console.error('File ' + arg + ' does not exist')
                else:
                    c = read(arg)
                    tokenizer = Tokenizer()
                    parser = Parser()
                    self.console.outputs = []
                    tokens = tokenizer.tokenize(c)
                    ast = parser.parse(tokens)
                    TranspilerPython().transpile(ast.root, 'last.py') 
        elif command.startswith('exec') or command == 'tests':
            if command == 'tests': # hack
                command = 'exec tests'
            args = command.split(' ')
            if len(args) < 2:
                self.console.error('You must indicate a file to process.')
            else:
                arg = args[1]
                if os.path.isdir(arg):
                    files = os.listdir(arg)
                    cpt = 0
                    max_file = 0
                    for f in files:
                        if f.endswith('.ash'):
                            max_file += 1
                    for f in files:
                        if f.endswith('.ash'):
                            cpt += 1
                            self.console.info(f'Executing {f} ({cpt}/{max_file})')
                            filename = os.path.join(arg, f)
                            html_output = os.path.join(arg, 'html', f)
                            c = read(filename)
                            self.run_ash(c, html_output[:-4] + '.html')
                else:
                    if not arg.endswith('.ash'):
                        arg += '.ash'
                    if not os.path.isfile(arg):
                        self.console.error('File ' + arg + ' does not exist')
                    else:
                        c = read(arg)
                        self.run_ash(c)
        elif command == 'reset':
            self.interpreter = Interpreter(self.console)
        elif command == 'tests':
            pass #read_tests('./tests/tests.txt')
        elif command.startswith('mode'):
            args = command.split(' ')
            if len(args) == 1:
                print(f'[INFO] Mode is {self.mode}')
            elif args[1] in ['full', 'tokenize', 'lex', 'parse', 'transpile']:
                self.mode = args[1]
                print(f'[INFO] Mode set to {self.mode}')
            else:
                self.console.error('Mode must be chosen between full, tokenize, lex, parse and transpile.')
        elif not command: # empty string are false
            pass
        else:
            try:
                tokens, ast, result = self.run_ash(command)
                if self.mode in [Mode.TOKENIZE, Mode.LEX, Mode.PARSE]:
                    print(f'[INFO] Token positions: {command}')
                    start_line = ''
                    for num, tok in enumerate(tokens):
                        while len(start_line) < tok.first:
                            start_line += ' '
                        si = str(num)
                        start_line += si + '_' * (tok.length - len(si))
                    print(f'[INFO]                  {start_line}')
                    print('[INFO] List of tokens:')
                    for num, tok in enumerate(tokens):
                        print(f'    {num:3d} {tok}')
                    if self.mode == Mode.PARSE:
                        print('[INFO] Abstract syntax tree:')
                        print(ast)
                if self.mode == Mode.TRANSPILE:
                    print(result)
                elif self.mode == Mode.FULL:
                    self.console.puts('= ' + str(result))
            except Error as e:
                self.console.error(f'{e}')
            except Exception as e:
                self.console.error(f'Exception: {e}')
                traceback.print_exception(*sys.exc_info())
        return True

    # Can rise two types of exceptions:
    # - Exception: standard Python error
    # - Error: Ash error
    def run_ash(self, command, file_output=None):
        self.console.outputs = []
        tokens = []
        ast = None
        res = None
    
        tokens = self.lexer.lex(command)
        if self.mode in [Mode.TOKENIZE, Mode.LEX]:
            return (tokens, None, None)
        ast = self.parser.parse(tokens)
        if self.mode == Mode.PARSE:
            return (tokens, ast, None)
        if self.mode == Mode.TRANSPILE:
            trans = self.transpiler.transpile(ast)
            return (tokens, ast, trans)
        res = self.interpreter.do_ast(ast)

        if self.debug:
            filename = file_output if file_output is not None else 'last.html'
            f = open(filename, mode='w', encoding='utf8')
            f.write('<html>\n  <body>\n')
            f.write('    <h2>Command</h2>\n      <pre>\n')
            f.write(command)
            f.write('    </pre>\n')
            f.write('    <h2>Tokens</h2>\n      <table border="1">\n')
            for i, t in enumerate(tokens):
                f.write(f'      <tr><td>{i}</td><td>{t.typ}</td><td>{t.val}</td></tr>\n')
            f.write('    </table>\n')
            f.write('    <h2>Abstract syntax tree</h2>\n')
            if ast is not None:
                f.write(ast.to_html())
            else:
                f.write('    <p>No AST defined.</p>\n')
            f.write('    <h2>Outputs</h2>\n      <table border="1">\n')
            for t in self.console.outputs:
                f.write('      <tr><td>' + str(t) + '</td></tr>\n')
            f.write('    </table>\n')
            f.write('    <h2>Result</h2>\n')
            f.write('    <h1>' + str(res) + '</h1>\n')
            f.write('  </body>\n</html>')
            f.close()
        return (tokens, ast, res)


class Mode:
    FULL = 'full'
    TRANS = 'trans'
    PARSE = 'parse'
    LEX = 'lex'
    TOKENIZE = 'tokenize'
    TRANSPILE = 'transpile'


#------------------------------------------------------------------------------
# Main function
#------------------------------------------------------------------------------

def main():
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
        f = open(filepath, mode='r')
        data = f.read()
        f.close()
        Interpreter(Console()).do(data)
    else:
        repl = REPL()
        print(f'Ash {__version__} on Python {__py_version__} using Weyland {weyland.__version__}')
        print('Type "help" for more information.')
        res = True
        while res:
            command = input('ash> ')
            res = repl.exec(command)

if __name__ == '__main__':
    main()
