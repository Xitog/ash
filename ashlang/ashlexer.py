"""
    This lexer for the Ash language produces a list tokens from a sequence of characters
    str -> [Token]
"""

class TokenType:
    """Class for the different types of token"""

    def __init__(self, name: str):
        self.name = name

    def __str__(self):
        return self.name

    def __eq__(self, obj):
        if not isinstance(obj, TokenType):
            msg = "Can only compare instances of TokenType and not instances of"
            raise Exception(f"{msg} {type(obj)}")
        return self.name == obj.name


class Token:
    """
        +typ:enum the type of the token
        +val:string the actual string of the token
    """

    Integer = TokenType('Integer')
    Float = TokenType('Float')
    Identifier = TokenType('Identifier')
    Operator = TokenType('Operator')
    Separator = TokenType('Separator')
    Keyword = TokenType('Keyword')
    NewLine = TokenType('New Line')
    Boolean = TokenType('Boolean')
    String = TokenType('String')
    Comment = TokenType('Comment')
    Discard = TokenType('Discard') # Unused
    Error = TokenType('Error')   # Unused
    EndOfSource = TokenType('End')     # Unused

    def __init__(self, typ: TokenType, val: str, start=None):
        self.typ = typ
        self.val = val
        self.start = start
        self.length = len(val)
        self.lvl = 0

    def __str__(self):
        if self.start is None or self.length is None:
            return f":{self.typ}: [{self.val}]"
        return f":{self.typ}: [{self.val}] ({self.start} +{self.length})"

    def __repr__(self):
        return str(self)

    def format(self):
        return f'({self.typ}, "{self.val}")'

    def to_s(self, level=1):
        """String representation of the token type"""
        return "    " * level + "{Token} " + str(self)


