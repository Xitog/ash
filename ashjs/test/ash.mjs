//-----------------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------------

class NodeList {
	constructor() {
		this.nodes = [];
	}
	unshift(n) {
		this.nodes.unshift(n);
	}
	push(n) {
		this.nodes.push(n);
	}
	get(i) {
		return this.nodes[i];
	}
	getList() {
		return this.nodes;
	}
	getSize() {
		return this.nodes.length;
	}
	toString() {
		return `NodeList of ${this.nodes.length} elements`;
	}
}

class Node {
	constructor(type, value, left = null, right = null) {
		this.type = type;
		this.value = value;
		this.left = left;
		this.right = right;
	}

	// equals("keyword", "if") or equals("keyword", ["if", "while"])
	equals(t, v) {
		if (Array.isArray(v)) {
			return this.type === t && v.includes(this.value);
		}
		return this.type === t && this.value === v;
	}

	toString() {
		if (this.type !== "expr" && this.type !== "suite") {
			return `(t=${this.type}, v=${this.value})`;
		} else {
			return `(t=${this.type}, v=${this.value}, l=${this.left}, r=${this.right})`;
		}
	}

	toHTML() {
		return `${this.value}<sub>${this.type}</sub>`;
	}

	toHTMLTree(isRoot = false) {
		if (this.type !== "expr" && this.type !== "suite") {
			if (isRoot) {
				return `<ul class="monotree"><li data-type="${this.type}"><code>${this.value}</code><ul>`;
			} else {
				return `<code>${this.value}</code>`;
			}
		} else {
			// Handling of suite & expr
			let cls = "";
			if (isRoot) {
				cls = ' class="tree"';
			}
			let val = "";
			if (this.type === "expr") {
				val = this.value.value;
			} else {
				val = "suite";
			}
			let s = `<ul ${cls}><li data-type="${this.type}"><code>${val}</code><ul>`;
			s += "<li>" + this.left.toHTMLTree() + "</li>";
			if (this.right !== null) {
				s += "<li>" + this.right.toHTMLTree() + "</li>";
			}
			s += "</ul></li></ul>";
			return s;
		}
	}
}

//-----------------------------------------------------------------
// Classes for execution
//-----------------------------------------------------------------

class NilClass {
	toString() {
		return "nil";
	}
}

class AshParameter {
	constructor(name, type = "any", def = nil) {
		this.name = name;
		this.type = type;
		this.def = def;
	}

	toString() {
		let extra = "";
		if (this.def !== nil) {
			extra = ` = ${this.def}`;
		}
		return `${this.name} : ${this.type}${extra}`;
	}
}

class AshFunction {
	constructor(name, paramList, code) {
		this.name = name;
		this.paramList = paramList;
		this.code = code;
	}

	toString() {
		return (
			`function ${this.name} (` +
			this.paramList.map((x) => x.toString()).join(", ") +
			")"
		);
	}

	call(argList) {
		if (!(argList instanceof NodeList)) {
			throw new Error(
				`Parameters should be a NodeList and is a ${typeof argList}`
			);
		}
		if (argList.getSize() > this.paramList.length) {
			throw new Error(
				`Too many parameters: maximum expected is ${this.paramList.length} and ${argList.length} were provided.`
			);
		}
		for (let index = 0; index < this.paramList.length; index++) {
			let param = this.paramList[index];
			if (index < argList.getSize()) {
				let arg = argList.get(index);
				if (param.type !== "any") {
					if (param.type === "int") {
						if (
							!(arg instanceof Number) ||
							!Number.isInteger(left)
						) {
							throw new Error(
								`Parameter ${index} should be an integer.`
							);
						}
					} else if (param.type === "flt") {
						if (typeof arg !== "number") {
							throw new Error(
								`Parameter ${arg} at ${index} should be a number and is a ${typeof arg}.`
							);
						}
					} else if (param.type === "bool") {
						if (!(arg instanceof Boolean)) {
							throw new Error(
								`Parameter ${index} should be an boolean.`
							);
						}
					} else if (param.type === "str") {
						if (!(arg instanceof String)) {
							throw new Error(
								`Parameter ${index} should be a string.`
							);
						}
					}
				}
			} else {
				if (param.def === nil) {
					throw new Error(
						`Not enough parameter: ${this} requires ${
							this.paramList.length
						} parameters and ${argList.getSize()} were given.`
					);
				}
			}
		}
		return this.code(argList);
	}
}

