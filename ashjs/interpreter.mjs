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

const VERSION = 0.1;

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
import { Parser, Node } from "./parser.mjs"

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

class Value {
    constructor(identifier, type, value) {
        this.identifier = identifier;
        this.type = type;
        this.value = value;
    }

    setValue(value) {
        console.log(typeof value);
        console.log(Number.isInteger(value));
        if (
            (this.type === "boolean" && typeof value !== "boolean")
            || (this.type === "integer" && (typeof value !== "number" || !Number.isInteger(value)))
            || (this.type === "float" && typeof value !== "number")
            || (this.type === "string" && typeof value !== "string")
        ) {
            let expectedType = typeof value;
            if (expectedType === "number") {
                expectedType = Number.isInteger(value) ? "integer" : "float";
            }
            throw new Error(`[ERROR] Variable ${this.identifier} is of type ${this.type} cannot set to ${value} of type ${expectedType}`);
        }
        this.value = value;
    }

    getValue() {
        return this.value;
    }

    getType() {
        return this.type;
    }
}

class Function extends Value {
    constructor(identifier, type, value, parameters, code) {
        // value is only "procedure" or "function"
        // type is its return type
        super(identifier, type, value);
        this.parameters = parameters;
        this.code = code;
    }

    toString() {
		return (
			`function ${this.identifier} (` +
			this.parameters.map((x) => x.toString()).join(", ") +
			")"
		);
	}

    isProcedure() {
        return this.value === 'procedure';
    }

    call(args) {
        if (args.length != this.parameters.length) {
            throw new Error(`Too many or not enough parameters: expected number is ${this.parameters.length} and ${args.length} were provided.`);
        }
    }
}

let log = new Function(
    'log',
    nil,
    'procedure',
    {
        'o': 'any'
    },
    function (args) {
        console.log(args[0]);
        return nil;
    }
);

let clear = new Function(
    'clear',
    nil,
    'procedure',
    {},
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            context.clearRect(0, 0, 640, 480);
        }
        return nil;
    }
);

let line = new Function(
    'line',
    nil,
    'procedure',
    {},
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            let x1 = args.get(0);
            let y1 = args.get(1);
            let x2 = args.get(2);
            let y2 = args.get(3);
            context.lineWidth = args.get(4);
            context.strokeStyle = args.get(5);
            context.beginPath();
            context.moveTo(x1, y1);
            context.lineTo(x2, y2);
            context.stroke();
        }
        return nil;
    }
);

let circle = new Function(
    'circle',
    nil,
    'procedure',
    {
        'x': 'integer',
        'y': 'integer',
        'r': 'integer',
        'color': 'integer',
        'fill': 'boolean'
    },
    function (args) {
        let context = GlobalInterpreter.getContext();
        if (context !== null) {
            let centerX = args.get(0);
            let centerY = args.get(1);
            let radius = args.get(2);
            let color = args.get(3);
            let full = args.get(4);
            context.beginPath();
            context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
            if (full) {
                context.fillStyle = color;
                context.fill();
            } else {
                context.strokeStyle = color;
                context.stroke();
            }
        }
        return nil;
    }
);

let rect = new Function(
    'rect',
    nil,
    'procedure',
    {
        'x': 'integer',
        'y': 'integer',
        'width': 'integer',
        'height': 'integer',
        'fill': 'boolean'
    },
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            context.fillStyle = args.get(4);
            context.strokeStyle = args.get(4);
            if (args.get(5)) {
                context.fillRect(
                    args.get(0),
                    args.get(1),
                    args.get(2),
                    args.get(3)
                );
            } else {
                context.strokeRect(
                    args.get(0),
                    args.get(1),
                    args.get(2),
                    args.get(3)
                );
            }
        }
        return nil;
    }
);

let draw = new Function(
    'draw',
    nil,
    'procedure',
    {
        'x': 'integer',
        'y': 'integer',
        'image': 'string'
    },
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            context.drawImage(args[2], args[0], args[1]);
        }
        return nil;
    }
);

let text = new Function(
    'text',
    nil,
    'procedure',
    {
        's': 'string'
    },
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            ctx.fillText(args[2], args[0], args[1]);
        }
        return nil;
    }
);

let set_fill = new Function(
    'set_fill',
    nil,
    'procedure',
    {
        'c': 'string'
    },
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            context.fillStyle = args[0];
        }
    }
);

