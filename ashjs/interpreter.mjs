//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

import { Expression, Literal, Block, Call } from "./parser.mjs"

//-----------------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------------

class Interpreter
{
    constructor(output_function=null)
    {
        this.root = {};
        this.output_function = output_function == null ? console.log : output_function;
    }

    do(node)
    {
        if (node instanceof Expression)
        {
            if (node.operator.token.is(null, 'affectation') || node.operator.token.is(null, 'combined_affectation'))
            {
                let identifier = node.left.token.getValue();
                let value = this.do(node.right);
                if (identifier in this.root)
                {
                    console.log(`Setting ${identifier} ${node.operator.token.getValue()} ${value}`);
                } else {
                    console.log(`Definition of ${identifier} to ${value}`);
                }
                switch (node.operator.token.getValue())
                {
                    case '=':
                        this.root[identifier] = value;
                        break;
                    case '+=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] += value;
                        break;
                    case '-=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] -= value;
                        break;
                    case '*=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] *= value;
                        break;
                    case '/=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] /= value;
                        break;
                    case '//=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] = Math.floor(this.root[identifier] / value);
                        break;
                    case '**=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] = Math.pow(this.root[identifier], value);
                        break;
                    case '%=':
                        if (!(identifier in this.root))
                        {
                            throw new Error("Unknown variable : " + identifier);
                        }
                        this.root[identifier] %= value;
                        break;
                    default:
                        throw new Error("Not handled affectation operator : " + node.operator.token.getValue());
                }
                return this.root[identifier];
            } else if (node.operator.token.is(null, 'operator')) {
                let left = this.do(node.left);
                let right = this.do(node.right);
                switch (node.operator.token.getValue())
                {
                    case '+':
                        return left + right;
                    case '*':
                        return left * right;
                    case '**':
                        return Math.pow(left, right);
                    default:
                        throw new Error("Not handled operator : " + node.operator.token.getValue());
                }
            } else {
                throw new Error("Expression not handled");
            }
        } else if (node instanceof Call) {
            const id = node.identifier.token.getValue();
            switch(id)
            {
                case 'writeln':
                    const p = this.do(node.params[0]);//hack: only 1 param for now
                    this.output_function(p);
                    return p.toString().length;
                case 'exit':
                    alert("End of script");
                    return null;
                default:
                    throw new Error("Function unknown: " + id);
            }
        } else if (node instanceof Literal) {
            let tok = node.token;
            switch(tok.getType())
            {
                case 'integer':
                    return parseInt(tok.getValue());
                case 'number':
                    return parseFloat(tok.getValue());
                case 'string':
                    return tok.getValue().slice(1, tok.getValue().length - 1); // Remove the "
                default:
                    throw new Error("Not handled type : " + tok.getType());
            }
        } else if (node instanceof Block) {
            let last = null;
            for (const statement of node.statements) {
                last = this.do(statement);
            }
            return last;
        } else {
            throw new Error("Not handled Node type : " + node.constructor.name);
        }
    }
}

//-----------------------------------------------------------------------------
// Exports
//-----------------------------------------------------------------------------

export {Interpreter};
