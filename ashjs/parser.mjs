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
        this.statements = [];
    }

    add(node)
    {
        this.statements.push(node);
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
        this.parse_block();
        while (this.index < this.tokens.length)
        {
            console.log(`Unhandled token ${tok}`);
            this.index += 1;
        }
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
        console.log('    '.repeat(this.level) + `parse block level ${this.level}`);
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
        this.level += 1;
        console.log('    '.repeat(this.level) + `parse expr at ${this.index}, level ${this.level}`);
        this.parse_lit();
        this.level -= 1;
    }

    parse_lit()
    {
        console.log('    '.repeat(this.level) + `parse literal`)
        this.read(null, 'boolean');
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