// This file provides several languages definitions :
//   text
//   bnf
//   hamill
//   game
//   ash
//   json
//   lua
//   python
//   line
//   test

class Test
{
    constructor(lexer, text, result)
    {
        this.lexer = lexer;
        this.text = text;
        this.result = result;
        if (this.result === null || this.result === undefined)
        {
            throw new Error(`No expected results for test ${text}`);
        }
    }

    test(num=0, debug=false)
    {
        let tokens = this.lexer.lex(this.text, null, debug);
        if (tokens.length !== this.result.length)
        {
            console.log('Difference of length, dumping:')
            let longuest = Math.max(tokens.length, this.result.length);
            for (let index = 0; index < longuest; index++)
            {
                if (index < tokens.length && index < this.result.length)
                {
                    let cmp = (this.result[index] === tokens[index].getType());
                    console.log(`${index}. ${cmp} Expected=${this.result[index]} vs ${tokens[index].getType()} (${ln(tokens[index].getValue())})`);
                } else if (index < tokens.length) {
                    console.log(`${index}. Expected=null [null] vs ${tokens[index].getType()}`, ln(tokens[index].getValue()));
                } else if (index < this.result.length) {
                    console.log(`${index}. Expected=${this.result[index]} vs null`);
                }
            }
            throw new Error(`Error: expected ${this.result.length} tokens and got ${tokens.length}`);
        }
        for (const [index, r] of this.result.entries())
        {
            if (tokens[index].getType() !== r)
            {
                throw new Error(`Error: expected ${r} and got ${tokens[index].getType()} in ${this.text}`);
            }
        }
        console.log(`[SUCCESS] Test n°${num} Lang : ${this.lexer.getLanguage()}\nText : |${ln(this.text)}|\nResult:`);
        for (const tok of tokens)
        {
            console.log(tok);
        }
    }
}


