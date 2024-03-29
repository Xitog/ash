import copy # only once for read_expr

class Token:
    Integer = 'integer'
    Number = 'number'
    Identifier = 'identifier'
    Operator = 'operator'
    Separator = 'separator'
    Keyword = 'keyword'
    NewLine = 'newline'
    Boolean = 'boolean'
    String = 'string'
    Comment = 'comment'
    Affectation = 'AFFECTATION' #'affectation'
    Type = 'TYPE' # 'type'

# Model

class AST:

    def __init__(self):
        self.root = None

    def __str__(self):
        return self.root.to_s()

    def to_s(self, level=1):
        return self.root.to_s()

    def to_html_list(self, s, level, n):
        s += '  ' * level + '<li>' + n.get_name() + '\n'
        level += 1
        if hasattr(n, 'get_children'):
            lst = 'ol' if type(n) == Block else 'ul'
            s += '  ' * level + f'<{lst}>\n'
            inner = ''
            for c in n.get_children():
                inner = self.to_html_list(inner, level + 1, c)
            s += inner
            s += '  ' * level + f'</{lst}>\n'
        level -= 1
        s += '  ' * level + '</li>\n'
        return s

    def to_html(self):
        s = '    <ul>\n'
        s = self.to_html_list(s, 3, self.root)
        s += '    </ul>\n'
        return s


class Node:

    Terminal = "Terminal" # content is Token, right and left None
    Operation = "Operation" # content is Token.Operator, right and left are operands
    VarDeclaration = "VarDeclaration" # content is Token.Operator, right is ListTypedVar, left is expr
    TypedVar = "TypedVar" # content is the identifier (Token.Terminal), right is the type (Token.Terminal)
    ListTypedVar = "ListTypedVar" # list of TypedVar

    def __init__(self, content : Token, typ=None, right=None, left=None):
        if typ is None:
            if right is None and left is None:
                self.typ = Node.Terminal
            else:
                raise Exception("[ERROR] Cannot guess Node type")
        self.typ = typ
        self.content = content
        self.right = right
        self.left = left
        self.lvl = content.lvl if hasattr(content, 'lvl') else 0

    def to_s(self, level=1):
        s = "    " * level + "{Terminal}\n"
        s += self.content.to_s(level + 1)
        return s

    def __str__(self):
         return self.to_s()

    def is_terminal(self):
        return self.right is None and self.left is None

    def get_name(self):
        return '{' + self.typ + '}'

    def get_children(self):
        if self.left is not None:
            yield self.left
        if self.right is not None:
            yield self.right


class TypedVar(Node):

    def __init__(self, identifier, typ):
        Node.__init__(self, content=identifier, typ=Node.TypedVar, right=typ)

    def to_s(self, level=1):
        return "    " * level + self.get_name()

    def get_name(self):
        right = str(self.right)
        return f"<TypedVar {self.content} of type {right}>"


class ListTypedVar(Node):

    def __init__(self, lst):
        Node.__init__(self, content=lst, typ=Node.ListTypedVar)

    def to_s(self, level=1):
        s = "    " * level + "<ListTypedVar>\n"
        for e in self.content:
            s += e.to_s(level + 1) + "\n"
        s += "    " * level + "</ListTypedVar>\n"
        return s


class VarDeclaration(Node):

    def __init__(self, content, right, left, const_ref):
        Node.__init__(self, content, Node.VarDeclaration, right, left)
        self.const_ref = const_ref

    def to_s(self, level=1):
        s = "    " * level + f"<{self.typ} operator={self.content} const_ref={self.const_ref}>\n"
        s += self.right.to_s(level + 1)
        s += self.left.to_s(level + 1) + "\n"
        s += "    " * level + f"</{self.typ}>\n"
        return s

    def get_name(self):
        return f"{self.typ} {self.content.typ} {self.content.val}"


class Terminal(Node):

    def __init__(self, content):
        Node.__init__(self, content, Node.Terminal)

    def to_s(self, level=1):
        s = "    " * level + self.get_name()
        return s

    def get_name(self):
        return f"<Terminal {self.content.typ} {self.content.val} ({self.content.first} +{self.content.length})/>"