class AshLexer {
	lex(code, debug) {
		let nodes = [];
		let word = "";
		let matches = [];
		let old_matches = [];
		for (let index = 0; index < code.length; index += 1) {
			if (debug) {
				console.log(index);
			}
			if (index > 200) {
				throw new Error("too much");
			}
			let c = code[index];
			word += c;
			if (debug) {
				console.log(`Word is |${word}|`);
			}
			matches = [];
			for (const [t, elems] of Object.entries(language.tokens)) {
				for (const e of elems) {
					let res = false;
					if (e instanceof RegExp) {
						if (e.test(word)) {
							if (debug) {
								console.log(
									`matched ${t} with ${e} for ${word}`
								);
							}
							matches.push(new Node(t, word));
							res = true;
						}
					} else if (e === word) {
						if (debug) {
							console.log(`matched ${t}`);
						}
						matches.push(new Node(t, word));
						res = true;
						break;
					}
					if (debug) {
						console.log(`...against |${e}| = ${res}`);
					}
				}
			}
			if (debug) {
				console.log(
					`Matches: ${matches.length} and old: ${old_matches.length}`
				);
			}
			if (matches.length === 0 && old_matches.length > 0) {
				if (debug) {
					console.log(`Adding node! ${old_matches[0]}`);
				}
				nodes.push(old_matches[0]);
				word = "";
				index -= 1;
			}
			old_matches = matches;
		}
		if (debug) {
			console.log(
				`Matches: ${matches.length} and old: ${old_matches.length}`
			);
		}
		if (word.length > 0) {
			if (old_matches.length > 0) {
				nodes.push(old_matches[0]);
				word = "";
			} else {
				throw new Error(`Unlexed string: |${word}|`);
			}
		}
		// filter blanks
		let nn = [];
		for (const n of nodes) {
			if (n.type !== "blank") {
				nn.push(n);
			}
		}
		nodes = nn;
		// retag nodes
		for (const [i, n] of nodes.entries()) {
			if (n.type === "op") {
				n.type = "binop";
				if (n.value === "not") {
					n.type = "unaop";
				} else if (n.value === "-") {
					if (i == 0) {
						n.type = "unaop";
						n.value = "una-";
					} else if (i - 1 >= 0) {
						if (
							["sep", "binop", "unaop"].includes(
								nodes[i - 1].type
							)
						) {
							n.type = "unaop";
							n.value = "una-";
						}
					}
				}
			} else if (n.type === "sep" && n.value === "(") {
				if (i - 1 >= 0) {
					if (nodes[i - 1].type === "id") {
						n.type = "binop";
					}
				}
			}
		}
		//nodes.push(new Node("end", "end"));
		if (debug) {
			console.log("End of lexing", code);
		}
		return nodes;
	}
}

class AshParser {
	constructor() {
		this.index = 0;
		this.nodes = [];
		this.debug = false;
		this.current = null;
		this.level = 0;
	}

	shift(until = null) {
		this.level += 1;
		this.log(`>>> shift until ${until}`);
		if (until === null) {
			this.index += 1;
		} else {
			this.index = until;
		}
		if (this.index >= this.nodes.length) {
			this.current = null;
		} else {
			this.current = this.nodes[this.index];
		}
		this.level -= 1;
		return this.current;
	}

