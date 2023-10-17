class AshFunction {
	call(argList) {
		if (typeof this.code === "function") {
			return this.code(argList);
		} else {
			return GlobalInterpreter.execute(this.code);
		}
	}
}

	function execute_core(node, symbol = false, debug = false) {
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
								let val = left[right].call(new NodeList());
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
