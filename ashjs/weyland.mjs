const LANGUAGES = {
    'bnf-mini': new Language('bnf-mini',
        {
            'keyword': ['<[\\w- ]+>'],   // non-terminal
            'string' : PATTERNS['STRINGS'], // terminal
            'operator': ['::=', '\\|'],    // affect and choice
            'blank': PATTERNS['BLANKS'],
            'newline' : PATTERNS['NEWLINES'],
            'comment': ['\\#.*'],
        }
    ),
    'fr': new Language('fr',
        {
            'word': ['[a-zA-ZéàèùâêîôûëïüÿçœæÉÀÈÙÂÊÎÔÛËÏÜŸÇŒÆ]+'],
            'punct': [',', '\\.', ':', ';', '-', '\\(', '\\)', '!', '\\?', "'", '"'],
            'blank': [' ', '\n', '\t']
        }
    ),
    'lua': new Language('lua',
        {
            'keyword': ['and', 'break', 'do', 'else', 'elseif', 'end', 'for',
                        'function', 'goto', 'if', 'in', 'local', 'not', 'or',
                        'repeat', 'return', 'then', 'until', 'while'],
            'special': ['ipairs', 'pairs', '\\?', 'print'], // ? is here for demonstration only */
            'boolean': ['true', 'false'],
            'nil' : ['nil'],
            'identifier' : PATTERNS['IDENTIFIER'],
            'number' : ['\\d+', '\\d+\\.\\d+'],
            'string' : PATTERNS['STRINGS'],
            'operator': ['==', '~=', '<', '<=', '>', '>=',
                         '=',
                         '\\+', '\\*', '-', '/', '%', '\\^',
                         '&', '\\|', '~', '>>', '<<',
                         '\\.', '\\.\\.',
                         '#', ':'],
            'separator': ['\\{', '\\}', '\\(', '\\)', '\\[', '\\]', ',', ';'],
            'comment': ['--(?!\\[\\[).*(\n|$)', '--\\[\\[[\\s\\S]*--\\]\\](\n|$)'],
            'intermediate_comment': ['--\\[\\[[\\s\\S]*'],
            'newline' : PATTERNS['NEWLINES'],
            'blank': PATTERNS['BLANKS'],
            'wrong_int' : PATTERNS['WRONG_INTEGER'],
        },
        ['wrong_integer'],
        {
            'ante_identifier': ['function'],
        }
    ),
    'python': new Language('python',
        {
            'keyword' : ['await', 'else', 'import', 'pass', 'break', 'except', 'in',
                     'raise', 'class', 'finally', 'is', 'return', 'and', 'for',
                     'continue', 'lambda', 'try', 'as', 'def', 'from', 'while',
                     'nonlocal', 'assert', 'del', 'global', 'not', 'with', 'if',
                     'async', 'elif', 'or', 'yield'],
            'special': ['print'],
            'identifier' : PATTERNS["IDENTIFIER"],
            'integer' : PATTERNS["INTEGER"].concat(PATTERNS["INTEGER_HEXA"]).concat(PATTERNS["INTEGER_BIN"]),
            'float' : PATTERNS["FLOAT"],
            'boolean' : ['True', 'False'],
            'string' : PATTERNS["STRINGS"],
            'nil': ['None'],
            'operator': ['\\+', '/', '//', '&', '\\^', '~', '\\|', '\\*\\*', '<<', '%', '\\*',
                      '-', '>>', ':', '<', '<=', '==', '!=', '>=', '>', '\\+=',
                      '&=', '/=', '<<=', '%=', '\\*=', '\\|=', '\\*\\*=', '>>=', '-=',
                      '/=', '\\^=', '\\.', '='],
            'separator': ['\\{', '\\}', '\\(', '\\)', '\\[', '\\]', ',', ';'],
            'comment': ['#[^\n]*(\n|$)'],
            'newline' : PATTERNS["NEWLINES"],
            'blank': PATTERNS["BLANKS"],
            'wrong_int' : PATTERNS["WRONG_INTEGER"],
        },
        ['wrong_int'],
        // Special
        {
            'ante_identifier': ['def', 'class'],
        }
    ),
    'test': new Language('test',
        {
            'keyword': ['if', 'then', 'end'],
            'identifier': PATTERNS['IDENTIFIER'],
            'integer': PATTERNS['INTEGER'].concat(PATTERNS['INTEGER_HEXA']),
            'wrong_integer': PATTERNS['WRONG_INTEGER'],
            'float': PATTERNS['FLOAT'],
            //'wrong_float': WRONG_FLOAT,
            'blank': PATTERNS['BLANKS'],
            'newline': PATTERNS['NEWLINES'],
            'operators': PATTERNS['OPERATORS'],
            'separators': PATTERNS['SEPARATORS'],
            'strings': PATTERNS['STRINGS']
        },
        ['wrong_integer']), //, 'wrong_float']);
    'text': new Language('text', {
        'normal': ['[^ \\t]*'],
        'blank': PATTERNS['BLANKS'],
        'newline': PATTERNS['NEWLINES'],
        }),
}

