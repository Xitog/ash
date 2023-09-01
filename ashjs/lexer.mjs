// -----------------------------------------------------------
// MIT Licence (Expat License Wording)
// -----------------------------------------------------------
// Copyright © 2020, Damien Gouteux
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// For more information about my projects see:
// https://xitog.github.io/dgx (in French)

//-------------------------------------------------------------------------------
// Environment & imports
//-------------------------------------------------------------------------------

const node =
	typeof process !== "undefined" &&
	process !== null &&
	typeof process.version !== "undefined" &&
	process.version !== null &&
	typeof process.version === "string";

const fs = node ? await import("fs") : null;
const path = node ? await import("path") : null;

const main = (node) ? path.basename(process.argv[1]) === 'lexer.mjs': false;

//-------------------------------------------------------------------------------
// Classes
//-------------------------------------------------------------------------------

class Token
{
    constructor(type, value, start, line)
    {
        this.type = type;
        this.value = value;
        this.start = start;
        this.line = line;
    }

	// equals("keyword", "if") or equals("keyword", ["if", "while"])
    equals(type, value=null, start=null)
    {
        let ok_type  = this.type  === type;
        let ok_value = (value === null ? true : this.value === value);
        if (value !== null && Array.isArray(value)) {
            ok_value = value.includes(this.value);
        }
        let ok_start = (start === null ? true : this.start === start);
        return ok_value && ok_type && ok_start;
    }

    toHTML(lang, raw=false, debug=false)
    {
        let val = this.getValue();
        if (!raw) {
            val = val.replace('&', '&amp;');
            val = val.replace('>', '&gt;');
            val = val.replace('<', '&lt;');
            if (debug) {
                return `${val}<sub>${this.type}</sub>`;
            } else {
                return `<span class=${lang}-${this.type}>${val}</span>`
            }
        } else {
            return val;
        }
    }

	toHTMLTree(isRoot = false) {
        let val = this.getValue();
        val = val.replace('&', '&amp;');
        val = val.replace('>', '&gt;');
        val = val.replace('<', '&lt;');
		if (isRoot) {
			return `<ul class="monotree"><li data-type="${this.type}"><code>${val}</code><ul>`;
		} else {
			return `<code data-type="${this.type}">${val}</code>`;
		}
	}

    getType()
    {
        return this.type;
    }

    getValue()
    {
        return this.value;
    }

    getStart()
    {
        return this.start;
    }

    getLine()
    {
        return this.line;
    }

    toString(pad=null)
    {
        let val = this.value.replace(/\n/g, '\\n');
        if (pad === null) {
            return `{${this.type} |${val}|(${this.value.length}) @${this.start},L${this.line}}`;
        } else {
            return `{Token ${this.type.padEnd(pad)} |${(val + '|').padEnd(pad*2)}(${this.value.length}) @${this.start},L${this.line}`;
        }
    }
}

class Language
{
    static LANGUAGES = [];

    static readDefinition(raw=null)
    {
        if (fs !== null) {
            return fs.readdirSync('languages').map(fileName => {
                let raw = JSON.parse(fs.readFileSync(path.join('languages', fileName), 'utf8'));
                let wrong = [];
                if ('wrong' in raw) {
                    wrong = raw['wrong'];
                }
                Language.LANGUAGES[raw['name']] = new Language(raw['name'], raw['definitions'], wrong);
                return Language.LANGUAGES[raw['name']];
            });
        } else if (raw !== null) {
            let wrong = [];
            if ('wrong' in raw) {
                wrong = raw['wrong'];
            }
            Language.LANGUAGES[raw['name']] = new Language(raw['name'], raw['definitions'], wrong);
            return Language.LANGUAGES[raw['name']];
        }
    }

    constructor(name, definitions, wrong=[])
    {
        this.name = name;
        if (typeof definitions !== 'object')
        {
            throw new Error(`For lang |${name}|, definitions should be an object and it is a ` + typeof definitions);
        }
        this.definitions = definitions;
        for (const [type, patterns] of Object.entries(definitions))
        {
            if (patterns === null || patterns === undefined)
            {
                throw new Error(`No variants for ${type} in language ${name}`);
            }
        }
        // In order to match the entire string we put ^ and $ at the start of each regex
        for (const patterns of Object.values(definitions))
        {
            for (let index of Object.keys(patterns))
            {
                if (typeof patterns[index] !==  "object")
                {
                    let pattern = patterns[index];
                    if (pattern[0] !== '^') { pattern = '^' + pattern;}
                    if (pattern[pattern.length-1] !== '$') { pattern += '$'}
                    if (pattern.includes('[\\s\\S]'))
                    {
                        patterns[index] = new RegExp(pattern, 'm');
                    } else {
                        patterns[index] = new RegExp(pattern);
                    }
                }
            }
        }
        this.wrong = wrong;
    }