let set_stroke = new Function(
    'set_stroke',
    nil,
    'procedure',
    {
        'c': 'string'
    },
    function (args) {
        context = GlobalInterpreter.getContext();
        if (context !== null) {
            context.strokeStyle = args[0];
        }
    }
);

class Interpreter {
    constructor(output_function = null, output_screen = null) {
        this.root = {};
        this.output_function = output_function == null ? console.log : output_function;
        this.output_screen = output_screen;
        this.scope = {};
        this.debug = false;
    }

    getContext() {
        let canvas = document.getElementById("screen");
        if (canvas !== null) {
            return canvas.getContext("2d");
        }
        return null;
    }

    execute(code, debugLex = false, debugParse = false, debugExecute = false) {
        let tokens = new Lexer('ash').lex(code, null, false, debugLex);
        let node = new Parser().parse(tokens, debugParse);
        this.debug = debugExecute; // Ne sert à rien pour l'instant
        return this.do(node);
    }

    do(node, level = 0, evalId = true) {
        if (node === undefined) {
            throw new Error(`[ERROR] node is undefined.`);
        }
        if (!(node instanceof Node)) {
            throw new Error(`[ERROR] node must be of type Node and found: ${typeof node}.`);
        }
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
            let security = 1000000000;
            while (cond === true) {
                this.do(node.left, level + 1);
                cond = this.do(node.value, level + 1);
                security -= 1;
                if (security === 0) {
                    throw new Error("Infinite loop detected");
                }
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
                let val = -this.do(node.left, level + 1);
                console.log('    '.repeat(level) + `UnaryOp(${node.value}) ${val}`);
                return val;
            } else if (node.value === 'not') {
                let val = this.do(node.left, level + 1);
                if (typeof val !== "boolean") {
                    throw new Error(`[ERROR] Unsupported unary operator ${node.value} for ${typeof val}`);
                }
                val = !val;
                console.log('    '.repeat(level) + `UnaryOp(${node.value}) ${val}`);
                return val;
            } else {
                throw new Error(`[ERROR] Unknown Unary Op: ${node}`)
            }
        } else if (node.type === 'BinaryOp') {
            if (['=', '+=', '-=', '*=', '/=', '//=', '**=', '%='].includes(node.value)) {
                // Left side
                let identifier = this.do(node.left, level + 1, false);
                if (typeof identifier !== 'string') {
                    throw new Error(
                        "[ERROR] Left part of an affectation should be an identifer"
                    );
                }
                let val = this.do(node.right, level + 1);
                if (node.value !== '=' && !(identifier in this.scope)) {
                    throw new Error(`Unknown variable ${identifier} in current scope`);
                } else if (!(identifier in this.scope)) {
                    console.log('    '.repeat(level) + `Declaration ${identifier} ${node.value} ${val}`);
                } else {
                    console.log('    '.repeat(level) + `Affectation ${identifier} ${node.value} ${val}`);
                }
                if (node.value === '=') {
                    if (!(identifier in this.scope)) {
                        let type = "object";
                        if (typeof val === "boolean") {
                            type = 'boolean';
                        } else if (typeof val === "number") {
                            type = Number.isInteger(val) ? 'integer' : 'float';
                        } else if (typeof val === "string") {
                            type = 'string';
                        }
                        this.scope[identifier] = new Value(identifier, type, val);
                    } else {
                        this.scope[identifier].setValue(val);
                    }
                } else if (node.value === '+=') {
                    this.scope[identifier].setValue(this.scope[identifier].getValue() + val);
                } else if (node.value === '-=') {
                    this.scope[identifier].setValue(this.scope[identifier].getValue() - val);
                } else if (node.value === '*=') {
                    this.scope[identifier].setValue(this.scope[identifier].getValue() * val);
                } else if (node.value === '/=') {
                    this.scope[identifier].setValue(this.scope[identifier].getValue() / val);
                } else if (node.value === '**=') {
                    this.scope[identifier].setValue(Math.pow(scope[identifier].getValue(), val));
                } else if (node.value === '//=') {
                    this.scope[identifier].setValue(Math.floor(scope[identifier].getValue(), val));
                } else if (node.value === '%=') {
                    this.scope[identifier].setValue(this.scope[identifier].getValue() % val);
                }
                return this.scope[identifier].getValue();
            } else {
                let left = this.do(node.left, level + 1);
                let right = this.do(node.right, level + 1);
                let res = null;
                if (typeof left === "boolean") {
                    switch (node.value) {
                        case 'and':
                            res = left && right;
                            break;
                        case 'or':
                            res = left || right;
                            break;
                        default:
                            throw new Error(`[ERROR] Unsupported binary operator ${node.value} for boolean`);
                    }
                } else if (typeof left === "string") {
                    switch (node.value) {
                        case '+':
                            if (typeof right !== "string") {
                                throw new Error(`[ERROR] Unsupported binary operator ${node.value} for ${type} with ${typeof right} parameter. Can only add a string to a string.`);
                            }
                            res = left + right;
                            break;
                        case '*':
                            if (typeof right !== "number" || !Number.isInteger(right)) {
                                throw new Error(`[ERROR] Unsupported binary operator ${node.value} for ${type} with ${typeof right} parameter. Can only repeat a string by an integer.`);
                            }
                            res = left.repeat(right);
                            break;
                        default:
                            throw new Error(`[ERROR] Unsupported binary operator ${node.value} for string`);
                    }
                } else if (typeof left === "number") {
                    let type = Number.isInteger(left) ? "integer" : "float";
                    switch (node.value) {
                        case '+':
                            if (typeof right !== "number") {
                                throw new Error(`[ERROR] Unsupported binary operator ${node.value} for ${type} with ${typeof right} parameter`);
                            }
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
                        default:
                            throw new Error(`[ERROR] Unsupported binary operator ${node.value} for ${type}`);
                    }
                } else {
                    throw new Error(`[ERROR] Unsupported type : ${typeof left}`);
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
                    throw new Error(`Unknown identifier |${node.value}| in current scope`);
                }
                console.log('    '.repeat(level) + `Identifier ${node.value} as value  ${this.scope[node.value].getValue()}`);
                return this.scope[node.value].getValue();
            } else {
                console.log('    '.repeat(level) + `Identifier ${node.value} as identifier`);
                return node.value;
            }
        } else if (node.type === 'String') {
            console.log('    '.repeat(level) + `String ${node.value}`); //no slice(1, .length -1)
            return node.value.slice(1, node.value.length - 1);
        } else {
            console.log(node, typeof node);
            console.log(node.type, typeof node.type);
            console.log(node.value, typeof node.value);
            throw new Error(`[ERROR] Not handled node type |${node.type}| for node: ${node}`);
        }
    }

    library(id, args) {
        let base = {
            // Graphic functions
            'clear': clear, 'line': line,
            'circle': circle, 'react': react, 'draw': draw,
            'text': text,
            'set_fill': set_fill, 'set_stroke': set_stroke,
            // Console functions
            'log': log, 'writeln': log
        };
        if (id in base) {
            return base[id](args);
        }
        throw new Error(`[ERROR] Unknown function ${id}`);
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
        ["a + 2", 7],
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
        ["2 + 5 ; 4 + 8", 12],
        ["not false and true", true]
    ];

    for (let i = 0; i < tests.length; i++) {
        let cmd = tests[i][0];
        let expected = tests[i][1];
        if (debug) {
            console.log("===================================");
            console.log(`${i}: ${cmd}. Expected: ${expected}`);
            console.log("===================================");
            let result = GlobalInterpreter.execute(cmd);
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
    } else if (process.argv.length === 4 && process.argv[2] === 'build') {
        let appPath = process.argv[3];
        // find the app
        if (!fs.existsSync(appPath)) {
            throw new Error(`Impossible to find a valid ash file at ${path.join(process.cwd(), appPath)}`);
        }
        let appName = path.basename(appPath);
        let appDirectory = path.dirname(appPath);
        let appBuildDirectory = path.join(appDirectory, 'build');
        console.log(`Building ${appName} application in directory ${appDirectory}`);
        if (!fs.existsSync(appBuildDirectory)) {
            fs.mkdirSync(appBuildDirectory);
        }
        let data = fs.readFileSync("template.html", "utf-8");
        data = data.replace(/%AppName%/g, appName);
        let code = fs.readFileSync(appPath, "utf-8");
        data = data.replace('//%code%', code);
        fs.writeFileSync(path.join(appBuildDirectory, "index.html"), data);
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
            if (cmd !== 'exit' && cmd !== 'vars') {
                let result = execute(cmd);
                if (result !== null) { // notAnExpression
                    console.log(result);
                }
            } else if (cmd === 'vars') {
                if (Object.keys(GlobalInterpreter.scope).length > 0) {
                    for (let [key, val] of Object.entries(GlobalInterpreter.scope)) {
                        console.log(`${key} : ${val.getType()} = ${val.getValue()}`);
                    }
                } else {
                    console.log("No variables");
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

export { Interpreter, nil, VERSION };
