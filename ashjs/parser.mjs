function ashPostLexing(tokens)
{
    // retag nodes
    for (const [i, n] of tokens.entries()) {
        if (n.type === "operator") {
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
        } else if (n.type === "separator" && n.value === "(") {
            if (i - 1 >= 0) {
                if (nodes[i - 1].type === "id") {
                    n.type = "binop";
                }
            }
        }
    }
}

class Call extends Node
{
    constructor(identifier, parameters)
    {
        super();
        this.identifier = identifier;
        if (parameters == null || !(parameters instanceof ExpressionList))
        {
            throw new Error("Params should be instanceof ExpressionList");
        }
        this.parameters = parameters;
    }

    display(level=0)
    {
        let out = '    '.repeat(level) + "Call\n";
        out += this.parameters.display(level + 1);
        return out;
    }
}

class ExpressionList extends Node
{
    constructor()
    {
        super();
        this.expressions = [];
    }

    get()
    {
        return this.expressions;
    }

    add(node)
    {
        this.expressions.push(node);
    }

    display(level=0)
    {
        let out = '    '.repeat(level) + "ExpressionList\n";
        if (this.expressions.length === 0)
        {
            out += '    '.repeat(level) + "Empty\n";
        }
        for (let exp of this.expressions)
        {
            out += '    '.repeat(level) + exp.display(level + 1);
        }
        out += "\n";
        return out;
    }
}

class If extends Node
{
    constructor(cond, block, elseblock)
    {
        super();
        this.cond = cond;
        this.block = block;
        this.elseblock = elseblock;
    }

    display(level=0)
    {
        let out = "";
        out += '    '.repeat(level) + 'if\n';
        out += this.cond.display(level + 1);
        out += '    '.repeat(level) + 'then\n';
        out += this.block.display(level + 1) + '\n';
        if (this.elseblock != null)
        {
            out += '    '.repeat(level) + 'else\n';
            out += this.elseblock.display(level + 1) + '\n';
        }
        out += '    '.repeat(level) + 'end\n'
        return out;
    }
}

class While extends Node
{
    constructor(cond, block)
    {
        super();
        this.cond = cond;
        this.block = block;
    }

    display(level=0)
    {
        let out = "";
        out += '    '.repeat(level) + 'while\n';
        out += this.cond.display(level + 1);
        out += '    '.repeat(level) + 'do\n';
        out += this.block.display(level + 1) + '\n';
        out += '    '.repeat(level) + 'end\n'
        return out;
    }
}

class Parser
{
    read(value, type='keyword', optional=false)
    {
        // Length check
        if (this.index >= this.tokens.length)
        {
            if (!optional)
            {
                throw new Error(`Parse error, unexpected end of stream of tokens, expected ${value}, ${type} @${this.index}.`);
            }
            return false;
        }
        // Token check on value and/or type
        const tok = this.tokens[this.index];
        if (!tok.equals(type, value))
        {
            if (!optional)
            {
                throw new Error(`Parse error, expected ${value}, ${type} @${this.index} got ${tok}`);
            } else {
                return false;
            }
        }
        // Display for debug
        if (value !== null)
        {
            console.log('    '.repeat(this.level) + `read ${value} @${this.index}`);
        } else {
            console.log('    '.repeat(this.level) + `read ${type} @${this.index}`);
        }
        // Forward
        this.index += 1;
        return true;
    }

    parse_call()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse call ${this.index}`);
        let expr = this.parse_lit();
        if (this.index < this.tokens.length)
        {
            let tok = this.tokens[this.index];
            if (tok.equals("separator", "("))
            {
                let parameters = new ExpressionList();
                this.read("(", "separator");
                tok = this.tokens[this.index];
                while (!tok.equals("separator", ")")) // function call without parameter
                {
                    let param = this.parse_expr();
                    parameters.add(param);
                    tok = this.tokens[this.index];
                    if (tok.equals("separator", ","))
                    {
                        this.read(",", "separator");
                        tok = this.tokens[this.index];
                    } else if (!tok.equals("separator", ")")) {
                        throw new Error("Syntax error in parameters");
                    }
                }
                this.read(")", "separator");
                expr = new Call(expr, parameters); // expr is the function identifier in this case
            }
        }
        this.level -= 1;
        return expr;
    }

    parse_while()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse while at ${this.index}`);
        this.read('while');
        let cond = this.parse_expr();
        this.read('do');
        let block = this.parse_block();
        this.index += 1;
        this.level -= 1;
        return new While(cond, block);
    }

    parse_if()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse if at ${this.index}`);
        this.read('if');
        let cond = this.parse_expr();
        this.read('then');
        let block = this.parse_block();
        let else_block = null;
        if (this.read('else', 'keyword', true))
        {
            else_block = this.parse_block();
        }
        this.read('end');
        this.level -= 1;
        return new If(cond, block, else_block);
    }

}
