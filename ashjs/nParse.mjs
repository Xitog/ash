
//-------------------------------------------------------------------------------
// Import
//-------------------------------------------------------------------------------

import { Token, Lexer, Language } from "./lexer.mjs";

class Node extends Token {
    constructor(type, value=null, start=null, line=null, left=null, right=null) {
        super(type, value, start, line);
        this.left = left;
        this.right = right;
    }

    isLeaf() {
        return this.left === null && this.right === null;
    }

    toString(level = 0, right = null) {
        let content = '';
        if (['Affectation', 'Identifier', 'Integer', 'Float', 'BinaryOp'].includes(this.type)) {
            content = ` (${this.value})`;
        }
        let sigil = 'L';
        if (right) {
            sigil = 'R';
        }
        if (this.left !== null && this.right !== null) {
            return '    '.repeat(level) + `${level}. (${sigil}) ${this.type}${content}\n` + this.left.toString(level + 1, false) + this.right.toString(level + 1, true);
        } else if (this.left !== null) {
            return '    '.repeat(level) + `${level}. (${sigil}) ${this.type}${content}\n` + this.left.toString(level + 1, false);
        } else if (this.right !== null) {
            return '    '.repeat(level) + `${level}. (${sigil}) ${this.type}${content}\n` + this.right.toString(level + 1, true);
        } else {
            return '    '.repeat(level) + `${level}. (${sigil}) ${this.type}${content}\n`;
        }
    }
}

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------

class Parser {
    constructor() {
        this.debug = false;
        this.index = 0;
        this.tokens = [];
        this.level = 0;
        this.root = null;
    }

    log(s) {
        if (this.debug) {
            console.log('    '.repeat(this.level) + s);
        }
    }

    uFirst(s) {
        return s.charAt(0).toUpperCase() + s.slice(1);
    }

    parse(tokens, debug = false) {
        this.debug = debug;
        this.index = 0;
        this.tokens = tokens.filter(x => !x.equals("blank"));
        this.level = 0;
        this.root = this.parseBlock();
        if (this.index < this.tokens.length) {
            this.abort();
        }
        return this.root;
    }

    parseBlock() {
        // if, while, for, break, next, expression, affectation, comment, newline
        this.log(`>>> parseBlock at ${this.index}`);
        let root = null;
        let suite = null;
        this.level += 1;
        while (this.index < this.tokens.length) {
            let res = null;
            let current = this.read();
            // Test
            if (current.equals("newline") || current.equals("comment") || current.equals("blank")) {
                continue;
            } else if (current.equals("keyword", "if")) {
                res = this.parseIf();
            } else if (current.equals("keyword", "while")) {
                res = this.parseWhile();
            } else if (current.equals("keyword", "for")) {
                res = this.parseWhile();
            } else if (current.equals("keyword", ["break", "next"])) {
                res = new Node(this.uFirst(current.getType()), null, current.getStart(), current.getLine());
                this.advance();
            } else if (current.equals("keyword", "import")) {
                res = this.parseImport();
            } else if (current.equals("keyword", ["function", "procedure"])) {
                res = this.parseSubProgram();
            } else {
                res = this.parseExpression();
            }
            // Checking
            if (res === null || res === undefined) {
                throw new Error("Something went wrong in parsing. Aborting.");
            }
            // Chaining
            if (suite === null) {
                suite = new Node("Block");
                root = suite;
            } else {
                suite.right = new Node("Block");
                suite = suite.right;
            }
            suite.left = res;
        }
        this.level -= 1;
        return root;
    }

    parseExpression()
    {
        this.level += 1;
        this.log(`>>> parseExpression at ${this.index}`);
        let res = this.parseAffectation();
        this.level -= 1;
        return res;
    }

    parseAffectation()
    {
        this.level += 1;
        this.log(`>>> parseAffectation at ${this.index}`);
        let left = this.parseAddSub();
        if (left === null) {
            return null;
        }
        let current = this.read();
        if (current?.equals("affectation")) {
            this.advance();
            let right = this.parseExpression();
            if (right === null) {
                throw new Error("No expression on the right of an affectation at " + current.getLine());
            }
            this.level -= 1;
            return new Node('Affectation', current.getValue(), current.getStart(), current.getLine(), left, right);
        }
        this.level -= 1;
        return left;
    }

    parseAddSub()
    {
        this.level += 1;
        this.log(`>>> parseAffectation at ${this.index}`);
        let left = this.parseMulDivMod();
        let current = this.read();
        // On peut faire un while ici pour traiter les suites en chaînant avec expr = new Expression(expr, operator, right);
        if (current?.equals("operator", ["+", "-"])) {
            this.advance();
            let right = this.parseExpression();
            if (right === null) {
                throw new Error("No expression on the right of a binary operator at " + current.getLine());
            }
            this.level -= 1;
            return new Node('BinaryOp', current.getValue(), current.getStart(), current.getLine(), left, right);
        } else {
            this.level -= 1;
            return left;
        }
    }

    parseMulDivMod()
    {
        this.level += 1;
        this.log(`>>> parseMulDivDivIntMod at ${this.index}`);
        let left = this.parseLiteral();
        let current = this.read();
        if (current?.equals("operator", ["*", "/", "//", "%"])) {
            this.advance();
            let right = this.parseExpression();
            if (right === null) {
                throw new Error("No expression on the right of a binary operator at " + current.getLine());
            }
            this.level -= 1;
            return new Node('BinaryOp', current.getValue(), current.getStart(), current.getLine(), left, right);
        } else {
            this.level -= 1;
            return left;
        }
    }

    parseLiteral()
    {
        this.level += 1;
        this.log(`>>> parseLiteral at ${this.index}`);
        let current = this.read();
        if (current !== null && ['identifier', 'integer', 'float'].includes(current.type)) {
            this.advance();
            this.level -= 1;
            return new Node(this.uFirst(current.getType()), current.getValue(), current.getStart(), current.getLine());
        }
        this.level -= 1;
        return null;
    }

    read(type=null, value=null) {
        const token = (this.index < this.tokens.length) ? this.tokens[this.index] : null;
        if (value !== null) {
            if (token !== null && !token.equals(type, value)) {
                throw new Error(`Expected |${type},${value}| and got ${token}`);
            } else if (token === null) {
                throw new Error(`Expected |${type},${value}| and got nothing.`);
            }
        }
        return token;
    }

    next(type=null, value=null) {
        this.index += 1;
        let token = this.read(type, value);
        this.index -= 1;
        return token;
    }

    advance() {
        this.index += 1;
    }

    abort() {
        this.tokens.slice(this.index).forEach(_ => `${this.index}. Unhandled token ${this.read()}`);
    }
}

// tokens.slice(start).find(token => token.value === target)

Language.readDefinition();
let tokens = new Lexer('ash').lex("a = 5 + 2 * 3");
console.log('Tokens:');
console.log(tokens.filter(x=> !x.equals("blank")));
let res = new Parser().parse(tokens, true);
console.log('AST:');
console.log(res.toString());
