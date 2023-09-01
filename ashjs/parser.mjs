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

//-------------------------------------------------------------------------------
// Import
//-------------------------------------------------------------------------------

import { Token } from "./lexer.mjs";

function ashPostLexing(tokens)
{
    // retag nodes
    for (const [i, n] of tokens.entries()) {
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
}

class Node extends Token
{
    display(level=0)
    {
        return '    '.repeat(level) + "Node";
    }
}

class Block extends Node
{
    constructor()
    {
        super();
        this.statements = [];
    }

    add(node)
    {
        this.statements.push(node);
    }

    display(level=0)
    {
        let out = '    '.repeat(level) + "Block\n";
        for (let sta of this.statements)
        {
            out += sta.display(level + 1) + "\n";
        }
        return out;
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

class Parameter extends Node
{
    constructor(param, next)
    {
        super();
        this.param = param;
        this.next = next;
    }
}

class Literal extends Node
{
    constructor(tok)
    {
        super();
        this.token = tok;
    }

    display(level)
    {
        return '    '.repeat(level) + this.token.toString() + "\n";
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

class For extends Node
{

}

class Break extends Node
{

}

class Next extends Node
{

}

class Expression extends Node
{
    constructor(left, operator, right)
    {
        super();
        this.left = left;
        this.operator = operator;
        this.right = right;
    }

    display(level)
    {
        let out = '    '.repeat(level) + "Expression\n";
        out += '    '.repeat(level + 1) + 'left:  ' + this.left.display(level + 1);
        out += '    '.repeat(level + 1) + 'op:    ' + this.operator.display(level + 1);
        out += '    '.repeat(level + 1) + 'right: ' + this.right.display(level + 1);
        return out;
    }
}

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------


class Parser
{
    constructor()
    {
        this.index = 0;
        this.level = 0;
        this.tokens = [];
    }

    parse(tokens)
    {
        this.tokens = tokens;
        let res = this.parse_block();
        while (this.index < this.tokens.length)
        {
            const tok = this.tokens[this.index];
            console.log(`Unhandled token ${tok}`);
            this.index += 1;
        }
        return res;
    }

    parse_scope()
    {
        // function
        // procedure
    }

    parse_block()
    {
        // if
        // while
        // for
        // break
        // next
        // expression
        // affectation
        // comment
        // newline
        let b = new Block();
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse block at ${this.index}`);
        while (this.index < this.tokens.length)
        {
            let tok = this.tokens[this.index];
            let next = (this.index + 1) < this.tokens.length ? this.tokens[this.index+1] : null;
            if (tok.equals("keywords", 'if'))
            {
                b.add(this.parse_if());
            }
            else if (tok.equals('comment') || tok.equals('newline'))
            {
                this.index += 1;
            }
            else if (tok.equals('keyword', 'break'))
            {
                this.index += 1;
                b.add(new Break());
            }
            else if (tok.equals('keyword', 'next'))
            {
                this.index += 1;
                b.add(new Next());
            }
            else if (tok.equals('keyword', 'while'))
            {
                b.add(this.parse_while());
            }
            else if (tok.equals('keyword', 'for'))
            {
                b.add(this.parse_for());
            }
            else if (tok.equals('identifier')
                     && next !== null
                     && (next.equals('affectation') || next.equals('combined_affectation')))
            {
                b.add(this.parse_affectation());
            }
            else if (tok.equals('identifier') ||
                     tok.equals('string')     ||
                     tok.equals('integer')    ||
                     tok.equals('number')     ||
                     tok.equals('special')    ||
                     tok.equals('boolean'))
            {
                b.add(this.parse_expr());
            }
            else
            {
                break;
            }
        }
        this.level -= 1;
        return b;
    }

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

    parse_expr()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse expr at ${this.index}`);
        let node = this.parse_bool();
        this.level -= 1;
        return node;
    }

    parse_bool()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse bool at ${this.index}`);
        let expr = this.parse_test();
        if (this.index < this.tokens.length)
        {
            const tok = this.tokens[this.index];
            if(['and', 'or'].includes(tok.getValue()))
            {
                let operator = this.parse_lit();
                let right = this.parse_test();
                expr = new Expression(expr, operator, right);
            }
        }
        this.level -= 1;
        return expr;
    }

    parse_test()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse test at ${this.index}`); // with operator at ${this.index} : ${tok}`);
        let expr = this.parse_shift();
        if (this.index < this.tokens.length)
        {
            const tok = this.tokens[this.index];
            if (['==', '!=', '<', '<=', '>=', '>'].includes(tok.getValue()))
            {
                let operator = this.parse_lit();
                let right = this.parse_shift();
                expr = new Expression(expr, operator, right);
            }
        }
        this.level -= 1;
        return expr;
    }

    parse_shift()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse shift at ${this.index}`);
        let res = this.parse_add_sub();
        this.level -= 1;
        return res;
    }

    parse_add_sub()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse add or sub at ${this.index}`);
        let expr = this.parse_mul_div_mod();
        let tok = this.tokens[this.index];
        while (tok != null && (tok.equals('operator', '+') || tok.equals('operator', '-')))
        {
            let operator = this.parse_lit();
            let right = this.parse_mul_div_mod();
            expr = new Expression(expr, operator, right);
            if (this.index < this.tokens.length)
            {
                tok = this.tokens[this.index];
            } else {
                tok = null;
            }
        }
        this.level -= 1;
        return expr;
    }

    parse_mul_div_mod()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse mul, div, intdiv or mod at ${this.index}`);
        let expr = this.parse_pow();
        let tok = this.tokens[this.index];
        while (tok != null && (
                tok.equals('operator', '*')
                || tok.equals('operator', '/')
                || tok.equals('operator', '//')
                || tok.equals('operator', '%')
            ))
        {
            let operator = this.parse_lit();
            let right = this.parse_pow();
            expr = new Expression(expr, operator, right);
            if (this.index < this.tokens.length)
            {
                tok = this.tokens[this.index];
            } else {
                tok = null;
            }
        }
        this.level -= 1;
        return expr;
    }

    // /!\ Right to left associative
    parse_pow()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse pow ${this.index}`);
        let expr = this.parse_call();
        let tok = this.tokens[this.index];
        while (tok?.equals('operator', '**'))
        {
            let operator = this.parse_lit();
            let right = this.parse_pow();
            expr = new Expression(expr, operator, right);
            if (this.index < this.tokens.length)
            {
                tok = this.tokens[this.index];
            } else {
                tok = null;
            }
        }
        this.level -= 1;
        return expr;
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

    parse_lit()
    {
        this.level += 1;
        const tok = this.tokens[this.index];
        console.log('    '.repeat(this.level) + `${this.level}. parse literal at ${this.index} : ${tok}`);
        this.index += 1;
        this.level -= 1;
        return new Literal(tok);
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

    parse_affectation()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse affectation at ${this.index}`);
        let id = this.parse_lit();
        let op = this.parse_lit(); // Read = += -=  *= /= //= **= %=
        let expr = this.parse_expr();
        return new Expression(id, op, expr);
    }

}

//-----------------------------------------------------------------------------
// Exports
//-----------------------------------------------------------------------------

export {Parser, Node, Block, If, While, For, Break, Next, Expression, Literal, Call, ExpressionList};
