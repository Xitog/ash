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

const FILENAME = "interpreter.mjs";

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

import { Lexer, Language } from "./lexer.mjs";
import { Parser } from "./parser.mjs"

//-----------------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------------

let GlobalInterpreter = null;

class NilClass {
	toString() {
		return "nil";
	}
}
const nil = new NilClass();

class Interpreter
{
    constructor(output_function=null, output_screen=null)
    {
        this.root = {};
        this.output_function = output_function == null ? console.log : output_function;
        this.output_screen = output_screen;
        this.scope = {};
    }

    do(node, level=0, evalId=true) {
        if (node.type === 'Block') {
            let val = this.do(node.left, level + 1);
            if (node.right !== null) {
                val = this.do(node.right, level + 1);
            }
            console.log('    '.repeat(level) + `Block ${val}`);
            return val;
        } else if (node.type === 'Import') {
            console.log('Importing: ' + this.do(node.left, level + 1, false));
            return nil;
        } else if (node.type === 'Call') {
            let idFun = this.do(node.left, level + 1, false);
            let arg = null;
            if (node.right !== null) {
                arg = this.do(node.right, level + 1);
            }
            return this.library(idFun, arg);
        } else if (node.type === 'While') {
            let cond = this.do(node.value, level + 1);
            while (cond === true) {
                this.do(node.left, level + 1);
                cond = this.do(node.value, level + 1);
            }
            return nil;
        } else if (node.type === 'If') {
            let cond = this.do(node.value, level + 1);
            if (cond === true) {
                this.do(node.left, level + 1);
            } else if (node.right !== null) {
                this.do(node.right, level + 1);
            }
            return nil;
        } else if (node.type === 'UnaryOp') {
            if (node.value === '-') {
                let val = -this.do(node.left, level+1);
                console.log('    '.repeat(level) + `UnaryOp(${node.value}) ${val}`);
                return val;
            } else if (node.value === 'not') {
                let val = !this.do(node.left, level+1);
                console.log('    '.repeat(level) + `UnaryOp(${node.value}) ${val}`);
                return val;
            }  else {
                throw new Error(`[ERROR] Unknown Unary Op: ${node}`)
            }
        } else if (node.type === 'BinaryOp') {
            if (['=', '+=', '-=', '*=', '/=', '//=', '**=', '%='].includes(node.value)) {
                // Left side
                let identifier =  this.do(node.left, level+1, false);
                let val = this.do(node.right, level+1);
                if (node.value !== '=' && !(identifier in this.scope)) {
                    throw new Error(`Unknown variable ${identifier} in current scope`);
                } else if (!(identifier in this.scope)) {
                    console.log('    '.repeat(level) + `Declaration ${identifier} ${node.value} ${val}`);
                } else {
                    console.log('    '.repeat(level) + `Affectation ${identifier} ${node.value} ${val}`);
                }
                if (node.value === '=') {
                    this.scope[identifier] = val;
                } else if (node.value === '+=') {
                    this.scope[identifier] += val;
                } else if (node.value === '-=') {
                    this.scope[identifier] -= val;
                } else if (node.value === '*=') {
                    this.scope[identifier] *= val;
                } else if (node.value === '/=') {
                    this.scope[identifier] /= val;
                } else if (node.value === '**=') {
                    this.scope[identifier] = Math.pow(scope[identifier], val);
                } else if (node.value === '//=') {
                    this.scope[identifier] = Math.floor(scope[identifier], val);
                } else if (node.value === '%=') {
                    this.scope[identifier] %= val;
                }
                return this.scope[identifier];
            } else {
                let left = this.do(node.left, level + 1);
                let right = this.do(node.right, level  +1);
                let res = null;
                switch (node.value)
                {
                    case '+':
                        res = left + right;
                        break;
                    case '-':
                        res = left - right;
                        break;
                    case '*':
                        res = left * right;
                        break;
                    case '/':
                        res = left / right;
                        break;
                    case '**':
                        res = Math.pow(left, right);
                        break;
                    case '//':
                        res = Math.floor(left / right);
                        break;
                    case '%':
                        res = left % right;
                        break;
                    case '==':
                        res = left === right;
                        break;
                    case '!=':
                        res = left !== right;
                        break;
                    case '>':
                        res = left > right;
                        break;
                    case '<':
                        res = left < right;
                        break;
                    case '>=':
                        res = left >= right;
                        break;
                    case '<=':
                        res = left <= right;
                        break;
                    case 'and':
                        res = left && right;
                        break;
                    case 'or':
                        res = left || right;
                        break;
                    default:
                        throw new Error(`[ERROR] Unknown binary operator not handled : ${node.value}`);
                }
                console.log('    '.repeat(level) + `Binaryop(${node.value}) ${res}`);
                return res;
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
                if (!(node.value in this.scope)) {
                    console.log('ERROR Scope = ');
                    console.log(this.scope);
                    throw new Error(`Unknown identifier=${node.value} in current scope`);
                }
                console.log('    '.repeat(level) + `Identifier ${node.value} as value  ${this.scope[node.value]}`);
                return this.scope[node.value];
            } else {
                console.log('    '.repeat(level) + `Identifier ${node.value} as identifier`);
                return node.value;
            }
        } else if (node.type === 'String') {
            console.log('    '.repeat(level) + `String ${node.value}`); //no slice(1, .length -1)
            return node.value.slice(1, node.value.length - 1);
        } else {
            throw new Error(`[ERROR] Not handled node: ${node}`);
        }
    }