    isWrong(type)
    {
        return this.wrong.includes(type);
    }

    getName()
    {
        return this.name;
    }

    getTypeDefinitions()
    {
        return Object.entries(this.definitions);
    }

    getNumberOfTypes()
    {
        return Object.keys(this.definitions).length;
    }

    getNumberOfRegex()
    {
        let sum = 0;
        for (const patterns of Object.values(this.definitions))
        {
            sum += patterns.length;
        }
        return sum;
    }

    toString()
    {
        return `Language ${this.getName()} with ${this.getNumberOfTypes()} types and ${this.getNumberOfRegex()} regex`;
    }
}

class Match
{
    constructor(type, elem, start, line)
    {
        this.type = type;
        this.elem = elem;
        this.start = start;
        this.line = line;
    }

    toString()
    {
        return `Match type=${this.type} elem=${this.elem} start=${this.start} line=${this.line}`;
    }
}

class Lexer
{
    constructor(lang, discards=[])
    {
        if (typeof lang === "string")
        {
            this.lang = Language.LANGUAGES[lang];
        } else if (typeof lang === "object" && lang instanceof Language) {
            this.lang = lang;
        } else {
            throw new Error(`Lang |${lang}| must be a recognized language or an instance of Language`);
        }
        this.discards = discards;
        this.debug = false;
    }

    getLanguage()
    {
        return this.lang;
    }

    match(start, line, word, debug=false)
    {
        let matches = [];
        let safeWord = word.replace(/\n/g, "\\n");
        for (const [type, patterns] of this.lang.getTypeDefinitions())
        {
            for (let elem of patterns)
            {
                if (elem.test(word))
                {
                    this.log(`    Match: ${type} with ${elem} for ${safeWord}`);
                    matches.push(new Match(type, elem, start, line));
                }
            }
        }
        return matches;
    }

    log(s)
    {
        if (this.debug) {
            console.log(s);
        }
    }

    lex(text, discards=null, debug=false)
    {
        this.debug = debug;
        discards = discards === null ? this.discards : discards;
        let word = '';
        let matches = [];
        let oldMatches = [];
        let tokens = [];
        let start = 0;
        let line = 1;
        let index = 0;
        while (index < text.length)
        {
            word += text[index];
            let safeWord = word.replace(/\n/g, "\\n");
            this.log(`${index}. Word is |${safeWord}|`);
            matches = this.match(start, line, word, debug);
            if (text[index] === '\n') {
                line += 1;
            }

            if (matches.length === 0) {
                this.log('    No match this turn');
                if (oldMatches === null || oldMatches.length === 0) {
                    // Nothing, we try to add the maximum
                    //throw new Error("Impossible to map the language.");
                } else {
                    // Visions: trying to see if there is something after
                    if (index + 1 < text.length)
                    {
                        let future_index = index + 1;
                        let future_word = word + text[future_index];
                        matches = this.match(start, line, future_word, debug);
                        if (debug && matches.length > 0)
                        {
                            console.log('    Vision of the future OK');
                        }
                    }
                    // Si et seulement si dans le futur on n'aura rien on fait un jeton, sinon on continue
                    if (matches.length === 0)
                    {
                        let content =  word.substring(0, word.length-1);
                        this.log(`pour le mot |${content}| nous avons : ${oldMatches.map(x => x.toString()).join("\n")}`);
                        if (this.lang.isWrong(oldMatches[0].type))
                        {
                            throw new Error(`A wrong token definition ${oldMatches[0].type} : ${oldMatches[0].elem} has been validated by the lexer: ${content}`);
                        }
                        if (!discards.includes(oldMatches[0].type))
                        {
                            tokens.push(new Token(oldMatches[0].type, content, oldMatches[0].start, oldMatches[0].line));
                        }
                        word = '';
                        index -= 1;
                        start = oldMatches[0].start + content.length;
                    }
                }
            }
            oldMatches = matches;
            index += 1;
        }
        if (oldMatches !== null && oldMatches.length > 0)
        {
            let content =  word;
            this.log(`pour le mot |${content}| nous avons : ${oldMatches.map(x => x.toString()).join("\n")}`);
            if (this.lang.isWrong(oldMatches[0].type))
            {
                throw new Error(`A wrong token definition ${oldMatches[0].type} : ${oldMatches[0].elem} has been validated by the lexer: ${content}`);
            }
            if (!discards.includes(oldMatches[0].type))
            {
                tokens.push(new Token(oldMatches[0].type, content, oldMatches[0].start, oldMatches[0].line));
            }
        } else if (word.length > 0)
        {
            console.log(tokens);
            console.log(word.charCodeAt(0));
            throw new Error(`Text not lexed at the end: |${word}|`);
        }
        return tokens;
    }

