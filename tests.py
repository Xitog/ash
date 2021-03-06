from ashlang import main, Tokenizer, Token

import token as pytoken
import keyword
import tokenize
from io import BytesIO

t = Tokenizer()

gen = tokenize.tokenize(BytesIO("2+3 1.3 if ()".encode('utf-8')).readline)
prow = -1
for a in gen: # type, exact_type, start, end, line, string
    srow, scol = a.start
    if prow == -1 or prow != srow:
        prow = srow
        print(f'Row: {prow}')
    erow, ecol = a.end
    typ = pytoken.tok_name[a.type]
    etyp = pytoken.tok_name[a.exact_type]
    if typ == 'NAME' and keyword.iskeyword(a.string):
        typ = 'KEYWORD'
    elif typ == 'NUMBER':
        if float(a.string).is_integer():
            typ = 'INTEGER'
        else:
            typ = 'FLOAT'
    elif etyp in ['LPAR', 'RPAR', 'LSQB', 'RSQB', 'COLON', 'COMMA', 'SEMI']:
        typ = 'SEPARATOR'
    elif typ == 'OP':
        typ = 'OPERATOR'
    # type and line not used
    print(f'{scol:02d}-{ecol:02d} {typ:10} {a.string}')


def python_tokenize(string):
    gen = tokenize.tokenize(BytesIO(string.encode('utf-8')).readline)
    tokens = []
    for token in gen:
        typ = pytoken.tok_name[token.type]
        if typ == 'ENCODING':
            continue
        elif typ == 'NAME':
            if keyword.iskeyword(token.string):
                typ = Token.Keyword
            else:
                typ = Token.Identifier
        elif typ == 'NUMBER':
            if float(token.string).is_integer():
                typ = Token.Integer
            else:
                typ = Token.Float
        elif typ == 'OP':
            etyp = pytoken.tok_name[token.exact_type]
            if etyp in ['LPAR', 'RPAR', 'LSQB', 'RSQB', 'COLON', 'COMMA', 'SEMI']:
                typ = Token.Separator
            else:
                typ = Token.Operator
        elif typ == 'NEWLINE':
            typ = Token.NewLine
        elif typ == 'ENDMARKER':
            continue
        else:
            raise Exception(f"Python token type not handled yet: {typ}")
        tokens.append(Token(typ, token.string, token.start[0]))
    return tokens[:-1] # remove last NEWLINE


class Test:

    nb = 0
    nb_good = 0

    def __init__(self, s, expected=None):
        Test.nb += 1
        process = t.tokenize(s, to_s = True)
        vs_python = False
        if expected is None:
            expected = t.format(python_tokenize(s))
            vs_python = True
        if process == expected:
            info = ' ' * 11 if not vs_python else '(vs python)'
            print(f'Test {Test.nb} ok     {s:20} {info} {process}')
            Test.nb_good += 1
        else:
            print(f'Test {Test.nb} failed {s:}:')
            print(f'1. Processed: {process}')
            print(f'2. Expected:  {expected}')
            if vs_python:
                print('Expected obtained from Python tokenizer.')

Test('2+3', '(TokenList, [(Integer, "2"), (Operator, "+"), (Integer, "3")])')
Test('2+3') # 14h26 plus besoin de spécifier le résultat, on utilise le tokenizer de base de Python :-) !
Test('2..3', '(TokenList, [(Integer, "2"), (Operator, ".."), (Integer, "3")])')
Test('2.3..4', '(TokenList, [(Float, "2.3"), (Operator, ".."), (Integer, "4")])')
Test('2 + 3 * 2')
Test('(2 + 3) * 2')
Test('a = 5')
Test('if a == 5 then writeln("a equals 5") end', '(TokenList, [(Keyword, "if"), (Identifier, "a"), (Operator, "=="), (Integer, "5"), (Keyword, "then"), (Identifier, "writeln"), (Separator, "("), (String, "a equals 5"), (Separator, ")"), (Keyword, "end")])')
print(f"Test passed: {Test.nb_good} / {Test.nb}")