const LEXERS = {
    'ash': new Lexer(LANGUAGES['ash'], ['blank']),
    'bnf': new Lexer(LANGUAGES['bnf'], ['blank']),
    'bnf-mini': new Lexer(LANGUAGES['bnf-mini'], ['blank']),
    'fr': new Lexer(LANGUAGES['fr'], ['blank']),
    'game': new Lexer(LANGUAGES['game'], ['blank', 'newline']),
    'hamill': new Lexer(LANGUAGES['hamill'], ['blank']),
    'json': new Lexer(LANGUAGES['json'], ['blank', 'newline']),
    'line': new Lexer(LANGUAGES['line']),
    'lua': new Lexer(LANGUAGES['lua'], ['blank']),
    'python': new Lexer(LANGUAGES['python']),
    'text': new Lexer(LANGUAGES['text'], ['blank']),
}

let a = [
    new Test(LEXERS['fr'], "bonjour l'ami !", ['word', 'word', 'punct', 'word', 'punct']),
    new Test(LEXERS['text'], "je suis là", ['normal', 'normal', 'normal']),
    new Test(LEXERS['game'], "Baldur's Gate\nTotal Annihilation\nHalf-Life\nFar Cry: Blood Dragon",
             ['normal', 'normal', 'normal', 'normal', 'normal', 'normal', 'normal', 'operator', 'normal', 'normal']),
    new Test(LEXERS['json'], "{'alpharius': 20, 'heretic': true}",
             ['separator', 'string', 'separator', 'number', 'separator', 'string', 'separator', 'boolean', 'separator']),
    new Test(LEXERS['bnf'], "<rule 1> ::= 'terminal1' 'terminal2'",
             ['keyword', 'operator', 'string', 'string']),
    new Test(LEXERS['bnf-mini'], "<rule xtrem> ::= 'terminal xtrem'", ['keyword', 'operator', 'string']),
    new Test(LEXERS['python'], "def a():\n\tif a == 5:\n\t\tprint('hello')",
             ['keyword', 'blank', 'identifier', 'separator', 'separator', 'operator', 'newline',
              'blank', 'keyword', 'blank', 'identifier', 'blank', 'operator', 'blank', 'integer', 'operator', 'newline',
              'blank', 'special', 'separator', 'string', 'separator']),

    new Test(LEXERS['hamill'], "§§ ceci est un commentaire\n§§ ceci est un autre", ['comment', 'comment']),
    new Test(LEXERS['lua'], '3+5', ['number', 'operator', 'number']),
    new Test(LEXERS['lua'], 'a = 5', ['identifier', 'operator', 'number']),
    new Test(LEXERS['lua'], 't = { ["k1"] = 5 }', ['identifier', 'operator', 'separator', 'separator', 'string', 'separator', 'operator', 'number', 'separator']),
    new Test(LEXERS['lua'], 't = { ["k1"] = 5, ["k2"] = "v", [4] = 6 } -- Définition\nprint(t["k1"]) -- Accès\nprint(t.k1) -- Accès avec sucre syntaxique',
            ['identifier', 'operator', 'separator', 'separator', 'string', 'separator', 'operator', 'number', 'separator',
             'separator', 'string', 'separator', 'operator', 'string', 'separator', 'separator', 'number', 'separator', 'operator', 'number',
             'separator', 'comment', 'special', 'separator', 'identifier', 'separator', 'string', 'separator', 'separator', 'comment',
             'special', 'separator', 'identifier', 'operator', 'identifier', 'separator', 'comment']),
    new Test(LEXERS['lua'], '--[[Ceci est un\nz--]]', ['comment']),
    new Test(LEXERS['lua'], '--[[Ceci est un\ncommentaire multiligne--]]', ['comment']),

    new Test(LEXERS['ash'], 'writeln("hello")', ['special', 'separator', 'string', 'separator']),
    new Test(LEXERS['ash'], 'if a == 5 then\n    writeln("hello")\nend',
                ['keyword', 'identifier', 'operator', 'integer', 'keyword', 'newline',
                 'special', 'separator', 'string', 'separator', 'newline',
                 'keyword']),

    new Test(LEXERS['hamill'], "**bold * \\** text**", ['bold', 'normal', 'bold']),
    new Test(LEXERS['hamill'], "**bold ''text''**", ['bold', 'normal', 'italic', 'normal', 'italic', 'bold']),

];