const LANGUAGES = {
    'ash': new Language('ash',
        {
            'keyword' : ['if', 'then', 'elif', 'else', 'end',
                'while', 'do', 'for',
                'break', 'next', 'return',
                'var', 'fun', 'sub', 'get', 'set', 'class',
                'import', 'from', 'as',
                'try', 'catch', 'finally', 'raise', 'const'],
            'special': ['writeln', 'write'],
            'boolean' : ['false', 'true'],
            'operator' : ['-', 'not', '#', '~', 'and', 'or', // boolean
                'in', // belongs to
                '\\+', '-', '\\*', '/', '//', '\\*\\*', '%', // mathematical
                '&', '\\|', '~', '>>', '<<', // bitwise
                '<', '<=', '>', '>=', '==', '!=', // comparison
                '\\.'], // call
            'identifier' : PATTERNS["IDENTIFIER"],
            // Old
            'affectation' : ['='],
            'combined_affectation' : ['\\+=', '-=', '\\*=', '/=', '//=', '\\*\\*=', '%='],
            'type' : [':', '->'],
            'fast' : ['=>'],
            'label' : ['::'],
            // 'unary_operator' : ['-', 'not', r'\#', '~'],
            // New
            'integer' : PATTERNS["INTEGER"].concat(PATTERNS["INTEGER_BIN"]).concat(PATTERNS["INTEGER_HEXA"]),
            'number' : PATTERNS["FLOAT"],
            'nil': ['nil'],
            // 'binary_operator' : ['and', 'or', # boolean
            'separator': ['\\{', '\\}', '\\(', '\\)', '\\[', '\\]', ',', ';'],
            'wrong_int' : PATTERNS["WRONG_INTEGER"],
            'blank': PATTERNS["BLANKS"],
            'newline' : PATTERNS["NEWLINES"],
            'comment': ['--[^\n]*'],
            'string' : PATTERNS["STRINGS"],
        },
        ['wrong_int'],
        // Special
        {
            'ante_identifier': ['var', 'const', 'function', 'procedure', 'fun', 'pro', 'class', 'module'],
        }
    ),
    'bnf': new Language('bnf',
        {
            'keyword': ['<[\\w- ]+>'],  // non-terminal
            'identifier': ['expansion', 'A', 'B', 'C', 'D', 'nom'], // expansion
            'operator': ['::=', '\\|', '\\.\\.\\.', '=', '-', '\\?', '\\*', '\\+', '@', '\\$', '_'],
            'separator': ['\\(', '\\)', '\\[', '\\]', '\\{', '\\}', ',', ';'],
            'string' : ['"[\\w- <>:=,;\\|\']*"', "'[\\w- <>:=,;\\|\"]*'"], // terminal
            'blank': PATTERNS['BLANKS'],
            'comment': ['#[^\n]*\n'],
            'newline' : PATTERNS['NEWLINES'],
        }
    ),
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
    'game': new Language('game',
        {
            'number': ['\\d+'],
            'normal': ['\\w[\\w\'-]*'], // Total Annihilation => 2 tokens, Baldur's => 1, Half-life => 1
            'blank': PATTERNS['BLANKS'],
            'wrong_int' : PATTERNS['WRONG_INTEGER'],
            'newline' : ['\n'],
            'operator': [':'] // FarCry:
        }
    ),
    'hamill' : new Language('hamill',
        {
            'keyword': ['var', 'const', 'include', 'require', 'css', 'html'],
            'newline' : PATTERNS["NEWLINES"],
            'comment': ['§§.*(\n|$)'],
            'bold': ['\\*\\*'],
            'italic': ["''"],
            'special': ['\\*', "'"],
            'normal': ["([^\\\\*'§\n]|\\\\\\*\\*|\\\\\\*|\\\\''|\\\\')+"]
        },
        ['wrong_int'],
        // Special
        {
            'ante_identifier': ['var', 'const'],
            'string_markers': [],
        },
        function(tokens)
        {
            let res = [];
            // Première passe, fusion des speciaux
            for (const [index, tok] of tokens.entries())
            {
                if (tok.getType() === 'special')
                {
                    if (index > 0 && res.length > 0 && res[res.length - 1].getType() === 'normal')
                    {
                        res[res.length - 1].value += tok.getValue();
                    }
                    else if (index + 1 < tokens.length && tokens[index + 1].getType() === 'normal')
                    {
                        tokens[index + 1].value = tok.getValue() + tokens[index + 1].value;
                        tokens[index + 1].start -= tok.getValue().length;
                    }
                } else {
                    res.push(tok);
                }
            }
            // Seconde passe, fusion des normaux
            let res2 = [];
            let index = 0;
            while (index < res.length)
            {
                let tok = res[index];
                if (tok.getType() === 'normal')
                {
                    let futur = index + 1;
                    let merged_value = tok.getValue();
                    while (futur < res.length && res[futur].getType() === 'normal')
                    {
                        merged_value += res[futur].getValue();
                        futur += 1;
                    }
                    tok.value = merged_value;
                    res2.push(tok);
                    index = futur;
                }
                else
                {
                    res2.push(tok);
                    index+=1;
                }
            }
            return res2;
        }
    ),
    'json': new Language('json',
        {
            'boolean': ['true', 'false'],
            'identifier' : PATTERNS['IDENTIFIER'],
            'number' : PATTERNS['INTEGER'].concat(PATTERNS['FLOAT']),
            'string' : PATTERNS['STRINGS'],
            'nil': [],
            'keyword': ['null'],
            'operator': [],
            'separator': ['\\{', '\\}', '\\(', '\\)', '\\[', '\\]', ',', ':', "\\."],
            'comment' : [],
            'newline' : PATTERNS['NEWLINES'],
            'blank': PATTERNS['BLANKS'],
            'wrong_int' : PATTERNS['WRONG_INTEGER'],
        },
        ['wrong_int'],
        // Special
        {
            'ante_identifier': [],
        }
    ),
    // Un langage qui divise simplement en lignes
    'line': new Language('line',
        {
            'line': ['.*(\n|$)']
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

const TESTS = [
    new Test(LEXERS['line'], "bonjour\ntoi qui\nvient de loin", ['line', 'line', 'line']),
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

    new Test(LEXERS['ash'], "a ** 5", ['identifier', 'operator', 'integer']),
    new Test(LEXERS['ash'], 'writeln("hello")', ['special', 'separator', 'string', 'separator']),
    new Test(LEXERS['ash'], 'if a == 5 then\n    writeln("hello")\nend',
                ['keyword', 'identifier', 'operator', 'integer', 'keyword', 'newline',
                 'special', 'separator', 'string', 'separator', 'newline',
                 'keyword']),

    new Test(LEXERS['hamill'], "**bold * \\** text**", ['bold', 'normal', 'bold']),
    new Test(LEXERS['hamill'], "**bold ''text''**", ['bold', 'normal', 'italic', 'normal', 'italic', 'bold']),
    new Test(LEXERS['ash'], 'a = 5', ['identifier', 'affectation', 'integer']),
]

function tests(debug=false)
{
    const text = "if a == 5 then\nprintln('hello')\nend\nendly = 5\na = 2.5\nb = 0xAE\nc = 2.5.to_i()\nd = 2.to_s()\n"; //5A";
    let lexer = new Lexer(LANGUAGES['test'], ['blank']);
    let tokens = lexer.lex(text);
    console.log('Text:', text);
    for (const [index, tok] of tokens.entries())
    {
        console.log(`${index.toString().padStart(4)}  ` + tok.toString());
    }

    for (const [index, t] of TESTS.entries())
    {
        t.test(index + 1, debug);
    }

    console.log("\n--- Test of to_html ------------------------------------------\n");
    console.log(LEXERS['lua'].to_html("if a >= 5 then println('hello') end", null, ['blank']));
}

tests(false);