	delimite_if(until) {
		this.level += 1;
		this.log(`>>> delimite_if from=${this.index} max=${until}`);
		let levels = ["if"];
		let results = [];
		for (let i = this.index + 1; i < until; i++) {
			let n = this.nodes[i];
			if (n.equals("keyword", "then") && levels.length === 1) {
				results["then"] = i;
			} else if (n.equals("keyword", "if")) {
				levels.push("if");
			} else if (n.equals("keyword", "end")) {
				levels.pop();
			}
			if (levels.length === 0) {
				results["end"] = i;
				break;
			}
		}
		if (!("then" in results) || !("end" in results)) {
			throw new Error(`Wrong if detected: ${results}`);
		}
		this.level -= 1;
		return results;
	}

	delimite_loop(until) {
		this.level += 1;
		this.log(`>>> delimite_loop from=${this.index} max=${until}`);
		let levels = ["while"];
		let results = [];
		for (let i = this.index + 1; i < until; i++) {
			let n = this.nodes[i];
			if (n.equals("keyword", "do") && levels.length === 1) {
				results["do"] = i;
			} else if (n.equals("keyword", "while")) {
				levels.push("while");
			} else if (n.equals("keyword", "loop")) {
				levels.pop();
			}
			if (levels.length === 0) {
				results["end"] = i;
				break;
			}
		}
		if (!("do" in results) || !("end" in results)) {
			throw new Error(`Wrong loop detected: ${results}`);
		}
		this.level -= 1;
		return results;
	}

	delimite_expr(until) {
		this.level += 1;
		this.log(`>>> delimite_expr from=${this.index} max=${until}`);
		let results = [];
		for (let i = this.index; i < until; i++) {
			let n = this.nodes[i];
			console.log(i, n, until);
			if (n.equals("sep", [";", "\n"])) {
				results["end"] = i;
				break;
			} else if (i === until - 1) {
				results["end"] = until; // We must take the last
			}
		}
		if (!("end" in results)) {
			throw new Error(`Wrong expr detected: ${results}`);
		}
		this.level -= 1;
		return results;
	}

	log(s) {
		if (this.debug) {
			if (Array.isArray(s)) {
				this.log("Affichage liste :");
				for (const [i, e] of s.entries()) {
					this.log(`    ${i}. ${e}`);
				}
			} else {
				console.log("    ".repeat(this.level) + s);
			}
		}
	}