class Block(Node):

    def __init__(self):
        self.actions = []

    def add(self, action):
        self.actions.append(action)

    def to_s(self, level=1):
        output = "    " * level + "<Block>\n"
        for elem in self.actions:
            if elem is None:
                raise Exception('None element in Block detected')
            output += elem.to_s(level+1)
            if output[-1] != '\n':
                output += '\n'
        output += "    " * level + "</Block>\n"
        return output

    def is_terminal(self):
        return len(self.actions) == 0

    def get_name(self):
        return '<Block>'

    def get_children(self):
        for elem in self.actions:
            yield elem


class Operation(Node):

    def __init__(self, operator : Token, left=None, right=None):
        Node.__init__(self, operator, Node.Operation, right, left)
        self.operator = self.content
        assert self.operator.is_terminal() and self.operator.content.typ == Token.Operator, "Operator should be of type Token.Operator and is " + self.operator.content.typ

    def to_s(self, level=1):
        name = self.get_name()
        s = "    " * level + f"{name}\n" # {self.content.content.val}
        if self.right is not None:
            s += "    " * (level + 1) + "right =\n"
            s += self.right.to_s(level + 2) + "\n"
        if self.left is not None:
            s += "    " * (level + 1) + "left =\n"
            s += self.left.to_s(level + 2) + "\n"
        return s

    def get_name(self):
        n = "{Operation} Binary" if self.left is not None and self.right is not None else "Unary"
        n += ' ' + self.content.content.val
        return n


class FunCall(Node):

    def __init__(self, name, arg): # mono arg
        assert type(name) == Terminal and name.content.typ == Token.Identifier, "Name of a function should be an identifier"
        self.name = name
        self.arg = arg

    def to_s(self, level=1):
        typ = "{Function Call}"
        return "    " * level + f"{typ} {self.name} {self.arg.to_s()} \n"

    def is_terminal(self):
        return True


class Statement(Node):

    def __init__(self, cond, action, alter=None, loop=False, on_false=False):
        self.cond = cond
        self.action = action
        self.alter = alter
        self.loop = loop
        self.on_false = on_false

    def get_name(self):
        if not self.on_false and not self.loop:
            return '{Statement} if'
        elif not self.on_false and self.loop:
            return '{Statement} while'
        elif self.on_false and not self.loop:
            return '{Statement} unless'
        elif self.on_false and self.loop:
            return '{Statement} until'
        else:
            raise Exception('Statement is not in a valid state')

    def to_s(self, level=1):
        start = "    " * level
        block = "    " * (level + 1)
        s = start + self.get_name() + '\n' + block + 'Cond:\n' + self.cond.to_s(level + 2)
        s += block + 'Action:\n' + self.action.to_s(level + 2)
        if self.alter is not None:
            s += block + 'Else:\n' + self.alter.to_s(level + 2)
        s += start + 'end\n'
        return s

    def __str__(self):
        return self.get_name()

    def get_children(self):
        yield self.cond
        yield self.action
        if self.alter is not None:
            yield self.alter