    toHTML(tokens, raws=[], debug=false)
    {
        let html = tokens.map(tok => tok.toHTML(this.lang.getName(), raws.includes(tok.getType()), debug));
        return html.join('');
    }
}

//-------------------------------------------------------------------------------
// Globals and constants
//-------------------------------------------------------------------------------

// Shared definitions
const PATTERNS = {
    'IDENTIFIER'    : ['[a-zA-Z]\\w*'],
    'INTEGER'       : ['\\d+'],
    'INTEGER_HEXA'  : ['0[xX][\\dABCDEFabcdef]+'],
    'INTEGER_BIN'   : ['0[bB][01]+'],
    'WRONG_INTEGER' : ['\\d+\\w+'],
    'FLOAT'         : ['\\d+\\.\\d+', '\\d+[eE]-\\d+', '\\d+\\.\\d+[eE]-?\\d+'],
    'WRONG_FLOAT'   : ['\\d+\\.'],
    'BLANKS'        : ['[ \u00A0\\t]+'],
    'NEWLINES'      : ['\n', '\n\r', '\r\n'],
    'OPERATORS'     : ['==', '=', '\\.'],
    'STRINGS'       : ["'([^\\\\']|\\\\['nt])*'", '"([^\\\\"]|\\\\["nt])*"'],
    'SEPARATORS'    : ['\\(', '\\)']
};

const SHORTCUTS = {
    'keyword': 'kw',
    'special': 'spe',
    'identifier': 'id',
    'affectation': 'aff',
    'combined_affectation': 'aff',
    'separator': 'sep',
    'operator': 'op',
    'comment': 'com',
    // Types
    'boolean': 'bool',
    'integer': 'int',
    'number': 'num',
    'float': 'flt',
    'string': 'str',
}

//-------------------------------------------------------------------------------
// Tests
//-------------------------------------------------------------------------------

class Test
{
    constructor(language, text, result, discards=[])
    {
        this.language = language;
        this.text = text;
        this.result = result;
        if (this.result === null || this.result === undefined)
        {
            throw new Error(`No expected results for test ${text}`);
        }
        this.discards = discards;
    }

    getDiscards()
    {
        return this.discards;
    }

    getLanguage()
    {
        return this.language;
    }