	parse(nodes, debug) {
		this.index = 0;
		this.nodes = nodes;
		this.debug = debug;
		this.current = this.nodes[this.index];
		this.level = 0;
		this.log(nodes);
		return this.parseStart();
	}
	parseStart(until = null) {
		until = until === null ? this.nodes.length : until;
		this.log(`>>> parseStart from ${this.index} to ${until}`);
		let res = null;
		if (this.current.equals("keyword", "if")) {
			res = this.parseIf(until);
		} else if (this.current.equals("keyword", "while")) {
			res = this.parseWhile(until);
		} else {
			res = this.parseSuite(until);
		}
		if (res === null || res === undefined) {
			throw new Error("Something went wrong in parsing. Aborting.");
		}
		return res;
	}
	parseSuite(until) {
		this.level += 1;
		this.log(`>>> parseSuite from ${this.index} max ${until}`);
		let root = null;
		let suite = null;
		let security = 10;
		while (this.index < this.nodes.length) {
			let end = this.delimite_expr(until)["end"];
			this.log(
				`    --- parseSuite from ${this.index} to ${end}`,
				this.nodes.slice(this.index, end)
			);
			if (suite === null) {
				suite = new Node("suite");
				root = suite;
			} else {
				suite.right = new Node("suite");
				suite = suite.right;
			}
			res = this.parseExpression(end);
			suite.left = res;
			this.log("    --- suite = " + suite.toString());
			this.shift(end + 1); // shift all + ending tokens
			security -= 1;
			if (security === 0) {
				throw new Error("Too many iteration. Aborting");
			}
		}
		this.level -= 1;
		return root;
	}
	parseIf(until) {
		this.level += 1;
		let res = this.delimite_if(until);
		if (this.debug) {
			console.log(
				`>>> parseIf from ${this.index} to ${until}, then at=${res["then"]} (max=${until})`
			);
		}
		this.shift(); // remove if
		let condition = this.parseExpression(res["then"]);
		this.shift(res["then"] + 1); // remove then
		let action = this.parseStart(res["end"]);
		this.shift(res["end"] + 1); // remove end
		this.level -= 1;
		return new Node("if", condition, action); // Promoting node from keyword to if
	}
	parseWhile(until) {
		this.level += 1;
		let res = this.delimite_loop(until);
		if (this.debug) {
			console.log(
				`>>> parseWhile from ${this.index} to ${res["end"]}, do at=${res["do"]} (max=${until})`
			);
		}
		this.shift(); // remove while
		let condition = this.parseExpression(res["do"]);
		this.shift(res["do"] + 1); // remove do;
		let action = this.parseStart(res["end"]);
		this.shift(res["end"] + 1); // remove end
		this.level -= 1;
		return new Node("while", condition, action); // Promoting node from keyword to while
	}
	parseExpression(until) {
		this.level += 1;
		// make a local shallow copy
		let nodes = Array.from(this.nodes.slice(this.index, until)); // take also the last element
		if (this.debug) {
			this.log(`>>> parseExpression from ${this.index} to ${until}`);
			this.log(nodes);
		}
		// in advance, we set the index to the end
		this.shift(until);
		let security = 100;
		while (nodes.length > 1 && security > 0) {
			security -= 1;
			let max = 0;
			let index = null;
			let current_level = 1;
			for (const [i, n] of nodes.entries()) {
				// Selection of the most prioritary operator
				if (
					["binop", "unaop"].includes(n.type) &&
					n.value in language.precedences
				) {
					if (this.debug) {
						this.log(
							`Test on ${n} with lvl=${current_level} got ${
								language.precedences[n.value] * current_level
							} and max=${max}`
						);
					}
					// En cas d'égalité on prend le plus à gauche
					// c'est obligatoire pour les opérateurs unaires
					if (language.precedences[n.value] * current_level >= max) {
						if (this.debug) {
							this.log("Taken");
						}
						index = i;
						max = language.precedences[n.value] * current_level;
					}
				}
				// Level of ( ). Warning: ( can be a sep OR a binop for call
				// ( add to current level but must be counted as outside
				if (n.value === "(") {
					current_level *= 10;
				} else if (n.value === ")") {
					current_level /= 10;
				}
			}
			if (current_level !== 1) {
				throw new Error(`Mismatched parenthesis: ${current_level}`);
			}

			if (this.debug && max !== 0) {
				this.log(
					`Index node chosen @${index} ${nodes[index]} with max=${max}`
				);
			}
			if (max === 0) {
				this.log("ERROR");
				this.log(nodes.join(", "));
				throw new Error(`No operator found`);
			}
			// Perimeter
			let current = nodes[index];
			let left,
				right,
				startDeleteAt,
				deleteLength = null;
			if (["binop", "expr"].includes(current.type)) {
				left = nodes[index - 1];
				right = nodes[index + 1];
				startDeleteAt = index - 1;
				deleteLength = 3;
			} else if (current.type === "unaop") {
				left = nodes[index + 1];
				startDeleteAt = index;
				deleteLength = 2;
			} else {
				throw new Error(`Unexpected type: ${current.type}`);
			}
			// Deletion of sep ( )
			if (
				index - 2 >= 0 &&
				nodes[index - 2].value === "(" &&
				nodes[index - 2].type === "sep" && // don't delete ( in function call !
				index + 2 < nodes.length &&
				nodes[index + 2].value === ")"
			) {
				startDeleteAt = index - 2;
				deleteLength += 2;
			}
			// Deletation of sep ) after a calling (
			if (current.type === "binop" && current.value === "(") {
				if (
					index + 2 >= nodes.length ||
					nodes[index + 2].type !== "sep" ||
					nodes[index + 2].value !== ")"
				) {
					throw new Error(
						"Calling opening parenthesis not matched" // Should never be called due to Mismatched parenthesis error before
					);
				} else {
					deleteLength += 1;
				}
			}
			if (this.debug) {
				this.log(
					`Splicing from ${startDeleteAt} with length ${deleteLength}`
				);
			}
			nodes.splice(
				startDeleteAt,
				deleteLength,
				new Node("expr", current, left, right)
			);
			if (this.debug) {
				this.log(nodes);
			}
		}
		if (security === 0) {
			throw new Error("Infinite parsing loop, aborting.");
		}
		if (this.debug) {
			this.log("<<< ret Ending parseExpression");
			this.log(nodes);
		}
		this.level -= 1;
		return nodes[0];
	}
}

