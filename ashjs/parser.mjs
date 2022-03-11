//-----------------------------------------------------------------------------
// AST
//-----------------------------------------------------------------------------

class Node
{

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

    toString()
    {
        return 'Block';
    }
}

class Literal extends Node
{
    constructor(tok)
    {
        super();
        this.token = tok;
    }
}

class If extends Node
{

}

class While extends Node
{

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
            if (tok.is('if'))
            {
                b.add(this.parse_if());
            }
            else if (tok.is(null, 'comment') || tok.is(null, 'newline'))
            {
                this.index += 1;
            }
            else if (tok.is('break'))
            {
                this.index += 1;
                b.add(new Break());
            }
            else if (tok.is('next'))
            {
                this.index += 1;
                b.add(new Next());
            }
            else if (tok.is('while'))
            {
                b.add(this.parse_while());
            }
            else if (tok.is('for'))
            {
                b.add(this.parse_for());
            }
            else if (tok.is(null, 'identifier') ||
                     tok.is(null, 'string')     ||
                     tok.is(null, 'integer')    ||
                     tok.is(null, 'number')     ||
                     tok.is(null, 'boolean'))
            {
                b.add(this.parse_expr());
            }
            else if (tok.is(null, 'newline'))
            {
                index += 1;
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
        if (!tok.is(value, type))
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
        return this.parse_test();
    }

    parse_test()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse test at ${this.index}`); // with operator at ${this.index} : ${tok}`);
        let expr = this.parse_shift();
        const tok = this.tokens[this.index];
        if (['==', '!=', '<', '<=', '>=', '>'].includes(tok.getValue()))
        {
            let operator = this.parse_lit();
            let right = this.parse_shift();
            expr = new Expression(expr, operator, right);
        }
        this.level -= 1;
        return expr;
    }

    parse_shift()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `${this.level}. parse shift at ${this.index}`);
        let res = this.parse_lit();
        this.level -= 1;
        return res;
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

    parse_if()
    {
        this.level += 1;
        console.log('    '.repeat(this.level) + `parse if at ${this.index - 1}, level ${this.level}`);
        this.parse_expr();
        this.read('then');
        this.parse_block();
        if (this.read('else', 'keyword', true))
        {
            this.parse_block();
        }
        this.level -= 1;
        this.read('end');
    }

}

//-----------------------------------------------------------------------------
// Exports
//-----------------------------------------------------------------------------

export {Parser, Node, Block, If, While, For, Break, Next, Expression};