    test(lexer, num=0, debug=false)
    {
        let tokens = lexer.lex(this.text, null, debug);
        if (tokens.length !== this.result.length)
        {
            console.log('Difference of length, dumping:')
            let longuest = Math.max(tokens.length, this.result.length);
            for (let index = 0; index < longuest; index++)
            {
                if (index < tokens.length && index < this.result.length)
                {
                    let cmp = (this.result[index] === tokens[index].getType());
                    console.log(`${index}. ${cmp} Expected=${this.result[index]} vs ${tokens[index].getType()} (${tokens[index].getValue()})`);
                } else if (index < tokens.length) {
                    console.log(`${index}. Expected=null [null] vs ${tokens[index].getType()}`, tokens[index].getValue());
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
        console.log(`[SUCCESS] Test n°${num} language : |${lexer.getLanguage().getName()}|`);
        if (this.discards.length > 0) {
            console.log(`Discarded: ${this.discards.join(', ')}`);
        }
        let val = this.text.replace(/\n/g, '\\n');
        console.log(`Text :\n    |${val}|`);
        console.log(`Result:`);
        tokens.forEach(tok => { console.log(`    ${tok.toString(12)}`);});
        console.log('\n');
    }
}

const TESTS = [

    //---------------------------------------------------------------
    // Language: Line
    //---------------------------------------------------------------

    new Test(
        'line',
        "bonjour\ntoi qui\nvient de loin",
        ['line', 'line', 'line']
    ),
    new Test(
        'line',
        "Et où commence la beauté du jour?\nSi ce n'est à l'aube et bien après minuit\nAnonyme.",
        ['line', 'line', 'line']
    ),

    //---------------------------------------------------------------
    // Language: Ash
    //---------------------------------------------------------------

    new Test(
        'ash',
        "a = 1.2",
        ['identifier', 'affectation', 'float'],
        ['blank']
    ),
    new Test(
        'ash',
        "a = 1.to_f",
        ['identifier', 'affectation', 'integer', 'operator', 'identifier'],
        ['blank']
    ),
    new Test(
        'ash',
        "const a = 20 + 5 ; 'hello'",
        ['keyword', 'blank', 'identifier', 'blank', 'affectation', 'blank', 'integer', 'blank', 'operator', 'blank',
        'integer', 'blank', 'separator', 'blank', 'string']
    ),
    new Test(
        'ash',
        'a = 5',
        ['identifier', 'blank', 'affectation', 'blank', 'integer']
    ),
    new Test(
        'ash',
        "a ** 5",
        ['identifier', 'blank', 'operator', 'blank', 'integer']
    ),
    //new Test(
    //    'ash', "if a == 5 then\nprintln('hello')\nend\nendly = 5\na = 2.5\nb = 0xAE\nc = 2.5.to_i()\nd = 2.to_s()\n"
    //),
    new Test(
        'ash',
        'writeln("hello")',
        ['special', 'separator', 'string', 'separator'],
        ['blank']
    ),
    new Test(
        'ash',
        'if a == 5 then\n    writeln("hello")\nend',
        [
            'keyword', 'identifier', 'operator', 'integer', 'keyword', 'newline',
            'special', 'separator', 'string', 'separator', 'newline', 'keyword'
        ],
        ['blank']
    ),

    //---------------------------------------------------------------
    // Language: BNF et BNF-mini
    //---------------------------------------------------------------

    new Test(
        'bnf',
        "<rule 1> ::= 'terminal1' 'terminal2'",
        ['non-terminal', 'operator', 'terminal', 'terminal'],
        ['blank']
    ),
    new Test(
        'bnf-mini',
        "<rule xtrem> ::= 'terminal xtrem'",
        ['non-terminal', 'operator', 'terminal'],
        ['blank']
    ),

    //---------------------------------------------------------------
    // Language: Lua
    //---------------------------------------------------------------

    new Test('lua', '3+5', ['number', 'operator', 'number']),
    new Test('lua', 'a = 5', ['identifier', 'blank', 'operator', 'blank', 'number']),
    new Test('lua', 'a = 5', ['identifier', 'operator', 'number'], ['blank']),
    new Test('lua', '-- Ceci est un commentaire\nabc', ['comment', 'identifier']),
    new Test('lua', '--[[Ceci est un\ncommentaire multiligne--]]', ['comment']),
    new Test(
        'lua',
        't = { ["k1"] = 5 }',
        [
            'identifier', 'operator', 'separator', 'separator', 'string', 'separator', 'operator', 'number', 'separator'
        ],
        ['blank']
    ),
    new Test(
        'lua',
        't = { ["k1"] = 5, ["k2"] = "v", [4] = 6 } -- Définition\nprint(t["k1"]) -- Accès\nprint(t.k1) -- Accès avec sucre syntaxique',
        [
            'identifier', 'operator', 'separator', 'separator', 'string', 'separator', 'operator', 'number',
            'separator', 'separator', 'string', 'separator', 'operator', 'string', 'separator', 'separator', 'number',
            'separator', 'operator', 'number', 'separator', 'comment', 'special', 'separator', 'identifier',
            'separator', 'string', 'separator', 'separator', 'comment', 'special', 'separator', 'identifier',
            'operator', 'identifier', 'separator', 'comment'
        ],
        ['blank']
    ),

    //---------------------------------------------------------------
    // Language: Python
    //---------------------------------------------------------------

    new Test(
        'python',
        "def a():\n\tif a == 5:\n\t\tprint('hello')",
        [
            'keyword', 'blank', 'identifier', 'separator', 'separator', 'operator', 'newline',
            'blank', 'keyword', 'blank', 'identifier', 'blank', 'operator', 'blank', 'integer', 'operator', 'newline',
            'blank', 'special', 'separator', 'string', 'separator'
        ]
    ),

    //---------------------------------------------------------------
    // Language: Game
    //---------------------------------------------------------------

    new Test(
        'game',
        "Baldur's Gate (1998), Far Cry: Blood Dragon",
        ['game', 'blank', 'year', 'separator', 'blank', 'game']
    ),
    new Test(
        'game',
        "Baldur's Gate (1998), Far Cry: Blood Dragon",
        ['game', 'year', 'separator', 'game'],
        ['blank']
    ),
    new Test(
        'game',
        "Baldur's Gate\nTotal Annihilation\nHalf-Life\nFar Cry: Blood Dragon",
        ['game', 'newline', 'game', 'newline', 'game', 'newline', 'game']
    ),

    //---------------------------------------------------------------
    // Language: FR
    //---------------------------------------------------------------

    new Test(
        'fr',
        "On n'habite pas un pays, on habite une langue. Une patrie, c'est cela et rien d'autre.",
        ['word', 'word', 'punct', 'word', 'word', 'word', 'word', 'punct', 'word', 'word', 'word', 'word', 'punct',
        // On     n       '        habite pas     un      pays     ,        on      habite une     langue   .
         'word', 'word', 'punct', 'word', 'punct', 'word', 'word', 'word', 'word', 'word', 'punct', 'word', 'punct'],
        // Une   patrie  ,        c        '        est     cela    et     rien     d       '        autre   .
        ['blank']
    ),
    new Test('fr', "bonjour l'ami !", ['word', 'word', 'punct', 'word', 'punct'], ['blank']),

    //---------------------------------------------------------------
    // Language: Test
    //---------------------------------------------------------------

    new Test(
        'text',
        "je suis là",
        ['normal', 'blank', 'normal', 'blank', 'normal']
    ),

    //---------------------------------------------------------------
    // Language: JSON
    //---------------------------------------------------------------

    new Test(
        'json',
        "{'alpharius': 20, 'heretic': true}",
        ['separator', 'string', 'separator', 'number', 'separator', 'string', 'separator', 'boolean', 'separator'],
        ['blank']),

];

function testTokens() {
    let tok = new Token('identifier', 'a', 0, 1);
    console.log(tok.toString());
    console.log(tok.toString(10));
}

function tests(debug=false)
{
    let index = 1;
    for (const test of TESTS)
    {
        let lexer = new Lexer(Language.LANGUAGES[test.getLanguage()], test.getDiscards());
        test.test(lexer, index, debug);
        index += 1;
    }
    console.log("\n--- Test of to_html ------------------------------------------\n");
    let lexer = new Lexer(Language.LANGUAGES['lua']);
    let text = "if a >= 5 then println('hello') end";
    let tokens = lexer.lex(text);
    let expectedNormal = "<span class=lua-keyword>if</span> <span class=lua-identifier>a</span> <span class=lua-operator>&gt;=</span> <span class=lua-number>5</span> <span class=lua-keyword>then</span> <span class=lua-identifier>println</span><span class=lua-separator>(</span><span class=lua-string>'hello'</span><span class=lua-separator>)</span> <span class=lua-keyword>end</span>";
    let expectedDebug = "if<sub>keyword</sub> a<sub>identifier</sub> &gt;=<sub>operator</sub> 5<sub>number</sub> then<sub>keyword</sub> println<sub>identifier</sub>(<sub>separator</sub>'hello'<sub>string</sub>)<sub>separator</sub> end<sub>keyword</sub>";
    let resNormal = lexer.toHTML(tokens, ['blank']);
    let resDebug = lexer.toHTML(tokens, ['blank'], true);
    if (resNormal !== expectedNormal) {
        console.log('[ERROR]');
        console.log(text);
        console.log(`Expected: ${expectedNormal}`);
        console.log(`Got: ${resNormal}`);
    } else {
        console.log('[SUCESS] expectedNormal');
    }
    if (resDebug !== expectedDebug) {
        console.log('[ERROR]');
        console.log(text);
        console.log(`Expected: ${expectedDebug}`);
        console.log(`Got: ${resDebug}`);
    } else {
        console.log('[SUCESS] expectedDebug');
    }
}

if (node && main) {
    console.log('Tests tokens');
    testTokens();
    console.log('Read definitions');
    console.log(Language.readDefinition());
    console.log('Test lexer');
    tests(false);
}

export {Token, Language, Lexer};