class AshInterpreter {
	execute(node, symbol = false, debug = false) {
		if (debug) {
			console.log(`executing ${node}`);
		}
		let r = this.execute_core(node, symbol, debug);
		if (debug) {
			console.log(`result is ${r}`);
		}
		return r;
	}
	execute_core(node, symbol = false, debug = false) {
		if (node.type === "suite") {
			if (node.right === null) {
				return this.execute(node.left, false, debug);
			} else {
				this.execute(node.left);
				return this.execute(node.right, false, debug);
			}
		} else if (node.type === "int") {
			return parseInt(node.value);
		} else if (node.type === "float") {
			return parseFloat(node.value);
		} else if (node.type === "bool") {
			return node.value === "true";
		} else if (node.type === "id") {
			if (!symbol) {
				// Function call without parameters
				if (scope[node.value] instanceof Function) {
					let val = scope[node.value]();
					if (val === undefined || val === null) {
						return "nil"; // replace by nil object
					}
					return val;
				}
				return scope[node.value];
			} else {
				return node.value;
			}
		} else if (node.type === "string") {
			return node.value.substring(1, node.value.length - 1);
		} else if (node.type === "expr") {
			let right =
				node.right === null
					? null
					: this.execute(node.right, false, debug);
			let op = node.value.value;
			// Handling of affectation
			if (op === "=") {
				let symbol = this.execute(node.left, true, debug);
				if (typeof symbol !== "string") {
					throw new Error(
						"Left part of an affectation should be an identifer"
					);
				}
				scope[symbol] = right;
				return right;
			}
			// Handling of calling
			if (op === "(") {
				let symbol = this.execute(node.left, true, debug);
				if (typeof symbol !== "string") {
					throw new Error(
						"Left part of a calling should be an identifer"
					);
				}
				if (!(right instanceof NodeList)) {
					let nx = new NodeList();
					nx.unshift(right);
					right = nx;
				}
				if (scope[symbol] instanceof AshFunction) {
					return scope[symbol].call(right);
				} else if (scope[symbol] instanceof Function) {
					return scope[symbol](right);
				} else {
					throw new Error(`${symbol} is not a function.`);
				}
			}
			let left = this.execute(node.left, false, debug);
			// Handling of list
			if (op === ",") {
				if (right instanceof NodeList) {
					right.unshift(left);
					if (debug) {
						console.log("Adding to NodeList", right);
					}
				} else {
					let nx = new NodeList();
					nx.unshift(right);
					nx.unshift(left);
					right = nx;
					if (debug) {
						console.log("Creating new NodeList", right);
					}
				}
				return right;
			}
			// String ---------------------------------------
			if (typeof left === "string") {
				if (op === "+") {
					if (typeof right !== "string") {
						throw new Error("Can only add a string to a string");
					}
					return left + right;
				} else if (op === "*") {
					if (typeof right !== "number") {
						throw new Error("Can only repeat a string by a number");
					}
					return left.repeat(Math.floor(right));
				} else {
					throw new Error(`Unsupported operator ${op} for string`);
				}
			} else if (typeof left === "number") {
				if (op === "+") {
					return left + right;
				} else if (op === "-") {
					return left - right;
				} else if (op === "*") {
					return left * right;
				} else if (op === "/") {
					return left / right;
				} else if (op === "//") {
					return Math.floor(left / right);
				} else if (op === "**") {
					return Math.pow(left, right);
				} else if (op === "%") {
					return left % right;
				} else if (op === "<") {
					return left < right;
				} else if (op === "<=") {
					return left <= right;
				} else if (op === ">") {
					return left > right;
				} else if (op === ">=") {
					return left >= right;
				} else if (op === "==") {
					return left === right;
				} else if (op === "!=") {
					return left !== right;
				} else if (op === "una-") {
					return -left;
				} else {
					let type = Number.isInteger(left) ? "integer" : "float";
					throw new Error(`Unsupported operator ${op} for ${type}`);
				}
			} else if (typeof left === "boolean") {
				if (op === "not") {
					return !left;
				} else if (op === "and") {
					return left && right;
				} else if (op === "or") {
					return left || right;
				} else {
					throw new Error(`Unsupported operator ${op} for boolean`);
				}
			} else {
				throw new Error(`Unsupported type: ${typeof left}`);
			}
		} else if (node.type === "if") {
			let condition = this.execute(node.value);
			if (condition === true) {
				return this.execute(node.left);
			} else {
				return nil;
			}
		} else if (node.type === "while") {
			let condition = this.execute(node.value);
			let last = nil;
			while (condition === true) {
				last = this.execute(node.left);
				condition = this.execute(node.value);
			}
			return last;
		} else {
			console.log(node, typeof node);
			console.log(node.type, typeof node.type);
			console.log(node.value, typeof node.value);
			throw new Error(
				`Unknown node type |${node.type}| for node ${node}`
			);
		}
	}
}
//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

