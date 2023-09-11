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

// Trucs intelligents :
// - block demande à ce qu'une expression se termine, pas parseExpression
// - on fait niv1 { left = niv2 if (tok === ce que je veux) { right = niv1 (si y a deux fois le même op)}}
//   lui il fait un while et fait right = niv2
// Il faut maintenant rettaguer les not et - en unaop
// - "not" forécment en unaop
// - "-" uniquement si il est premier ou qu'avant il y a un sep, binop ou unaop

//-------------------------------------------------------------------------------
// Environment & imports
//-------------------------------------------------------------------------------

const FILENAME = "parser.mjs";

const node =
	typeof process !== "undefined" &&
	process !== null &&
	typeof process.version !== "undefined" &&
	process.version !== null &&
	typeof process.version === "string";

const fs = node ? await import("fs") : null;
const path = node ? await import("path") : null;
const reader = node ? await import("readline-sync") : null;

const main = (node) ? path.basename(process.argv[1]) === FILENAME : false;

import { Token, Lexer, Language } from "./lexer.mjs";

//-------------------------------------------------------------------------------
// Classes
//-------------------------------------------------------------------------------

class Node extends Token {
    constructor(type, value = null, start = null, line = null, left = null, right = null) {
        super(type, value, start, line);
        this.left = left;
        this.right = right;
    }

    isLeaf() {
        return this.left === null && this.right === null;
    }

