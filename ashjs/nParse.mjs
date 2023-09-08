
//-------------------------------------------------------------------------------
// Import
//-------------------------------------------------------------------------------

import { Token, Lexer, Language } from "./lexer.mjs";

class Node extends Token {
    constructor(type, value = null, start = null, line = null, left = null, right = null) {
        super(type, value, start, line);
        this.left = left;
        this.right = right;
    }

    isLeaf() {
        return this.left === null && this.right === null;
    }

    toString(level = 0, right = null) {
        let content = '';
        if (['Affectation', 'Identifier', 'Integer', 'Float', 'Boolean', 'BinaryOp'].includes(this.type)) {
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

let precedence = [
    ['=', '+=', '-=', '*=', '/=', '//=', '%='],
    ['and', 'or'],
    ['<', '<=', '>=', '>', '==', '!='],
    ['<<', '>>'],
    ['+', '-'],
    ['*', '/', '//', '%'],
    ['**'], // /!\ Right to left associative
];

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

    parseExpression() {
        this.level += 1;
        this.log(`>>> parseExpression at ${this.index}`);
        let res = this.parseBinaryOp();
        this.level -= 1;
        return res;
    }

    parseBinaryOp(opLevel=0) {
        if (opLevel === precedence.length) {
            return this.parseLiteral();
        }
        this.level += 1;
        let name = precedence[opLevel].map(x => this.uFirst(x)).join(", ");
        let right = null;
        let node = null;
        this.log(`>>> parsing ${name} (operator ${opLevel+1}/${precedence.length}) at ${this.index}`);
        node = this.parseBinaryOp(opLevel + 1);
        // On peut faire un while ici pour traiter les suites en chaînant avec expr = new Expression(expr, operator, right);
        if (this.test('operator', precedence[opLevel])) {
            let op = this.advance();
            right = this.parseBinaryOp(opLevel);
            if (right === null) {
                throw new Error("No expression on the right of a binary operator at " + current.getLine());
            }
            node = new Node('BinaryOp', op.getValue(), op.getStart(), op.getLine(), node, right);
        }
        this.level -= 1;
        return node;
    }

    parseLiteral() {
        this.level += 1;
        this.log(`>>> parseLiteral at ${this.index}`);
        let current = this.read();
        if (current !== null && ['identifier', 'integer', 'float', 'boolean'].includes(current.type)) {
            this.log('Reading: ' + current);
            this.advance();
            this.level -= 1;
            return new Node(this.uFirst(current.getType()), current.getValue(), current.getStart(), current.getLine());
        }
        this.level -= 1;
        return null;
    }

    read(type = null, value = null) {
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

    test(type = null, value = null) {
        const token = (this.index < this.tokens.length) ? this.tokens[this.index] : null;
        return token?.equals(type, value);
    }

    advance() {
        let current = this.index < this.tokens.length ? this.tokens[this.index] : null
        this.index += 1;
        return current;
    }

    abort() {
        this.tokens.slice(this.index).forEach(_ => `${this.index}. Unhandled token ${this.read()}`);
    }
}

// tokens.slice(start).find(token => token.value === target)

let scope = {};

function miniExec(node, level=0) {
    if (node.type === 'Block') {
        let val = miniExec(node.left, level + 1);
        if (node.right !== null) {
            val = miniExec(node.right, level + 1);
        }
        console.log('    '.repeat(level) + `Block ${val}`);
        return val;
    } else if (node.type === 'BinaryOp') {
        if (node.value === '=') {
            let identifier =  miniExec(node.left, level+1);
            let val = miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Affectation ${identifier} ${node.value} ${val}`);
            if (val === '=') {
                scope[identifier] = val;
            }
            return val;
        } else if (node.value === '+') {
            let val = miniExec(node.left, level+1) + miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(+) ${val}`);
            return val;
        } else if (node.value === '*') {
            let val = miniExec(node.left, level+1) * miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(*) ${val}`);
            return val;
        } else if (node.value === '**') {
            let val = Math.pow(miniExec(node.left, level+1), miniExec(node.right, level+1));
            console.log('    '.repeat(level) + `Binaryop(*) ${val}`);
            return val;
        } else if (node.value === 'and') {
            let val = miniExec(node.left, level+1) && miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(and) ${val}`);
            return val;
        } else if (node.value === 'or') {
            let val = miniExec(node.left, level+1) || miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(and) ${val}`);
            return val;
        } else if (node.value === '>') {
            let val = miniExec(node.left, level+1) > miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(>) ${val}`);
            return val;
        } else {
            throw new Error(`ERROR2 : ${node}`)
        }
    } else if (node.type === 'Integer') {
        let val = parseInt(node.value);
        console.log('    '.repeat(level) + `Integer ${val}`);
        return val;
    } else if (node.type === 'Boolean') {
        let val = node.value === 'true';
        console.log('    '.repeat(level) + `Boolean ${val}`);
        return val;
    } else if (node.type === 'Identifier') {
        console.log('    '.repeat(level) + `Identifier ${node.value}`);
        return node.value;
    } else {
        throw new Error(`ERROR1 : ${node}`);
    }
}

Language.readDefinition();
function makeTree(text, expected=null) {
    let tokens = new Lexer('ash').lex(text);
    console.log('Tokens:');
    console.log(tokens.filter(x => !x.equals("blank")));
    let res = new Parser().parse(tokens, true);
    console.log(text);
    console.log('AST:');
    console.log(res.toString());
    console.log('Result:');
    let finalRes = miniExec(res);
    console.log(finalRes);
    if (expected !== null) {
        if (expected === finalRes) {
            console.log(`[SUCCESS] |${text}| got ${finalRes}`);
        } else {
            throw new Error(`[ERROR] expected ${expected} got ${finalRes}`); // ${text}
        }
    }
}

console.log("----------------------------------------------------");
console.log("----------------------------------------------------");
console.log("----------------------------------------------------");
console.log("----------------------------------------------------");
console.log("----------------------------------------------------");

makeTree("5", 5);
makeTree("2 + 3", 5);
makeTree("5 + 2 * 3", 11);
makeTree("2 * 4 + 6", 14);
makeTree("true and false", false);
makeTree("true or false", true);
makeTree("5 > 2", true);
makeTree("a = 5", 5);
makeTree("b = true or false and true", true);
makeTree("2 ** 3 ** 2", 512);

//-----------------------------------------------------------------------------
// Exports
//-----------------------------------------------------------------------------

export {Parser, Node};