let language = {
	tokens: {
		op: [
			// Maths
			"+",
			"-",
			"*",
			"/",
			"//",
			"**",
			"%",
			// Booleans
			"not",
			"and",
			"or",
			// Affectation
			"=",
			// Comparisons
			"==",
			"!=",
			"<",
			"<=",
			">=",
			">",
			// List
			",",
		],
		/*
        "%",
        // Specials
        ".",
        "<<",
        "#",
        "$",
        // Booleans
        "not",
        "in",
        "is",
    ],*/
		sep: ["(", ")", "[", "]", ";", "\n"],
		int: [/^[1-9]\d*$/, "0"],
		float: [/^[1-9]\d*\.\d+$/],
		wrong_float: [/^[1-9]\d*\.$/],
		bool: ["true", "false"],
		keyword: ["if", "then", "end", "while", "do", "loop"],
		id: [/^[a-zA-Z_]\w*$/],
		string: [/^"\w*"$/],
		blank: [" "],
	},
	precedences: {
		"(": 9,
		"una-": 8,
		"*": 7,
		"/": 7,
		"//": 7,
		"**": 7,
		"%": 7,
		"+": 6,
		"-": 6,
		">": 5,
		"<": 5,
		">=": 5,
		"<=": 5,
		"==": 5,
		"!=": 5,
		not: 4,
		and: 3,
		or: 3,
		",": 2,
		"=": 1,
	},
};

const nil = new NilClass();