    toString(level = 0, sigil = 'root') {
        let content = '';
        if (['Affectation', 'Identifier', 'Integer', 'Float', 'Boolean', 'BinaryOp'].includes(this.type)) {
            content = ` (${this.value})`;
        }
        let base = '    '.repeat(level) + `${level}. (${sigil}) ${this.type}${content}\n`;
        let value = '';
        let left = '';
        let right = '';
        if (this.value instanceof Node) {
            value = this.value.toString(level + 1, 'V');
        }
        if (this.left !== null) {
            left = this.left.toString(level + 1, 'L');
        }
        if (this.right !== null) {
            right = this.right.toString(level + 1, 'R');
        }
        return `${base}${value}${left}${right}`;
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
        this.log(`>>> ${this.level} START parseBlock at ${this.index}: ${this.read()}`);
        let root = null;
        let suite = null;
        this.level += 1;
        while (this.index < this.tokens.length) {
            this.log(`>>> ${this.level} LOOP parseBlock at ${this.index}: ${this.read()}`);
            let res = null;
            let current = this.read();
            // Test
            if (current.equals("newline") || current.equals("comment") || current.equals("blank")) {
                this.advance();
                continue;
            } else if (this.test("keyword", ["end", "loop"])) {
                break;
            } else if (current.equals("keyword", "if")) {
                res = this.parseIf();
            } else if (current.equals("keyword", "while")) {
                res = this.parseWhile();
            } else if (current.equals("keyword", "for")) {
                res = this.parseFor();
            } else if (current.equals("keyword", ["break", "next"])) {
                res = new Node(this.uFirst(current.getType()), null, current.getStart(), current.getLine());
                this.advance();
            } else if (current.equals("keyword", "import")) {
                res = this.parseImport();
            } else if (current.equals("keyword", ["function", "procedure"])) {
                res = this.parseSubProgram();
            } else if (this.test("separator", ["\n", ";"])) {
                this.advance();
            } else {
                res = this.parseExpression();
                this.log("HELLO " + this.index + " / " + this.tokens.length);
                if (this.index < this.tokens.length) {
                    if (this.test("separator", [";", ",", ")"]) || this.test("newline", "\n")) {
                        this.advance();
                    } else if (!this.test("keyword", ["end", "loop"])) {
                        throw new Error(`Unfinished Expression at ${this.tokens[this.index]} after ${this.tokens[this.index-1]}`);
                    }
                }
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
        this.log(`>>> ${this.level} parseExpression at ${this.index}`);
        let res = this.parseBinaryOp();
        this.level -= 1;
        return res;
    }

    parseBinaryOp(opLevel=0) {
        if (opLevel === precedence.length) {
            return this.parseCall();
        }
        this.level += 1;
        let name = precedence[opLevel].map(x => this.uFirst(x)).join(", ");
        let node = null;
        let right = null;
        this.log(`>>> ${this.level} START ${name} (operator ${opLevel+1}/${precedence.length}) at ${this.index}`);
        node = this.parseBinaryOp(opLevel + 1);
        // On peut faire un while ici pour traiter les suites en chaînant avec expr = new Expression(expr, operator, right);
        if (this.test('operator', precedence[opLevel])) {
            let op = this.advance();
            this.log(`>>> ${this.level} PARSING ${name} (operator ${opLevel+1}/${precedence.length}) at ${this.index}`);
            right = this.parseBinaryOp(opLevel);
            if (right === null) {
                throw new Error("No expression on the right of a binary operator at " + current.getLine());
            }
            node = new Node('BinaryOp', op.getValue(), op.getStart(), op.getLine(), node, right);
        }
        this.level -= 1;
        return node;
    }

    parseCall() {
        this.level += 1;
        let node = this.parseLiteral();
        if (this.test('separator', '(')) {
            this.log(`>>> ${this.level} PARSING Call at ${this.index}`);
            this.advance();
            let par1 = null;
            if (!this.test('separator', ')')) {
                par1 = this.parseExpression();
            }
            if (!this.test('separator', ')')) {
                throw new Error("Unclosed parenthesis");
            }
            node = new Node('Call', node.getValue(), node.getStart(), node.getLine(), node, par1);
        }
        this.level -= 1;
        return node;
    }

    parseLiteral() {
        this.level += 1;
        if (this.test('separator', '(')) {
            this.advance();
            let node = this.parseExpression();
            if (!this.test('separator', ')')) {
                throw new Error("Unclosed (");
            }
            this.advance();
            return node;
        }
        //this.log(`>>> parseLiteral at ${this.index}`);
        let current = this.read();
        if (current !== null && ['identifier', 'integer', 'float', 'boolean', 'string'].includes(current.type)) {
            this.log(`>>> ${this.level} PARSING literal at ${this.index} : ${current}`);
            this.advance();
            this.level -= 1;
            return new Node(this.uFirst(current.getType()), current.getValue(), current.getStart(), current.getLine());
        }
        this.level -= 1;
        return null;
    }

    parseWhile() {
        this.level += 1;
        this.log(`>>> ${this.level} PARSING While at ${this.index}`);
        let whiletoken = this.read('keyword', 'while');
        this.advance();
        let condition = this.parseExpression();
        this.read('keyword', 'do');
        this.advance();
        let action = this.parseBlock();
        this.read('keyword', 'loop');
        this.advance();
        this.level -= 1;
        return new Node('While', condition, whiletoken.getStart(), whiletoken.getLine(), action, null);
    }

    parseIf() {
        this.level += 1;
        this.log(`>>> ${this.level} PARSING If at ${this.index}`);
        let iftoken = this.read('keyword', 'if');
        this.advance();
        let condition = this.parseExpression();
        this.read('keyword', 'then');
        this.advance();
        let action = this.parseBlock();
        let actionElse = null;
        if (this.test('keyword', 'else')) {
            this.advance();
            actionElse = this.parseBlock();
        }
        this.read('keyword', 'end');
        this.advance();
        this.level -= 1;
        return new Node('If', condition, iftoken.getStart(), iftoken.getLine(), action, actionElse);
    }

    read(type = null, value = null) {
        const token = (this.index < this.tokens.length) ? this.tokens[this.index] : null;
        // Token check on value and/or type
        if (value !== null) {
            if (token !== null && !token.equals(type, value)) {
                throw new Error(`Parse error: expected |${type},${value}| and got ${token}`);
            } else if (token === null) {
                throw new Error(`Parse error: expected |${type},${value}| and got nothing.`);
            }
        }
        // Length check
        if (this.index >= this.tokens.length) {
            throw new Error(`Parse error: unexpected end of stream of tokens, expected |${type},${value}| @${this.index}.`);
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

class NilClass {
	toString() {
		return "nil";
	}
}
const nil = new NilClass();

function miniExec(node, level=0, evalId=true) {
    if (node.type === 'Block') {
        let val = miniExec(node.left, level + 1);
        if (node.right !== null) {
            val = miniExec(node.right, level + 1);
        }
        console.log('    '.repeat(level) + `Block ${val}`);
        return val;
    } else if (node.type === 'Call') {
        let idFun = miniExec(node.left, level + 1, false);
        if (idFun === 'log') {
            console.log(miniExec(node.right, level + 1));
            return nil;
        } else {
            throw new Error(`Unknown function ${idFun}`)
        }
    } else if (node.type === 'While') {
        let cond = miniExec(node.value, level + 1);
        while (cond) {
            miniExec(node.left, level + 1);
            cond = miniExec(node.value, level + 1);
        }
        return nil;
    } else if (node.type === 'If') {
        let cond = miniExec(node.value, level + 1);
        if (cond) {
            miniExec(node.left, level + 1);
        } else if (node.right !== null) {
            miniExec(node.right, level + 1);
        }
        return nil;
    } else if (node.type === 'BinaryOp') {
        if (['=', '-='].includes(node.value)) {
            // Left side
            let identifier =  miniExec(node.left, level+1, false);
            let val = miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Affectation ${identifier} ${node.value} ${val}`);
            if (node.value === '=') {
                scope[identifier] = val;
            } else if (node.value === '-=') {
                scope[identifier] -= val;
            }
            return val;
        } else if (node.value === '+') {
            let val = miniExec(node.left, level+1) + miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(+) ${val}`);
            return val;
        } else if (node.value === '-') {
            let val = miniExec(node.left, level+1) - miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(-) ${val}`);
            return val;
        } else if (node.value === '*') {
            let val = miniExec(node.left, level+1) * miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(*) ${val}`);
            return val;
        } else if (node.value === '/') {
            let val = miniExec(node.left, level+1) / miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(/) ${val}`);
            return val;
        } else if (node.value === '**') {
            let val = Math.pow(miniExec(node.left, level+1), miniExec(node.right, level+1));
            console.log('    '.repeat(level) + `Binaryop(*) ${val}`);
            return val;
        } else if (node.value === '//') {
            let val = Math.floor(miniExec(node.left, level+1) / miniExec(node.right, level+1));
            console.log('    '.repeat(level) + `Binaryop(//) ${val}`);
            return val;
        } else if (node.value === 'and') {
            let val = miniExec(node.left, level+1) && miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(and) ${val}`);
            return val;
        } else if (node.value === 'or') {
            let val = miniExec(node.left, level+1) || miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(and) ${val}`);
            return val;
        } else if (node.value === '==') {
            console.log('ZZZ', miniExec(node.left, level+1), miniExec(node.right, level+1))
            let val = miniExec(node.left, level+1) === miniExec(node.right, level+1);
            console.log('    '.repeat(level) + `Binaryop(==) ${val}`);
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
    } else if (node.type === 'Float') {
        let val = parseFloat(node.value);
        console.log('    '.repeat(level) + `Float ${val}`)
        return val;
    } else if (node.type === 'Boolean') {
        let val = node.value === 'true';
        console.log('    '.repeat(level) + `Boolean ${val}`);
        return val;
    } else if (node.type === 'Identifier') {
        if (evalId) {
            if (!(node.value in scope)) {
                console.log('ERROR Scope = ');
                console.log(scope);
                throw new Error(`Unknown identifier=${node.value} in current scope`);
            }
            console.log('    '.repeat(level) + `Identifier ${node.value} as value  ${scope[node.value]}`);
            return scope[node.value];
        } else {
            console.log('    '.repeat(level) + `Identifier ${node.value} as identifier`);
            return node.value;
        }
    } else if (node.type === 'String') {
        console.log('    '.repeat(level) + `String ${node.value}`);
        return node.value.slice(1, node.value.length - 1);
    } else {
        throw new Error(`ERROR1 : ${node}`);
    }
}

Language.readDefinition();

function makeTree(text) {
    let tokens = new Lexer('ash').lex(text);
    console.log('Tokens:');
    let filtered = tokens.filter(x => !x.equals("blank"));
    for (let i = 0; i < filtered.length; i++) {
        console.log(`    ${i}. ${filtered[i]}`);
    }
    let res = new Parser().parse(tokens, true);
    console.log(text);
    console.log('AST:');
    console.log(res.toString());
    console.log('Result:');
    let finalRes = miniExec(res);
    console.log(finalRes);
    return finalRes;
}

//-------------------------------------------------------------------------------
// Tests
//-------------------------------------------------------------------------------

function testsMain(debug) {
    console.log("----------------------------------------------------");
    console.log("Running tests");
    console.log("----------------------------------------------------");

    const tests = [
        ["5", 5],
        ["2 + 3", 5],
        ["5 + 2 * 3", 11],
        ["2 * 4 + 6", 14],
        ["2 + 3 * 5", 17],
        ["true and false", false],
        ["true or false", true],
        ["5 > 2", true],
        ["a = 5", 5],
        ["b = true or false and true", true],
        ["2 ** 3 ** 2", 512],
        ["(5 + 2) * 3", 21],
        ["5 * (2 + 4)", 30],
        ["(2 + 3) * 5", 25],
        ["2 - 5", -3],
        ["9 / 2", 4.5],
        ["9 // 2", 4],
        ["4.2", 4.2],
        ['"abc" + "def"', "abcdef"],
        ["2 + 5 ; 4 + 8", 12]
    ];

    for (let i = 0; i < tests.length; i++) {
        let cmd = tests[i][0];
        let expected = tests[i][1];
        if (debug) {
            console.log("===================================");
            console.log(`${i}: ${cmd}. Expected: ${expected}`);
            console.log("===================================");
            let result = makeTree(cmd);
            if (expected === result) {
                console.log(`[SUCCESS] ${i}. cmd=|${cmd}| got ${result}`);
            } else {
                throw new Error(`[ERROR] ${i}. cmd=|${cmd}| expected ${expected} got ${result}`);
            }
        }
    }
}

//-------------------------------------------------------------------------------
// Main
//-------------------------------------------------------------------------------

function nodeMain(debug = true) {
    console.log(`Running nodeMain of ${FILENAME}`);
    console.log(`Parameters (${process.argv.length}):`);
    process.argv.forEach(x => console.log('    ' + x));
    // 1: node.exe
    // 2: filename.mjs
    if (process.argv.length === 3 && process.argv[2] === 'tests') {
        testsMain(debug);
    } else if (process.argv.length === 3 && fs.existsSync(process.argv[2])) {
        let filename = process.argv[2];
        console.log(`Running file ${filename}`);
        let data = fs.readFileSync(filename, "utf-8");
        data = data.replace(/\r\n/g, "\n").replace(/\n\r/g, "\n");
        if (debug) {
			console.log(`Data read from file: ${filename}`);
		}
        let res = makeTree(data);
        console.log("Final Res: " + res);
    } else if (process.argv.length > 4) {
        throw new Error(
			`Too many parameters: ${process.argv.length}. The maximum is 4.`
		);
    } else {
        console.log('Running REPL:');
        let cmd = "";
        while (cmd !== "exit") {
            cmd = reader.question(">>> ");
            if (cmd !== "exit") {
                let result = makeTree(cmd);
                if (result !== null) { // notAnExpression
                    console.log(result);
                }
            }
        }
    }
}

if (node && main) {
	nodeMain();
}

//-----------------------------------------------------------------------------
// Exports
//-----------------------------------------------------------------------------

export {Parser, Node};
