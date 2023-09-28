class AshFunction {
	call(argList) {
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
	constructor()
	{
		super();
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
			keydown: function (args) {
				console.log("function keydown", args);
				let code = args.get(0);
				console.log(GlobalInterpreter.KEYS);
				return (code in GlobalInterpreter.KEYS && GlobalInterpreter.KEYS[code]);
			},
		};
	}

	execute_core(node, symbol = false, debug = false) {
		if (node.type === "function" || node.type === "procedure") {
			this.scope[node.value.value] = new AshFunction(
				node.value.value,
				[],
				node.type === "procedure",
				node.right
			);
			return nil;
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
const notAnExpression = new NotAnExpression();
		//try {
		// Execute
		AshProcess(data, false, false, false);
		//} catch (e) {
		//	console.log(`Error when reading file: ${filename}`);
		//	console.log(e.message);
		//	console.trace();
		//}