let scope = {
	a: 5,
	b: 2,
	t: true,
	f: false,
	log: new AshFunction("log", [new AshParameter("x", "any")], function (
		args
	) {
		let arg = args.get(0);
		console.log(arg);
		return arg;
	}),
	noarg: function () {
		console.log("fonction sans arg");
		return nil;
	},
	add: new AshFunction(
		"add",
		[new AshParameter("x", "flt"), new AshParameter("y", "flt")],
		function (args) {
			let arg1 = args.get(0);
			let arg2 = args.get(1);
			return arg1 + arg2;
		}
	),
	circle: function (args) {
		console.log("function circle", args);
		// x, y, r, color, full
		if (!(args instanceof NodeList)) {
			throw new Error("Parameters should be a NodeList");
		}
		if (args.getSize() !== 5) {
			throw new Error("Circle takes 5 parameters");
		}
		let centerX = args.get(0);
		let centerY = args.get(1);
		let radius = args.get(2);
		let color = args.get(3);
		let full = args.get(4);
		let canvas = document.getElementById("screen");
		let context = canvas.getContext("2d");
		context.beginPath();
		context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
		if (full) {
			context.fillStyle = color;
			context.fill();
		} else {
			context.strokeStyle = color;
			context.stroke();
		}
		return nil;
	},
	rect: function (args) {
		// x y w h color full
		console.log("function rect", args);
		let canvas = document.getElementById("screen");
		let context = canvas.getContext("2d");
		context.fillStyle = args.get(4);
		context.strokeStyle = args.get(4);
		console.log(context.fillStyle);
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
	},
	clear: function (args) {
		console.log("function clear", args);
		let canvas = document.getElementById("screen");
		let context = canvas.getContext("2d");
		context.clearRect(0, 0, 640, 480);
	},
	line: function (args) {
		console.log("function line", args);
		var canvas = document.getElementById("screen");
		let context = canvas.getContext("2d");
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
	},
};

//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------

function test(name, code, debug_lex, debug_parse, debug_execute) {
	let nodes = new AshLexer().lex(code, debug_lex);
	let container = document.createElement("div");
	// Title
	let title = document.createElement("h1");
	title.innerText = name;
	container.appendChild(title);
	// Input nodes
	let input = document.createElement("pre");
	input.innerHTML = nodes.map((x) => x.toHTML()).join(" "); //.join(", ");
	container.appendChild(input);
	// Parsed nodes
	let ns = new AshParser().parse(nodes, debug_parse);
	if (debug_parse) {
		console.log("Result of parsing:", ns);
	}
	let root = document.createElement("div");
	container.appendChild(root);
	root.innerHTML = ns.toHTMLTree(true);
	if (debug_lex || debug_parse || debug_execute) {
		console.log("= Interpreting ====================================");
	}
	// Result
	let res = document.createElement("pre");
	container.appendChild(res);
	let result = new AshInterpreter().execute(ns, false, debug_execute);
	res.innerHTML = "Result = " + result;
	let boxres = document.getElementById("res");
	boxres.value = result;
	return [container, result];
}

function tests(debug = false) {
	let tests = {
		"Test 1": ["2 + 3 * 5", 17],
		"Test 2": ["(2 + 3) * 5", 25],
		"Test 3": ["2 - 5", -3],
		"Test 4": ["9 / 2", 4.5],
		"Test 5": ["9 // 2", 4],
		"Test 6": ["not false and true", true],
		"Test 7": ["a + 0", 5],
		"Test 8": ["4.2", 4.2],
		"Test 9": ['"abc" + "def"', "abcdef"],
	};
	let output = document.getElementById("output");
	for (const [name, content] of Object.entries(tests)) {
		let cmd = content[0];
		let expected = content[1];
		if (debug) {
			console.log("===================================");
			console.log(`${name}: ${cmd}. Expected: ${expected}`);
			console.log("===================================");
		}
		let result = process(name, cmd);
		if (result !== expected) {
			throw new Error(`Test error. Expected: ${expected} vs ${result}`);
		}
	}
}

function process(name, code) {
	console.clear();
	let output = document.getElementById("output");
	let debug_lex = document.getElementById("debug_lex").checked;
	let debug_parse = document.getElementById("debug_parse").checked;
	let debug_execute = document.getElementById("debug_execute").checked;
	let res = test(name, code, debug_lex, debug_parse, debug_execute);
	let container = res[0];
	let result = res[1];
	output.prepend(container); // appendChild
	return result;
}

export { process, tests };
