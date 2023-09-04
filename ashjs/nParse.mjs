
//-------------------------------------------------------------------------------
// Import
//-------------------------------------------------------------------------------

import { Token, Lexer, Language } from "./lexer.mjs";

class Node extends Token {
    constructor(type, start=null, line=null) {
        super(type, null, start, line);
        this.left = null;
        this.right = null;
    }

    isLeaf() {
        return this.left === null && this.right === null;
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
        this.tokens = tokens;
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
                res = new Node(this.uFirst(current.getType()), current.getStart(), current.getLine());
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
        return new Node("Expression");
    }

    read() {
        const token = (this.index < this.tokens.length) ? this.tokens[this.index] : null;
        this.index += 1;
        return token;
    }

    abort() {
        this.tokens.slice(this.index).forEach(_ => `${this.index}. Unhandled token ${this.read()}`);
    }
}

// tokens.slice(start).find(token => token.value === target)

Language.readDefinition();
let tokens = new Lexer('ash').lex("a + 2");
console.log(tokens);
let res = new Parser().parse(tokens);
console.log(res);