class Parser:

    PRIORITIES = {
        '=' : 1, '+=' : 1, '-=' : 1, '*=' : 1, '/=' : 1, '//=' : 1, '**=' : 1, '%=' : 1,
        ',' : 2,
        'and' : 5, 'or' : 5, 'xor' : 5,
        '>' : 8, '<' : 8, '>=' : 8, '<=' : 8, '==' : 8, '!=' : 8, '<=>' : 8,
        '<<': 9, '>>' : 9, '..' : 9, '..<' : 9,
        '+' : 10, '-' : 10,
        '*' : 20, '/' : 20, '//' : 20,
        '**' : 30, '%' : 30,
        'call' : 35,
        'call(' : 40,
        '.' : 50,
        'not' : 51,
        'unary-' : 52,
        'expr(' : 60,
    }

    def __init__(self, debug = False):
        self.level_of_ana = 0
        self.debug = debug
        self.debug_expression = False # expression are too verbose, we must try to segregate

    def set_debug(self):
        self.debug = not self.debug

    def parse(self, tokens):
        if self.debug:
            print('[INFO] Start parsing')
        if tokens is None:
            raise Exception("tokens is None!")
        # clean tokens
        cleaned = [tok for tok in tokens if tok.typ not in [Token.Comment]] # Token.NewLine,
        tokens = cleaned
        index = 0
        ast = AST()
        ast.root = self.read_block(tokens, 0)[1] # index is discared
        return ast

    def get_end(self, tokens, index):
        level = 0
        for t in range(index + 1, len(tokens)):
            tok = tokens[t]
            if tok.typ == Token.Keyword and tok.value in ['if', 'while', 'for']:
                level += 1
            elif tok.typ == Token.Keyword and tok.value == 'end' and level == 0:
                return t
            elif tok.typ == Token.Keyword and tok.value == 'end' and level > 0:
                level -= 1
        # should have an "end" keyword at the end
        raise Exception("[ERROR] Parser: malformed expression")

    def get_else(self, tokens, index, end):
        level = 0
        t = index + 1
        for t in range(index + 1, end):
            tok = tokens[t]
            if tok.typ == Token.Keyword and tok.value in ['if', 'while', 'for']:
                level += 1
            elif tok.typ == Token.Keyword and tok.value in ['else', 'elif'] and level == 0:
                return t
            elif tok.typ == Token.Keyword and tok.value == 'end' and level > 0:
                level -= 1
        return None

    def read_block(self, tokens, index, max_index=None):
        if max_index is None: max_index = len(tokens)
        #print('    ' * self.level_of_ana, 'read_block from', index, 'to', max_index)
        block = Block()
        while index < max_index:
            tok = tokens[index]
            if tok.typ == Token.Keyword and tok.value == 'if':
                end = self.get_end(tokens, index)
                els = self.get_else(tokens, index, end)
                index, node = self.read_if(tokens, index + 1, els, end)
                block.add(node)
            elif tok.typ == Token.Keyword and tok.value in ['while', 'for']:
                end = self.get_end(tokens, index)
                if tok.value == 'while':
                    index, node = self.read_while(tokens, index + 1, end)
                else:
                    index, node = self.read_for(tokens, index + 1, end)
                block.add(node)
            elif tok.typ == Token.NewLine: # discard not meaningfull newline
                index += 1
            else:
                end = self.get_keyword_or_nl(tokens, index, max_index, ['else', 'end'])
                end = max_index if end is None else end
                aff = self.get_aff(tokens, index, end)
                if (end == max_index or tokens[end].typ == Token.NewLine) and aff is not None:
                    const_ref = False
                    if tokens[index].typ == Token.Keyword and tokens[index].val == 'var':
                        index += 1 # skip var
                    elif tokens[index].typ == Token.Keyword and tokens[index].val == 'const':
                        index += 1 # skip const
                        const_ref = True
                    ids = self.read_id_list(tokens, index, aff - 1)
                    index, node = self.read_expr(tokens, aff + 1, end)
                    node = VarDeclaration(tokens[aff], ids, node, const_ref)
                else:
                    index, node = self.read_expr(tokens, index, end)
                block.add(node)
        return index, block

    def get_aff(self, tokens, index, max_index):
        for t in range(index, max_index):
            if tokens[t].typ == Token.Affectation and tokens[t].value == '=':
                return t
        return None

    def get_keyword_or_nl(self, tokens, index, max_index, keywords):
        if type(keywords) == str:
            keywords = [keywords]
        for t in range(index, max_index):
            if tokens[t].typ == Token.NewLine or (tokens[t].typ == Token.Keyword and tokens[t].value in keywords):
                return t
        return None

    def read_id_list(self, tokens, index, end):
        lst = []
        if tokens[index].typ == Token.Identifier and index-end == 0:
            lst.append(TypedVar(identifier=Terminal(tokens[index]), typ=None))
        else:
            after_column = False
            identifier = None
            typ = None
            for i in range(index, end + 1):
                if tokens[i].typ == Token.Identifier:
                     if not after_column:
                        if identifier is None:
                            identifier = Terminal(tokens[i])
                        else:
                            raise Exception("[ERROR] Parse (read_id_list) : An identifier cannot follow an identifier.")
                     else:
                        if typ is None:
                            typ = Terminal(tokens[i])
                        else:
                            raise Exception("[ERROR] Parse (read_id_list) : An type cannot follow a type.")
                        lst.append(TypedVar(identifier, typ))
                elif tokens[i].typ == Token.Type and tokens[i].value == ':':
                    if identifier is None:
                        raise Exception("[ERROR] Parse (read_id_list) : Column separator should be after an identifier.")
                    after_column = True
                else:
                    raise Exception(f"[ERROR] Parse (read_id_list) : Syntax is identifier : type, wrong token {tokens[i]}")
                index += 1
        return ListTypedVar(lst)

    def read_if(self, tokens, index, else_index, end_index):
        self.level_of_ana += 1
        #print('    ' * self.level_of_ana, 'read_if from', index, tokens[index], 'to', end_index)
        #for t in range(index, end_index):
        #    print('    ' * self.level_of_ana, ' -', tokens[t])
        # read condition
        cond = None
        t = self.get_keyword_or_nl(tokens, index, end_index, 'then')
        index, cond = self.read_expr(tokens, index, t)
        # read action
        action = None
        end = else_index if else_index is not None else end_index
        _, action = self.read_block(tokens, index, end)
        # read else action
        else_action = None
        if else_index is not None:
            if tokens[else_index].value == 'else':
                _, else_action = self.read_block(tokens, else_index + 1, end_index)
            else: # elif
                elif_else = self.get_else(tokens, else_index + 1, end_index)
                _, else_action = self.read_if(tokens, else_index + 1, elif_else, end_index)
        node = Statement(cond, action, else_action)
        self.level_of_ana -= 1
        return end_index + 1, node

    def read_while(self, tokens, index, end_index):
        # read condition
        cond = None
        t = self.get_keyword_or_nl(tokens, index, end_index, 'do')
        index, cond = self.read_expr(tokens, index, t)
        # read action
        action = None
        _, action = self.read_block(tokens, index, end_index)
        node = Statement(cond, action, loop=True)
        return end_index + 1, node

    def read_for(self, tokens, index, end_index):
        return 99

    def read_expr(self, tokens, index, end_index):
        #self.debug_expression = True
        if end_index is None: end_index = len(tokens)
        self.level_of_ana += 1
        # Sorting operators
        working_list = copy.deepcopy(tokens[index:end_index + 1])
        sorted_operators = []
        prio_values = []
        lvl = 1
        index = 0
        after_aug = False
        if self.debug_expression:
            print('================================')
            print('Building initial list')
            print('================================')
        while index < len(working_list):
            if after_aug:
                after_aug = False
                lvl *= 100
            token = working_list[index]
            token.lvl = lvl
            if self.debug_expression:
                print(f"    working_list {str(index)}. {token} lvl={token.lvl}")
            if token.typ in [Token.Operator, Token.Separator]:
                # Handling of ( )
                if token.value == '(':
                    if index >= 1 and working_list[index - 1].typ != Token.Operator: # not the first and not after an Operator
                        token.value = 'call('
                        token.typ = Token.Operator
                        after_aug = True
                    else:
                        lvl *= 100
                        del working_list[index]
                        continue
                elif token.value == ')':
                    lvl /= 100
                    del working_list[index]
                    continue
                # Handling of Operators
                if token.typ == Token.Operator:
                    if len(sorted_operators) == 0:
                        if self.debug_expression:
                            print('        => adding the first operator')
                        sorted_operators.append(index)
                        prio_values.append(Parser.PRIORITIES[token.value] * lvl)
                    else:
                        if self.debug_expression:
                            print('        => adding another operator')
                        computed_prio = Parser.PRIORITIES[token.value] * lvl
                        ok = False
                        for i in range(0, len(sorted_operators)):
                            if prio_values[i] < computed_prio:
                                sorted_operators.insert(i, index)
                                prio_values.insert(i, computed_prio)
                                ok = True
                                break
                        if not ok:
                            sorted_operators.append(index)
                            prio_values.append(computed_prio)
            index += 1
        # remove ending newline(s)
        while working_list[-1].typ == Token.NewLine:
            working_list.pop()
        if len(sorted_operators) == 0 and len(working_list) > 1:
            raise Exception("[ERROR] Incorrect expression len(sorted_operators) == 0")
        # Resolving operators
        length = len(working_list)
        if length <= 0:
            raise Exception("[ERROR] Parser: Empty expression of length = " + str(length))
        elif length == 1:
            if working_list[0].typ in [Token.Boolean, Token.Number, Token.Integer, Token.Identifier, Token.String]:
                node = Terminal(working_list[0])
                results = end_index + 1, node
            else:
                #for t in range(index, end_index):
                #    print(t, tokens[t])
                raise Exception("[ERROR] Parser: Expression of length 1 is not valid we have: " + str(working_list[0]))
        else:
            modifier = 0
            counter = 0 # only for debug
            length_at_start = len(sorted_operators) # only for debug
            while len(sorted_operators) > 0:
                # Debug Display
                counter += 1
                if self.debug_expression:
                    print('================================')
                    print('=', counter, '/', length_at_start)
                    print('================================')
                    print('--------------------')
                    print('Initial working list')
                    print('--------------------')
                    for index, token in enumerate(working_list):
                        print(f"    working_list {index}. {token.__class__.__name__} {token} lvl={token.lvl}")
                    print('----------------')
                    print('Sorted Operators')
                    print('----------------')
                    for so in range(0, len(sorted_operators)):
                        idx = sorted_operators[so]
                        obj = working_list[sorted_operators[so] + modifier]
                        print(f'    {so}. index={idx} type={obj.__class__.__name__} str={obj} modifier={modifier}')
                    print('-------------------')
                    print('Starting to resolve')
                    print('-------------------')
                # End of Debug Display
                operator_index = sorted_operators[0] + modifier
                if self.debug_expression:
                    print('    Resolving 0. in sorted_operators: ' + str(working_list[operator_index]))
                # Left parameter, None for Unary Operator
                op = working_list[operator_index]
                if (type(op) == Token and op.value == 'not') or (type(op) == Operation and op.operator.content.value == 'not'):
                    left = None
                elif ((type(op) == Token and op.value == '-') or (type(op) == Operation and op.operator.content.value == '-')) and operator_index - 1 < 0:
                    left = None
                else:
                    left = working_list[operator_index - 1]
                    if not isinstance(left, Node):
                        left = Terminal(left)
                # Right parameter
                right = None
                # We must have a parameter and its priority/lvl must be superior : ( "abc" ) abc is lvl = 100 vs () "abc" abc is lvl = 1
                if (type(op) == Token and op.value == 'call(') or (type(op) == Operation and op.operator.content.value == 'call('):
                    if len(working_list) > operator_index + 1 and working_list[operator_index + 1].lvl > working_list[operator_index].lvl:
                        right = working_list[operator_index + 1]
                # We must have always a right parameter (there is no unary op with left operand)
                else:
                    if len(working_list) > operator_index + 1:
                        right = working_list[operator_index + 1]
                if right is not None and not isinstance(right, Node):
                    right = Terminal(right)
                op = Terminal(working_list[operator_index])
                node = Operation(
                    op,
                    left,
                    right
                )
                # update working list
                if left is not None and right is not None:
                    del working_list[operator_index + 1] # right
                    del working_list[operator_index] # op
                    del working_list[operator_index - 1] # left
                    working_list.insert(operator_index - 1, node)
                elif left is None and right is not None:
                    del working_list[operator_index + 1] # right
                    del working_list[operator_index] # op
                    working_list.insert(operator_index, node)
                elif left is not None and right is None: # call( without arg
                    del working_list[operator_index] # op
                    del working_list[operator_index - 1] # left
                    working_list.insert(operator_index - 1, node)
                else: # left is None and right is None
                    del working_list[operator_index]
                    working_list.insert(operator_index, node)
                # update sorted operators
                if self.debug_expression:
                    print(f'    Updating working list, operator_index={operator_index}, left={left}, right={right}')
                    print('---------------------')
                    print('Modified working list')
                    print('---------------------')
                    for index, token in enumerate(working_list):
                        print(f"    working_list {index}. {token.__class__.__name__} {token} lvl={token.lvl}")
                    print('    Updating sorted operators')
                    print('    < Before')
                del sorted_operators[0]
                for so in range(0, len(sorted_operators)):
                    if sorted_operators[so] > operator_index:
                        if right is not None: # f()
                            sorted_operators[so] -= 2
                        else:
                            sorted_operators[so] -= 1
                if self.debug_expression:
                    print('    > After')
                    print('       ', sorted_operators)
            results = end_index + 1, node
        self.level_of_ana -= 1
        return results