    library(id, arg)
    {
        let ctx = this.output_screen.getContext("2d");
        switch(id)
        {
            case 'log':
                console.log(this.do(node.right, level + 1));
                return nil;
            // Console output
            case 'writeln':
                return this.output_function(arg.concat(["\n"]));
            case 'write':
                return this.output_function(arg);
            // Screen output
            case 'line': // x1, y1, x2, y2
                ctx.beginPath();
                ctx.moveTo(arg[0], arg[1]);
                ctx.lineTo(arg[2], arg[3]);
                ctx.stroke();
                return null;
            case 'rect': // x, y, w, h
                ctx.beginPath();
                ctx.rect(arg[0], arg[1], arg[2], arg[3]);
                ctx.stroke();
                return null;
            case 'fill': // x, y, w, h
                ctx.fillRect(arg[0], arg[1], arg[2], arg[3]);
                return null;
            case 'circle': // x, y, rayon
                ctx.beginPath();
                ctx.arc(arg[0], arg[1], arg[2], 0, 2 * Math.PI, false);
                ctx.stroke();
                return null;
            case 'text': // x, y, text
                ctx.fillText(arg[2], arg[0], arg[1]);
                return null;
            case 'set_fill':
                ctx.fillStyle = arg[0];
                return null;
            case 'set_stroke':
                ctx.strokeStyle = arg[0];
                return null;
            case 'clear':
                ctx.clearRect(0, 0, screen.width, screen.height);
                return null;
            // System
            case 'exit':
                alert("End of script");
                return null;
            default:
                throw new Error(`[ERROR] Unknown function ${id}`);
        }
    }
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
            let result = GlobalInterpreter.do(cmd);
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

function execute(text) {
    let tokens = new Lexer('ash').lex(text);
    console.log('Tokens:');
    let cpt = 0;
    for (let token of tokens) {
        if (!token.equals("blank")) {
            console.log(`    ${cpt}. ${token}`);
            cpt += 1;
        }
    }
    let res = new Parser().parse(tokens, true);
    console.log(text);
    console.log('AST:');
    console.log(res.toString());
    console.log('Result:');
    let finalRes = GlobalInterpreter.do(res);
    console.log(finalRes);
    return finalRes;
}

function nodeMain(debug = true) {
    Language.readDefinition();
    GlobalInterpreter = new Interpreter();
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
        let res = execute(data);
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
                let result = execute(cmd);
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

export {Interpreter, nil};