class Tokenizer:
    """The engine to tokenize a string"""

    NEWLINE = "New line"

    START_OF_ID = ['@', '_', '$']
    START_OF_STRING = ['"', "'"]
    END_OF_STRING = ['?', '!']

    SEPARATORS = [
        '(', ')', '[', ']', '{', '}',
        '\n',
    ]

    OPERATORS = [
        '+', '-', '*', '/', '%', '**', '//',
        '=', '+=', '-=', '*=', '/=', '%=', '**=', '//=',
        '==', '!=', '>', '>=', '<=', '<', '<=>',
        'and', 'or', 'not', 'xor',
        'not', 'in',
        '.', '..', '..<',
        ':', '|', '->',
        '<<', '>>',
        ',', ';' # concat : , for expression, ; for statement
    ]

    KEYWORDS = [
        'if', 'then', 'else', 'elif', 'end', # 'unless'
        'while', 'repeat', 'until', 'do',
        'for', 'in',
        'break', 'next',
        'new',
        'fun', 'sub', 'get', 'set',
        'return',
        'class'
    ]

    BOOLEANS = [
        'true',
        'false'
    ]

    def __init__(self, debug: bool = False):
        self.tokens = []
        self.line_count = 0
        Tokenizer.START_OF_OPERATOR = []
        for operator in Tokenizer.OPERATORS:
            if not operator[0].isalpha() and operator[0] not in Tokenizer.START_OF_OPERATOR:
                Tokenizer.START_OF_OPERATOR.append(operator[0])
        self.debug = debug

    def format(self, token_list):
        s = '(TokenList, ['
        for i, tok in enumerate(token_list):
            s += tok.format()
            if i != len(token_list) - 1:
                s += ', '
        s += '])'
        return s

    def read_number(self, line: str, start: int):
        """Read a number from a string"""
        index = start
        if self.debug:
            msg = f'read_number at {index}, line {self.line_count}, starts with [{line[index]}]'
            print("    " + msg)
        word = line[index]
        suspended = False
        index += 1
        is_float = False
        while index < len(line) and \
          (line[index].isdigit() or line[index] == '_' or line[index] == '.'):
            if line[index] == '_' and suspended:
                raise Exception("Twice _ following in number at line " + str(self.line_count))
            if line[index] == '_':
                suspended = True
            elif line[index] == '.':
                # 1..2 => range op
                if index + 1 < len(line) and line[index + 1] == '.':
                    break
                if is_float:
                    raise Exception("Twice . in a number")
                is_float = True
                word += '.'
            else:
                suspended = False
                word += line[index]
            index += 1
        if not is_float:
            token = Token(Token.Integer, word, start)
        else:
            token = Token(Token.Float, word, start)
        if self.debug:
            print("    " + f'Creating token {token} n°{len(self.tokens)}')
        self.tokens.append(token)
        return index

    def read_id(self, line: str, start: int):
        """Read an identifier or a keyword from a string"""
        index = start
        if self.debug:
            msg = f'read_id at {index}, line {self.line_count}, starts with [{line[index]}]'
            print("    " + msg)
        word = line[index]
        terminate = False
        index += 1
        while index < len(line) and (line[index].isalnum() or line[index] in ['_', '!', '?']):
            if terminate:
                raise Exception("Wrong ID with ! or ? inside of it at line " + str(self.line_count))
            if line[index] in ['!', '?']:
                terminate = True
            word += line[index]
            index += 1
        if word in Tokenizer.OPERATORS:
            token = Token(Token.Operator, word, start)
        elif word in Tokenizer.KEYWORDS:
            token = Token(Token.Keyword, word, start)
        elif word in Tokenizer.BOOLEANS:
            token = Token(Token.Boolean, word, start)
        else:
            token = Token(Token.Identifier, word, start)
        if self.debug:
            print("    " + f'Creating token {token} n°{len(self.tokens)}, returning index {index}')
        self.tokens.append(token)
        return index

    def read_operator(self, line: str, start: int):
        """Read an operator from a string"""
        index = start
        if self.debug:
            msg = f'read_operator at {index}, line {self.line_count}, starts with [{line[index]}]'
            print("    " + msg)
        operator = line[index]
        index += 1
        while index < len(line) and line[index] in Tokenizer.START_OF_OPERATOR:
            operator += line[index]
            index += 1
        if operator not in Tokenizer.OPERATORS:
            raise Exception("Operator unknown: " + operator + " at line " + str(self.line_count))
        token = Token(Token.Operator, operator, start)
        if self.debug:
            print("    " + f'Creating token {token} n°{len(self.tokens)}')
        self.tokens.append(token)
        return index

    def read_separator(self, line: str, start: int):
        """Read a separator"""
        index = start
        if self.debug:
            msg = f'read_separator at {index}, line {self.line_count}, starts with [{line[index]}]'
            print("    " + msg)
        if line[index] == '\n':
            token = Token(Token.NewLine, 'NEWLINE', start) #line[index], self.counter)
        else:
            token = Token(Token.Separator, line[index], start)
        if self.debug:
            print("    " + f'Creating token {token} n°{len(self.tokens)}')
        self.tokens.append(token)
        return index+1

    def read_string(self, line: str, start: int):
        """Read a string"""
        index = start
        if self.debug:
            msg = f'read_string at {index}, line {self.line_count}, starts with [{line[index]}]'
            print("    " + msg)
        terminator = line[index]
        index += 1
        escaped = False
        word = ''
        while index < len(line) and (escaped or line[index] != terminator):
            escaped = (line[index] == '\\' and not escaped)
            word += line[index]
            index += 1
        if index >= len(line):
            raise Exception("Terminator char not found for string!")
        if word == '\\n':
            word = '\n'
        token = Token(Token.String, word, start)
        if self.debug:
            print("    " + f'Creating token {token} n°{len(self.tokens)}')
        self.tokens.append(token)
        return index+1

    def tokenize(self, source: str, debug: bool = False, to_s: bool = False):
        """Transform a string into a list of tokens"""
        if debug:
            print('[INFO] Start lexing')
        self.debug = debug
        self.tokens.clear()
        index = 0
        line_position = 0
        self.line_count = 1
        skip_line = False
        while index < len(source):
            char = source[index]
            line_position += 1
            if char == '-' and index + 1 < len(source) and source[index + 1] == '-':
                skip_line = True
            if skip_line and char == '\n':
                skip_line = False
                self.line_count += 1
                line_position = 0
            elif skip_line:
                index += 1
            else:
                if char.isdigit(): # 0 1 2 3 4 5 6 7 8 9
                    index = self.read_number(source, index)
                elif char.isalpha() or char in Tokenizer.START_OF_ID: # a-z A-Z @ _
                    index = self.read_id(source, index)
                elif char.isspace() and char != '\n': # ' ' \t
                    index += 1
                elif char in Tokenizer.START_OF_STRING:
                    index = self.read_string(source, index)
                elif char in Tokenizer.START_OF_OPERATOR:
                    index = self.read_operator(source, index)
                elif char in Tokenizer.SEPARATORS:
                    index = self.read_separator(source, index)
                else:
                    msg = f"[TOKENS] Unknown {char} at line {self.line_count} pos {line_position}"
                    raise Exception(msg)
        if self.debug:
            print(f'[INFO] {len(self.tokens)} tokens created:')
            for i in range(0, len(self.tokens)):
                print(f'{i}. {self.tokens[i]}')
        if not to_s:
            return self.tokens
        else:
            return self.format(self.tokens)
