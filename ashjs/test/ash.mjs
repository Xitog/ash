class AshFunction {
	constructor(name, paramList, isProcedure, code) {
		this.name = name;
		this.paramList = paramList;
		this.isProcedure = isProcedure;
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
			} else if (param.def === nil) {
                throw new Error(
                    `Not enough parameter: ${this} requires ${this.paramList.length
                    } parameters and ${argList.getSize()} were given.`
                );
            }
		}
		if (typeof this.code === "function") {
			return this.code(argList);
		} else {
			// TODO: parameters
			return GlobalInterpreter.execute(this.code);
		}
	}
}

class AshInterpreter extends AshInstrument {
	constructor(debug = false, info = console.log, error = console.log) {
		super(debug);
		GlobalInterpreter = this;
		document.addEventListener('keydown', (event) => {
			let name = event.key;
			let code = event.code;
			// Alert the key name and key code on keydown
			console.log(`Key down ${name} \r\n Key code value: ${code}`);
			this.KEYS[code] = true;
		}, false);
		document.addEventListener('keyup', (event) => {
			let name = event.key;
			let code = event.code;
			// Alert the key name and key code on keydown
			console.log(`Key up ${name} \r\n Key code value: ${code}`);
			this.KEYS[code] = false;
		}, false);
		if (info !== null) {
			this.info = info;
		} else {
			this.info = console.log;
			info = console.log;
		}
		if (error !== null) {
			this.error = error;
		} else {
			this.error = console.log;
			// Code non utilisé : error = console.log;
		}
		this.KEYS = {};
		this.scope = {
			a: 5,
			b: 2,
			t: true,
			f: false,
			log: new AshFunction(
				"log",
				[new AshParameter("x", "any")],
				true,
				function (args) {
					let arg = args.get(0);
					if (info != console.log) {
						console.log(arg);
					}
					info(arg);
				}
			),
			noarg: function () {
				console.log("fonction sans arg");
				return nil;
			},
			add: new AshFunction(
				"add",
				[new AshParameter("x", "flt"), new AshParameter("y", "flt")],
				false,
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
			keydown: function (args) {
				console.log("function keydown", args);
				let code = args.get(0);
				console.log(GlobalInterpreter.KEYS);
				return (code in GlobalInterpreter.KEYS && GlobalInterpreter.KEYS[code]);
			},
			line: function (args) {
				console.log("function line", args);
				let canvas = document.getElementById("screen");
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
			draw: function (args) {
				console.log("function draw", args);
				let canvas = document.getElementById("screen");
				let context = canvas.getContext("2d");
				let x = args.get(1);
				let y = args.get(2);
				let i = args.get(0);
				if (typeof i === "string") {
					let img;
					if (i in resources) {
						img = resources[i];
						context.drawImage(img, x, y);
					} else {
						img = new Image(32, 32);
						img.onload = function () {
							context.drawImage(img, x, y);
						};
						img.src = i;
						resources[i] = img;
					}
				}
			},
		};
	}

	execute_core(node, symbol = false, debug = false) {
		if (node.type === "suite") {
			if (node.right === null) {
				return this.execute(node.left);
			} else {
				this.execute(node.left);
				return this.execute(node.right);
			}
		} else if (node.type === "function" || node.type === "procedure") {
			this.scope[node.value.value] = new AshFunction(
				node.value.value,
				[],
				node.type === "procedure",
				node.right
			);
			return nil;
		} else if (node.type === "id") {
			if (!symbol) {
				// Function call without parameters
				if (this.scope[node.value] instanceof Function) {
					// Todo Unifier avec AshFunction !
					let val = this.scope[node.value]();
					if (val === undefined || val === null) {
						return nil;
					}
					return val;
				}
				if (node.value in this.scope) {
					return this.scope[node.value];
				} else {
					throw new Error(
						`Variable ${node.value} unknown in current scope.`
					);
				}
			} else {
				return node.value;
			}
		} else if (node.type === "string") {
			return node.value.substring(1, node.value.length - 1);
		} else if (node.type === "expr") {
			let op = node.value.value;
			// Reorganize a += 5 as a = a + 5
			if (["*=", "**=", "/=", "//=", "+=", "-=", "%="].includes(op)) {
				let new_node = new Node(
					"expr",
					new Node("binop", "="),
					node.left,
					new Node(
						"expr",
						new Node("binop", op.replace("=", "")),
						node.left,
						node.right
					)
				);
				return this.execute_core(new_node);
			}
			let right =
				node.right === null
					? null
					: this.execute(node.right, op === ".");
			// Handling of affectation
			if (op === "=") {
				let symbol = this.execute(node.left, true);
				if (typeof symbol !== "string") {
					throw new Error(
						"Left part of an affectation should be an identifer"
					);
				}
				this.scope[symbol] = right;
				return right;
			}
			// Handling of calling
			if (op === "(") {
				let symbol = this.execute(node.left, true);
				if (typeof symbol !== "string") {
					throw new Error(
						"Left part of a calling should be an identifer"
					);
				}
				if (right === null) {
					right = new NodeList();
				} else if (!(right instanceof NodeList)) {
					let nx = new NodeList();
					nx.unshift(right);
					right = nx;
				}
				if (this.scope[symbol] instanceof AshFunction) {
					if (this.scope[symbol].isProcedure) {
						this.scope[symbol].call(right);
						return notAnExpression;
					} else {
						return this.scope[symbol].call(right);
					}
				} else if (this.scope[symbol] instanceof Function) {
					return this.scope[symbol](right);
				} else {
					Object.entries(this.scope).forEach(function (key, val) {
						console.log(key, ":", val);
					});
					throw new Error(
						`${symbol} is not a function but a ${typeof this.scope[
						symbol
						]}.`
					);
				}
			}
			let left = this.execute(node.left);
			// Handling of list
			if (op === ",") {
				if (right instanceof NodeList) {
					right.unshift(left);
					this.log("Adding to NodeList", right);
				} else {
					let nx = new NodeList();
					nx.unshift(right);
					nx.unshift(left);
					right = nx;
					this.log("Creating new NodeList", right);
				}
				return right;
			} else if (typeof left === "object") {
				if (op === ".") {
					if (typeof right === "string") {
						if (!symbol) {
							// Function call without parameters
							if (left[right] instanceof AshFunction) {
								let val = left[right].call(new NodeList()); // TODO
								if (val === undefined || val === null) {
									return nil;
								}
								return val;
							}
							if (right in left) {
								return left[right];
							} else {
								throw new Error(
									`${right} unknown member in ${left}.`
								);
							}
						} else {
							return node.value;
						}
					}
				}
				throw new Error(
					`Unsupported operator ${op} for object with right ${right}`
				);
			} else {
				throw new Error(`Unsupported type: ${typeof left}`);
			}
		} else if (node.type === "import") {
			let name = node.value.value;
			console.log(`Importing ${name}`);
			if (name === "io") {
				this.scope["io"] = {
					read: new AshFunction("read", [], function (args) {
						return reader.question("AAAA:");
					}),
				};
			}
			return nil;
		} else if (node.type === "keyword") {
			if (node.value === "break") {
				throw new Error("break");
			} else {
				throw new Error(
					`Don't know what to do with keyword node ${node.value}`
				);
			}
		}
	}
}
//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

const notAnExpression = new NotAnExpression();

let resources = {};

function AshLex(code, debug_lex = false) {
	if (debug_lex) {
		log(`Lexing ${debug_lex}`);
	}
	return new AshLexer(debug_lex).lex(code);
}

function AshParse(nodes, debug_parse = false) {
	if (debug_parse) {
		log(`Parsing ${debug_parse}`);
	}
	return new AshParser(debug_parse).parse(nodes);
}

function AshExecute(root, debug_execute = false, info = null, error = null) {
	if (debug_execute) {
		log(`Executing ${debug_execute}`);
	}
	return new AshInterpreter(debug_execute, info, error).execute(root);
}

function AshProcess(
	code,
	debug_lex = false,
	debug_parse = false,
	debug_execute = false
) {
	let nodes = AshLex(code, debug_lex);
	let root = AshParse(nodes, debug_parse);
	let result = AshExecute(root, debug_execute);
	return result;
}

function AshTests(debug = false) {
	let tests = {
		"Test 6": ["not false and true", true],
		"Test 7": ["a + 0", 5],
	};
}

function main() {
	// 3: filename or -d
	// 4: filename or -d
	} else if (process.argv.length === 4) {
		if (process.argv[3] === "-d") {
			debug = true;
			filename = process.argv[2];
		} else if (process.argv[2] === "-d") {
			debug = true;
			filename = process.argv[3];
		}
	} else if (process.argv.length === 3) {
		if (process.argv[2] === "-d") {
			debug = true;
		} else {
			filename = process.argv[2];
		}
	} else {
		// Run with script name
		let debug = false;
		if (process.argv.length > 3) {
			if (process.argv[3] === "-d") {
				debug = true;
			}
		}
		//try {
		// Execute
		AshProcess(data, false, false, false);
		//} catch (e) {
		//	console.log(`Error when reading file: ${filename}`);
		//	console.log(e.message);
		//	console.trace();
		//}
	}
}
